#ifndef COMMAND_LINE_PARSER_H
#define COMMAND_LINE_PARSER_H

#include <string>
#include <vector>

// Structure to hold crypto parameters
struct CryptoParams {
    std::vector<int> matrix;
    std::vector<int> key;
    int modulus;

    CryptoParams() : modulus(256) {} // Default modulus for byte operations
};

// Structure to hold command line arguments
struct CommandLineArgs {
    enum Mode {
        HELP,
        VERSION,
        ENCRYPT_FILE,
        DECRYPT_FILE,
        ENCRYPT_FOLDER,
        DECRYPT_FOLDER
    };

    Mode mode;
    std::string inputPath;
    std::string outputPath;
    bool verbose;
    CryptoParams crypto;

    CommandLineArgs() : mode(HELP), verbose(false) {}
};

class CommandLineParser {
public:
    static CommandLineArgs parse(int argc, char* argv[]);
    static void printHelp();
    static void printVersion();

private:
    static void parseMatrixArg(const std::string& arg, CryptoParams& params);
    static void parseKeyArg(const std::string& arg, CryptoParams& params);
    static void parseModulusArg(const std::string& arg, CryptoParams& params);
    static std::vector<int> parseIntArray(const std::string& str);
};

#endif // COMMAND_LINE_PARSER_H