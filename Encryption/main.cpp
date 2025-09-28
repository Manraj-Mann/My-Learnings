#include "mhla.h"
#include "folder_handler.h"
#include "file_handler.h"
#include "command_line_parser.h"
#include <cmath>

void printVector(const std::string& name, const Vector& vec) {
    std::cout << name << ": [ ";
    for (const auto& val : vec) {
        std::cout << val << " ";
    }
    std::cout << "]" << std::endl;
}

int main(int argc, char* argv[]) {
    try {
        auto args = CommandLineParser::parse(argc, argv);

        if (args.mode == CommandLineArgs::HELP) {
            CommandLineParser::printHelp();
            return 0;
        }

        if (args.mode == CommandLineArgs::VERSION) {
            CommandLineParser::printVersion();
            return 0;
        }

        // Initialize crypto system with parameters from args
        // Convert 1D vector to 2D matrix
        Matrix matrix;
        int matrixSize = static_cast<int>(sqrt(args.crypto.matrix.size()));
        matrix.resize(matrixSize);
        for (int i = 0; i < matrixSize; i++) {
            matrix[i].resize(matrixSize);
            for (int j = 0; j < matrixSize; j++) {
                matrix[i][j] = args.crypto.matrix[i * matrixSize + j];
            }
        }
        
        MHLA crypto_system(matrix, args.crypto.key, args.crypto.modulus);

        // Process according to mode
        bool success = true;
        std::string in_path = args.inputPath;
        std::string out_path = args.outputPath;

        switch (args.mode) {
            case CommandLineArgs::ENCRYPT_FILE: {
                if (out_path.empty()) {
                    out_path = in_path + ".encrypted";
                }
                if (args.verbose) {
                    std::cout << "Encrypting file: " << in_path << " -> " << out_path << std::endl;
                }
                success = FileHandler::encryptFile(in_path, out_path, crypto_system);
                if (success && args.verbose) {
                    std::cout << "File encrypted successfully." << std::endl;
                }
                break;
            }

            case CommandLineArgs::DECRYPT_FILE: {
                if (out_path.empty()) {
                    out_path = in_path;
                    std::string suffix = ".encrypted";
                    if (out_path.length() > suffix.length() &&
                        out_path.substr(out_path.length() - suffix.length()) == suffix) {
                        out_path = out_path.substr(0, out_path.length() - suffix.length());
                    }
                    out_path += ".decrypted";
                }
                if (args.verbose) {
                    std::cout << "Decrypting file: " << in_path << " -> " << out_path << std::endl;
                }
                success = FileHandler::decryptFile(in_path, out_path, crypto_system);
                if (success && args.verbose) {
                    std::cout << "File decrypted successfully." << std::endl;
                }
                break;
            }

            case CommandLineArgs::ENCRYPT_FOLDER: {
                if (out_path.empty()) {
                    out_path = in_path + "_encrypted";
                }
                if (args.verbose) {
                    std::cout << "Encrypting folder: " << in_path << " -> " << out_path << std::endl;
                }
                auto results = FolderHandler::encryptFolder(in_path, out_path, crypto_system);
                success = !results.empty();
                if (args.verbose) {
                    std::cout << "\nEncryption Results:\n";
                    for (const auto& result : results) {
                        std::cout << result.originalPath << " -> ";
                        if (result.success) {
                            std::cout << "SUCCESS: " << result.processedPath << std::endl;
                        } else {
                            std::cout << "FAILED: " << result.error << std::endl;
                            success = false;
                        }
                    }
                }
                break;
            }

            case CommandLineArgs::DECRYPT_FOLDER: {
                if (out_path.empty()) {
                    out_path = in_path + "_decrypted";
                }
                if (args.verbose) {
                    std::cout << "Decrypting folder: " << in_path << " -> " << out_path << std::endl;
                }
                auto results = FolderHandler::decryptFolder(in_path, out_path, crypto_system);
                success = !results.empty();
                if (args.verbose) {
                    std::cout << "\nDecryption Results:\n";
                    for (const auto& result : results) {
                        std::cout << result.originalPath << " -> ";
                        if (result.success) {
                            std::cout << "SUCCESS: " << result.processedPath << std::endl;
                        } else {
                            std::cout << "FAILED: " << result.error << std::endl;
                            success = false;
                        }
                    }
                }
                break;
            }

            default:
                std::cerr << "Invalid mode. Use --help for usage information." << std::endl;
                return 1;
        }

        return success ? 0 : 1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}