#pragma once
#include "file_handler.h"
#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;

struct ProcessedFileInfo {
    std::string originalPath;
    std::string processedPath;
    bool success;
    std::string error;
};

class FolderHandler {
public:
    static std::vector<ProcessedFileInfo> encryptFolder(
        const std::string& inputFolder,
        const std::string& outputFolder,
        MHLA& crypto_system
    );

    static std::vector<ProcessedFileInfo> decryptFolder(
        const std::string& inputFolder,
        const std::string& outputFolder,
        MHLA& crypto_system
    );

private:
    static bool createDirectoryIfNotExists(const std::string& path);
    static bool isImageFile(const std::string& path);
    static std::string getRelativePath(const std::string& basePath, const std::string& fullPath);
    static std::string maintainFolderStructure(
        const std::string& baseInputPath,
        const std::string& baseOutputPath,
        const std::string& currentPath
    );
};