#include "folder_handler.h"
#include <iostream>
#include <cctype>

std::vector<ProcessedFileInfo> FolderHandler::encryptFolder(
    const std::string& inputFolder,
    const std::string& outputFolder,
    MHLA& crypto_system
) {
    std::vector<ProcessedFileInfo> results;
    
    if (!createDirectoryIfNotExists(outputFolder)) {
        std::cerr << "Failed to create output directory: " << outputFolder << std::endl;
        return results;
    }

    try {
        for (const auto& entry : fs::recursive_directory_iterator(inputFolder)) {
            if (!entry.is_regular_file()) continue;
            
            const auto& path = entry.path();
            ProcessedFileInfo info{};
            info.originalPath = path.string();
            
            // Maintain folder structure in output
            std::string relPath = getRelativePath(inputFolder, path.string());
            info.processedPath = maintainFolderStructure(inputFolder, outputFolder, path.string());
            info.processedPath += ".encrypted";

            std::cout << "Processing: " << relPath << " -> " << info.processedPath << std::endl;

            try {
                if (FileHandler::encryptFile(path.string(), info.processedPath, crypto_system)) {
                    info.success = true;
                } else {
                    info.success = false;
                    info.error = "Encryption failed";
                }
            } catch (const std::exception& e) {
                info.success = false;
                info.error = e.what();
            }

            results.push_back(info);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error processing folder: " << e.what() << std::endl;
    }

    return results;
}

std::vector<ProcessedFileInfo> FolderHandler::decryptFolder(
    const std::string& inputFolder,
    const std::string& outputFolder,
    MHLA& crypto_system
) {
    std::vector<ProcessedFileInfo> results;
    
    if (!createDirectoryIfNotExists(outputFolder)) {
        std::cerr << "Failed to create output directory: " << outputFolder << std::endl;
        return results;
    }

    try {
        for (const auto& entry : fs::recursive_directory_iterator(inputFolder)) {
            if (!entry.is_regular_file()) continue;
            
            const auto& path = entry.path();
            if (path.extension() != ".encrypted") continue;

            ProcessedFileInfo info{};
            info.originalPath = path.string();
            
            // Maintain folder structure in output but remove .encrypted extension
            std::string relPath = getRelativePath(inputFolder, path.string());
            info.processedPath = maintainFolderStructure(inputFolder, outputFolder, path.string());
            // Remove .encrypted extension if present
            std::string suffix = ".encrypted";
            if (info.processedPath.length() > suffix.length() &&
                info.processedPath.substr(info.processedPath.length() - suffix.length()) == suffix) {
                info.processedPath = info.processedPath.substr(0, info.processedPath.length() - suffix.length());
            }

            std::cout << "Processing: " << relPath << " -> " << info.processedPath << std::endl;

            try {
                if (FileHandler::decryptFile(path.string(), info.processedPath, crypto_system)) {
                    info.success = true;
                } else {
                    info.success = false;
                    info.error = "Decryption failed";
                }
            } catch (const std::exception& e) {
                info.success = false;
                info.error = e.what();
            }

            results.push_back(info);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error processing folder: " << e.what() << std::endl;
    }

    return results;
}

bool FolderHandler::createDirectoryIfNotExists(const std::string& path) {
    try {
        // Check if the path already exists
        if (fs::exists(path)) {
            if (!fs::is_directory(path)) {
                std::cerr << "Path exists but is not a directory: " << path << std::endl;
                return false;
            }
            return true;
        }

        // Create all necessary parent directories
        bool success = fs::create_directories(path);
        if (!success) {
            std::cerr << "Failed to create directory structure for: " << path << std::endl;
            // Check parent directory permissions
            auto parent = fs::path(path).parent_path();
            if (!fs::exists(parent)) {
                std::cerr << "Parent directory does not exist: " << parent << std::endl;
            } else if (!fs::is_directory(parent)) {
                std::cerr << "Parent path exists but is not a directory: " << parent << std::endl;
            }
        }
        return success;
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error creating directory " << path << ": " << e.what() << std::endl;
        return false;
    } catch (const std::exception& e) {
        std::cerr << "Error creating directory " << path << ": " << e.what() << std::endl;
        return false;
    }
}

bool FolderHandler::isImageFile(const std::string& path) {
    std::string ext = fs::path(path).extension().string();
    // Convert to lowercase without std::transform
    std::string lower_ext;
    for (char c : ext) {
        lower_ext += std::tolower(c);
    }
    return lower_ext == ".jpg" || lower_ext == ".jpeg" || lower_ext == ".png" || lower_ext == ".gif" || 
           lower_ext == ".bmp" || lower_ext == ".tiff" || lower_ext == ".webp";
}

std::string FolderHandler::getRelativePath(const std::string& basePath, const std::string& fullPath) {
    fs::path full = fs::canonical(fullPath);
    fs::path base = fs::canonical(basePath);
    return full.lexically_relative(base).string();
}

std::string FolderHandler::maintainFolderStructure(
    const std::string& baseInputPath,
    const std::string& baseOutputPath,
    const std::string& currentPath
) {
    fs::path relative = fs::path(currentPath).lexically_relative(baseInputPath);
    fs::path outputPath = fs::path(baseOutputPath) / relative;
    
    // Create parent directory if it doesn't exist
    fs::create_directories(outputPath.parent_path());
    
    return outputPath.string();
}