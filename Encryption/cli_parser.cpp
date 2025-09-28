#include "cli_parser.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdexcept>

void CommandLineParser::printHelp() {
    std::cout << "MHLA Encryption Utility v1.0\n"
              << "Usage: mhl_demo [options] <command> <input> [output]\n\n"
              << "Commands:\n"
              << "  -e, --encrypt       Encrypt a file or folder\n"
              << "  -d, --decrypt       Decrypt a file or folder\n\n"
              << "Options:\n"
              << "  -o, --output PATH   Output path (default: input path + .encrypted/.decrypted)\n"
              << "  -r, --recursive     Process folders recursively (default: true)\n"
              << "  -v, --verbose       Show detailed progress\n"
              << "  -k, --keep          Keep original files (default: true)\n"
              << "  -t, --type TYPE     File types to process (default: jpg,jpeg,png)\n"
              << "  -c, --config FILE   Use encryption parameters from config file\n"
              << "  -h, --help          Show this help message\n"
              << "      --version       Show version information\n\n"
              << "Examples:\n"
              << "  mhl_demo -e image.jpg\n"
              << "  mhl_demo --encrypt --output /encrypted /path/to/images\n"
              << "  mhl_demo -d -o /decrypted /path/to/encrypted\n"
              << "  mhl_demo -e -t \"jpg,png,gif\" -c crypto.conf /images\n";
}

void CommandLineParser::printVersion() {
    std::cout << "MHLA Encryption Utility v1.0\n"
              << "Copyright (c) 2025\n"
              << "Quantum-Resistant Cryptographic Implementation\n";
}

CommandLineArgs CommandLineParser::parse(int argc, char* argv[]) {
    CommandLineArgs args;
    
    if (argc < 2) {
        args.mode = CommandLineArgs::HELP;
        return args;
    }

    std::vector<std::string> arguments(argv + 1, argv + argc);
    
    for (size_t i = 0; i < arguments.size(); ++i) {
        const std::string& arg = arguments[i];
        
        if (arg == "-h" || arg == "--help") {
            args.mode = CommandLineArgs::HELP;
            return args;
        }
        
        if (arg == "--version") {
            args.mode = CommandLineArgs::VERSION;
            return args;
        }
        
        if (arg == "-v" || arg == "--verbose") {
            args.verbose = true;
            continue;
        }
        
        if (arg == "-k" || arg == "--keep") {
            args.keepOriginal = true;
            continue;
        }
        
        if (arg == "-r" || arg == "--recursive") {
            args.recursive = true;
            continue;
        }
        
        if (arg == "-t" || arg == "--type") {
            if (i + 1 < arguments.size()) {
                std::string types = arguments[++i];
                std::stringstream ss(types);
                std::string type;
                while (std::getline(ss, type, ',')) {
                    args.fileTypes.push_back(type);
                }
            }
            continue;
        }
        
        if (arg == "-c" || arg == "--config") {
            if (i + 1 < arguments.size()) {
                args.configFile = arguments[++i];
                parseEncryptionConfig(args.configFile, args);
            }
            continue;
        }
        
        if (arg == "-o" || arg == "--output") {
            if (i + 1 < arguments.size()) {
                args.outputPath = arguments[++i];
            }
            continue;
        }
        
        if (arg == "-e" || arg == "--encrypt") {
            if (i + 1 < arguments.size()) {
                args.inputPath = arguments[++i];
                args.mode = CommandLineArgs::ENCRYPT_FILE;
            }
            continue;
        }
        
        if (arg == "-d" || arg == "--decrypt") {
            if (i + 1 < arguments.size()) {
                args.inputPath = arguments[++i];
                args.mode = CommandLineArgs::DECRYPT_FILE;
            }
            continue;
        }
        
        // If we get here, treat as input path if not set
        if (args.inputPath.empty()) {
            args.inputPath = arg;
        } else if (args.outputPath.empty()) {
            args.outputPath = arg;
        }
    }

    validateArgs(args);
    return args;
}

void CommandLineParser::validateArgs(CommandLineArgs& args) {
    if (args.mode == CommandLineArgs::UNKNOWN && !args.inputPath.empty()) {
        // Try to determine mode from input path
        if (args.inputPath.find(".encrypted") != std::string::npos) {
            args.mode = CommandLineArgs::DECRYPT_FILE;
        } else {
            args.mode = CommandLineArgs::ENCRYPT_FILE;
        }
    }

    // Set default file types if none specified
    if (args.fileTypes.empty()) {
        args.fileTypes = {"jpg", "jpeg", "png"};
    }

    // Set default crypto parameters if not loaded from config
    if (args.crypto.matrix.empty()) {
        args.crypto.matrix = {{2, 4, 1}, {6, 8, 9}};
        args.crypto.key = {3, 5, 7};
        args.crypto.modulus = 256;
    }

    // Convert file/folder mode based on input path
    if (args.mode == CommandLineArgs::ENCRYPT_FILE) {
        std::ifstream file(args.inputPath);
        if (!file.good()) {
            if (std::filesystem::is_directory(args.inputPath)) {
                args.mode = CommandLineArgs::ENCRYPT_FOLDER;
            } else {
                throw std::runtime_error("Input file or folder does not exist: " + args.inputPath);
            }
        }
    }
}

void CommandLineParser::parseEncryptionConfig(const std::string& configFile, CommandLineArgs& args) {
    auto config = parseConfigFile(configFile);
    
    try {
        // Parse matrix
        if (config.count("matrix")) {
            std::stringstream ss(config["matrix"]);
            std::vector<int> numbers;
            int num;
            while (ss >> num) {
                numbers.push_back(num);
            }
            
            // Assume 2x3 matrix
            if (numbers.size() == 6) {
                args.crypto.matrix = {
                    {numbers[0], numbers[1], numbers[2]},
                    {numbers[3], numbers[4], numbers[5]}
                };
            }
        }
        
        // Parse key
        if (config.count("key")) {
            std::stringstream ss(config["key"]);
            int num;
            while (ss >> num) {
                args.crypto.key.push_back(num);
            }
        }
        
        // Parse modulus
        if (config.count("modulus")) {
            args.crypto.modulus = std::stoi(config["modulus"]);
        }
    } catch (const std::exception& e) {
        throw std::runtime_error("Error parsing config file: " + std::string(e.what()));
    }
}

std::map<std::string, std::string> CommandLineParser::parseConfigFile(const std::string& path) {
    std::map<std::string, std::string> config;
    std::ifstream file(path);
    
    if (!file) {
        throw std::runtime_error("Cannot open config file: " + path);
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') continue;
        
        // Find key-value separator
        auto pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            
            // Trim whitespace
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            config[key] = value;
        }
    }
    
    return config;
}