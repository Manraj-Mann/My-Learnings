
#ifndef MHLA_H
#define MHLA_H

#include <iostream>
#include <vector>
#include <numeric>
#include <fstream>

// A simple alias for clarity
using Matrix = std::vector<std::vector<int>>;
using Vector = std::vector<int>;

/**
 * @class MHLA
 * @brief An implementation of the Modular Hash Learning Algorithm.
 * This class encapsulates the logic for encryption and decryption based on the
 * research paper "Quantum-Resistant Cryptographic Primitives Using Modular
 * Hash Learning Algorithms for Enhanced SCADA System Security".
 * * DISCLAIMER: This is an educational implementation and is NOT secure for
 * real-world use. Cryptography should be handled by peer-reviewed,
 * standardized libraries.
 */
class MHLA {
public:
    // // [cite_start]// Constructor to set up the algorithm's parameters [cite: 131, 132, 133, 152]
    MHLA(const Matrix& p, const Vector& s, int m)
        : P(p), S(s), M(m) {}

    // Main encryption function
    Vector encrypt(const Vector& values);

    // Main decryption function
    Vector decrypt(const Vector& r);


public:
    // // [cite_start]// The public key matrix (m x n) [cite: 133]
    Matrix P;
    // // [cite_start]// The secret key vector (n dimensions) [cite: 132]
    Vector S;
    // // [cite_start]// The modulus value [cite: 152]
    int M;

    // // [cite_start]// Custom hash function to generate noise [cite: 156, 173, 198]
    int hashFunction(int t_i, int value);
};

#endif // MHLA_H