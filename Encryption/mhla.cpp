#include "mhla.h"

int MHLA::hashFunction(int t_i, int value) {
    int result = (t_i % M) ^ value;
    for (int key_part : S) {
        result ^= key_part;
    }
    return result;
}

Vector MHLA::encrypt(const Vector& values) {
    Vector T;
    for (const auto& row : P) {
        int t_val = 0;
        for (size_t i = 0; i < S.size(); ++i) {
            t_val += row[i] * S[i];
        }
        T.push_back(t_val);
    }
    std::cout << "  Intermediate Vector T = P*S: ";
    for(int val : T) std::cout << val << " ";
    std::cout << std::endl;

    Vector N;
    for (size_t i = 0; i < values.size(); ++i) {
        N.push_back(hashFunction(T[i], values[i]));
    }
    std::cout << "  Generated Noise Vector N: ";
    for(int val : N) std::cout << val << " ";
    std::cout << std::endl;

    Vector R(T.size());
    for (size_t i = 0; i < T.size(); ++i) {
        R[i] = T[i] + N[i];
    }
    return R;
}

Vector MHLA::decrypt(const Vector& r) {
    Vector T;
    for (const auto& row : P) {
        int t_val = 0;
        for (size_t i = 0; i < S.size(); ++i) {
            t_val += row[i] * S[i];
        }
        T.push_back(t_val);
    }
    std::cout << "  Re-computed Intermediate Vector T: ";
    for(int val : T) std::cout << val << " ";
    std::cout << std::endl;

    Vector N(r.size());
    for (size_t i = 0; i < r.size(); ++i) {
        N[i] = r[i] - T[i];
    }
    std::cout << "  Recovered Noise Vector N = R - T: ";
    for(int val : N) std::cout << val << " ";
    std::cout << std::endl;

    Vector decrypted_values;
    for (size_t i = 0; i < N.size(); ++i) {
        int temp = N[i];
        for (int key_part : S) {
            temp ^= key_part;
        }
        int original_value = temp ^ (T[i] % M);
        decrypted_values.push_back(original_value);
    }
    return decrypted_values;
}