#include "file_handler.h"
#include <cstring>
#include <algorithm>
#include <cstdint>

// Helper: get file extension
static std::string get_extension(const std::string& filename) {
    auto pos = filename.find_last_of('.');
    if (pos == std::string::npos) return "unknown";
    return filename.substr(pos + 1);
}

FileMetaData FileHandler::getMetaData(const std::string& file_path) {
    FileMetaData meta;
    meta.original_name = file_path;
    std::ifstream file(file_path, std::ios::binary | std::ios::ate);
    meta.original_size = file.tellg();
    file.close();
    std::string ext = get_extension(file_path);
    if (ext == "jpg" || ext == "jpeg") meta.file_type = "image/jpeg";
    else if (ext == "png") meta.file_type = "image/png";
    else meta.file_type = "unknown";
    return meta;
}

// Helper: serialize metadata to a string with robust delimiter
static std::string serializeMeta(const FileMetaData& meta) {
    // Use a delimiter that's unlikely to appear in filenames
    const char DELIM = '|';
    return meta.file_type + DELIM + 
           meta.original_name + DELIM + 
           std::to_string(meta.original_size) + DELIM;
}

// Helper: deserialize metadata from a string
static FileMetaData deserializeMeta(const std::string& str) {
    FileMetaData meta{};
    try {
        const char DELIM = '|';
        size_t p1 = str.find(DELIM);
        size_t p2 = str.find(DELIM, p1 + 1);
        size_t p3 = str.find(DELIM, p2 + 1);
        
        if (p1 == std::string::npos || p2 == std::string::npos || p3 == std::string::npos) {
            throw std::runtime_error("Invalid metadata format");
        }
        
        meta.file_type = str.substr(0, p1);
        meta.original_name = str.substr(p1 + 1, p2 - p1 - 1);
        std::string size_str = str.substr(p2 + 1, p3 - p2 - 1);
        
        // Remove any non-digit characters that might have been corrupted
        size_str.erase(std::remove_if(size_str.begin(), size_str.end(), 
                                    [](char c) { return !std::isdigit(c); }), 
                      size_str.end());
                      
        if (size_str.empty()) {
            throw std::runtime_error("Invalid file size in metadata");
        }
        
        meta.original_size = std::stoull(size_str);
    } catch (const std::exception& e) {
        std::cerr << "Error deserializing metadata: " << e.what() << std::endl;
        // Return empty metadata rather than crashing
        meta.file_type = "unknown";
        meta.original_name = "recovered_file";
        meta.original_size = 0; // Will be set correctly later
    }
    return meta;
}

// Encrypt file with meta data header (header is encrypted, then written as prefix)
bool FileHandler::encryptFile(const std::string& file_path, const std::string& out_path, MHLA& crypto_system) {
    FileMetaData meta = getMetaData(file_path);
    std::string meta_str = serializeMeta(meta);
    std::vector<unsigned char> meta_bytes(meta_str.begin(), meta_str.end());
    Vector meta_vec(meta_bytes.begin(), meta_bytes.end());
    Vector enc_meta = crypto_system.encrypt(meta_vec);
    // Read file data
    std::ifstream file(file_path, std::ios::binary);
    std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(file), {});
    file.close();
    Vector file_vec(buffer.begin(), buffer.end());
    Vector enc_file;
    size_t block_size = crypto_system.P.size();
    for (size_t i = 0; i < file_vec.size(); i += block_size) {
        Vector block(file_vec.begin() + i, file_vec.begin() + std::min(i + block_size, file_vec.size()));
        while (block.size() < block_size) block.push_back(0);
        Vector enc = crypto_system.encrypt(block);
        enc_file.insert(enc_file.end(), enc.begin(), enc.end());
    }
    // Write header size, encrypted header, then encrypted file
    std::ofstream out(out_path, std::ios::binary);
    uint32_t meta_size = enc_meta.size();
    out.write(reinterpret_cast<const char*>(&meta_size), sizeof(meta_size));
    for (int v : enc_meta) {
        unsigned char b = static_cast<unsigned char>(v % 256);
        out.write(reinterpret_cast<const char*>(&b), 1);
    }
    for (int v : enc_file) {
        unsigned char b = static_cast<unsigned char>(v % 256);
        out.write(reinterpret_cast<const char*>(&b), 1);
    }
    out.close();
    return true;
}

// Decrypt file using meta data header
bool FileHandler::decryptFile(const std::string& enc_path, const std::string& out_path, MHLA& crypto_system) {
    try {
        std::ifstream in(enc_path, std::ios::binary);
        if (!in) {
            std::cerr << "Failed to open encrypted file: " << enc_path << std::endl;
            return false;
        }

        // Read metadata size
        uint32_t meta_size = 0;
        if (!in.read(reinterpret_cast<char*>(&meta_size), sizeof(meta_size))) {
            std::cerr << "Failed to read metadata size" << std::endl;
            return false;
        }

        if (meta_size == 0 || meta_size > 1024 * 1024) { // Sanity check: max 1MB metadata
            std::cerr << "Invalid metadata size: " << meta_size << std::endl;
            return false;
        }

        // Read and decrypt metadata
        std::vector<unsigned char> enc_meta_bytes(meta_size);
        if (!in.read(reinterpret_cast<char*>(enc_meta_bytes.data()), meta_size)) {
            std::cerr << "Failed to read metadata content" << std::endl;
            return false;
        }

        Vector enc_meta(enc_meta_bytes.begin(), enc_meta_bytes.end());
        Vector dec_meta = crypto_system.decrypt(Vector(enc_meta.begin(), enc_meta.begin() + crypto_system.P.size()));
        std::string meta_str(dec_meta.begin(), dec_meta.end());
        FileMetaData meta = deserializeMeta(meta_str);

        std::cout << "Metadata decoded: type=" << meta.file_type 
                << ", name=" << meta.original_name 
                << ", size=" << meta.original_size << std::endl;

        // Read encrypted file content
        std::vector<unsigned char> enc_file_bytes;
        char buffer[4096];
        while (in.read(buffer, sizeof(buffer))) {
            enc_file_bytes.insert(enc_file_bytes.end(), buffer, buffer + sizeof(buffer));
        }
        enc_file_bytes.insert(enc_file_bytes.end(), buffer, buffer + in.gcount());
        in.close();

        // Process in blocks matching the matrix size
        Vector dec_file;
        size_t block_size = crypto_system.P.size();
        Vector enc_file(enc_file_bytes.begin(), enc_file_bytes.end());

        for (size_t i = 0; i < enc_file.size(); i += block_size) {
            if (i + block_size <= enc_file.size()) {
                Vector block(enc_file.begin() + i, enc_file.begin() + i + block_size);
                Vector dec = crypto_system.decrypt(block);
                dec_file.insert(dec_file.end(), dec.begin(), dec.end());
            }
        }

        if (dec_file.empty()) {
            std::cerr << "Decryption produced no data" << std::endl;
            return false;
        }

        // Resize to original file size
        if (meta.original_size > 0 && meta.original_size <= dec_file.size()) {
            dec_file.resize(meta.original_size);
        }

        // Write decrypted data
        std::ofstream out(out_path, std::ios::binary);
        if (!out) {
            std::cerr << "Failed to create output file: " << out_path << std::endl;
            return false;
        }

        for (int v : dec_file) {
            unsigned char b = static_cast<unsigned char>(v % 256);
            out.write(reinterpret_cast<const char*>(&b), 1);
        }
        out.close();
        std::cout << "Successfully wrote " << dec_file.size() << " bytes to " << out_path << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error during decryption: " << e.what() << std::endl;
        return false;
    }
    return true;
}
