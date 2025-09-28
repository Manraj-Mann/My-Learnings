#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <cstdint>
#include "mhla.h"

struct FileMetaData {
    std::string file_type; // e.g., "image/jpeg"
    std::string original_name;
    size_t original_size;
    // Add more fields as needed
};

class FileHandler {
public:
    // Detects file type (very basic, can be improved)
    static FileMetaData getMetaData(const std::string& file_path);

    // Encrypts file with meta data header
    static bool encryptFile(const std::string& file_path, const std::string& out_path, MHLA& crypto_system);

    // Decrypts file using meta data header
    static bool decryptFile(const std::string& enc_path, const std::string& out_path, MHLA& crypto_system);
};
