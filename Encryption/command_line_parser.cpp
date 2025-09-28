#include "command_line_parser.h"
#include <iostream>
#include <stdexcept>
#include <sstream>

CommandLineArgs CommandLineParser::parse(int argc, char* argv[]) {
    CommandLineArgs args;
    
    if (argc < 2) {
        args.mode = CommandLineArgs::HELP;
        return args;
    }

    std::string command = argv[1];
    if (command == "--help" || command == "-h") {
        args.mode = CommandLineArgs::HELP;
        return args;
    }
    
    if (command == "--version" || command == "-v") {
        args.mode = CommandLineArgs::VERSION;
        return args;
    }

    // Parse operation mode
    if (command == "encrypt-file") {
        args.mode = CommandLineArgs::ENCRYPT_FILE;
    } else if (command == "decrypt-file") {
        args.mode = CommandLineArgs::DECRYPT_FILE;
    } else if (command == "encrypt-folder") {
        args.mode = CommandLineArgs::ENCRYPT_FOLDER;
    } else if (command == "decrypt-folder") {
        args.mode = CommandLineArgs::DECRYPT_FOLDER;
    } else {
        throw std::runtime_error("Invalid operation mode: " + command);
    }

    // Parse remaining arguments
    for (int i = 2; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "--verbose") {
            args.verbose = true;
        }
        else if (arg == "--matrix" && i + 1 < argc) {
            parseMatrixArg(argv[++i], args.crypto);
        }
        else if (arg == "--key" && i + 1 < argc) {
            parseKeyArg(argv[++i], args.crypto);
        }
        else if (arg == "--modulus" && i + 1 < argc) {
            parseModulusArg(argv[++i], args.crypto);
        }
        else if (arg == "--output" && i + 1 < argc) {
            args.outputPath = argv[++i];
        }
        else if (arg[0] != '-') {
            // If output path is already specified via --output, this must be input path
            if (!args.outputPath.empty()) {
                if (args.inputPath.empty()) {
                    args.inputPath = arg;
                } else {
                    throw std::runtime_error("Multiple input paths specified");
                }
            }
            // If no paths are specified yet, this is input path
            else if (args.inputPath.empty()) {
                args.inputPath = arg;
            }
            // If input path is specified but output isn't, this is output path
            else {
                args.outputPath = arg;
            }
        }
        else {
            throw std::runtime_error("Unknown argument: " + arg);
        }
    }

    return args;
}

void CommandLineParser::printHelp() {
    std::cout << "MHLA Encryption Tool\n"
              << "Usage: mhl_demo <command> [options] <input_path> [output_path]\n\n"
              << "Commands:\n"
              << "  encrypt-file     Encrypt a single file\n"
              << "  decrypt-file     Decrypt a single file\n"
              << "  encrypt-folder   Encrypt all files in a folder\n"
              << "  decrypt-folder   Decrypt all files in a folder\n\n"
              << "Options:\n"
              << "  --verbose       Show detailed operation information\n"
              << "  --matrix M      Encryption matrix (comma-separated integers)\n"
              << "  --key K         Encryption key (comma-separated integers)\n"
              << "  --modulus N     Modulus for encryption (default: 256)\n"
              << "  --output PATH   Specify output path (optional)\n"
              << "  --help, -h      Show this help message\n"
              << "  --version, -v   Show version information\n" << std::endl;
}

void CommandLineParser::printVersion() {
    std::cout << "MHLA Encryption Tool v1.0.0\n"
              << "Copyright (c) 2025 Manraj-Mann\n"
              << "License: MIT" << std::endl;
}

void CommandLineParser::parseMatrixArg(const std::string& arg, CryptoParams& params) {
    params.matrix = parseIntArray(arg);
}

void CommandLineParser::parseKeyArg(const std::string& arg, CryptoParams& params) {
    params.key = parseIntArray(arg);
}

void CommandLineParser::parseModulusArg(const std::string& arg, CryptoParams& params) {
    try {
        params.modulus = std::stoi(arg);
        if (params.modulus <= 0) {
            throw std::runtime_error("Modulus must be positive");
        }
    } catch (const std::invalid_argument&) {
        throw std::runtime_error("Invalid modulus value: " + arg);
    }
}

std::vector<int> CommandLineParser::parseIntArray(const std::string& str) {
    std::vector<int> result;
    std::stringstream ss(str);
    std::string item;
    
    while (std::getline(ss, item, ',')) {
        try {
            result.push_back(std::stoi(item));
        } catch (const std::invalid_argument&) {
            throw std::runtime_error("Invalid integer in array: " + item);
        }
    }
    
    return result;
}