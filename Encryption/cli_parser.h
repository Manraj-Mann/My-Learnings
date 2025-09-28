#pragma once
#include <string>
#include <vector>
#include <map>

struct CommandLineArgs {
    enum Mode {
        UNKNOWN,
        ENCRYPT_FILE,
        DECRYPT_FILE,
        ENCRYPT_FOLDER,
        DECRYPT_FOLDER,
        HELP,
        VERSION
    };

    Mode mode = UNKNOWN;
    std::string inputPath;
    std::string outputPath;
    bool verbose = false;
    bool keepOriginal = true;
    bool recursive = true;
    std::vector<std::string> fileTypes;
    std::string configFile;
    
    // Encryption parameters
    struct {
        std::vector<std::vector<int>> matrix;
        std::vector<int> key;
        int modulus = 256;
    } crypto;
};

class CommandLineParser {
public:
    static void printHelp();
    static void printVersion();
    static CommandLineArgs parse(int argc, char* argv[]);

private:
    static void parseEncryptionConfig(const std::string& configFile, CommandLineArgs& args);
    static void validateArgs(CommandLineArgs& args);
    static std::map<std::string, std::string> parseConfigFile(const std::string& path);
};