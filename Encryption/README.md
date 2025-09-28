# MHLA Encryption Utility

A command-line encryption utility that uses Matrix-based encryption with noise vectors for secure file and folder encryption.

## Features

- File encryption and decryption
- Folder encryption and decryption (recursive)
- Matrix-based encryption algorithm
- Noise vector generation for enhanced security
- Command-line interface for easy integration

## Build Instructions

```bash
g++ main.cpp -o mhl_demo
```

## Usage

### Encrypting a File
```bash
./mhl_demo encrypt-file --matrix "m11,m12,m21,m22" --key "k1,k2" input_file [--output output_file]
```

### Decrypting a File
```bash
./mhl_demo decrypt-file --matrix "m11,m12,m21,m22" --key "k1,k2" encrypted_file [--output decrypted_file]
```

### Encrypting a Folder
```bash
./mhl_demo encrypt-folder --matrix "m11,m12,m21,m22" --key "k1,k2" input_folder --output output_folder
```

### Decrypting a Folder
```bash
./mhl_demo decrypt-folder --matrix "m11,m12,m21,m22" --key "k1,k2" encrypted_folder --output decrypted_folder
```

## Parameters

- `matrix`: A 2x2 matrix represented as four comma-separated values (m11,m12,m21,m22)
- `key`: A vector of two values (k1,k2)
- `input_file/folder`: Path to the input file or folder to encrypt/decrypt
- `output`: Optional output path for the encrypted/decrypted result

## Example

```bash
# Encrypt a file
./mhl_demo encrypt-file --matrix "1,2,3,4" --key "5,6" image.jpg

# Decrypt a file
./mhl_demo decrypt-file --matrix "1,2,3,4" --key "5,6" image.jpg.encrypted

# Encrypt a folder
./mhl_demo encrypt-folder --matrix "1,2,3,4" --key "5,6" input_folder --output encrypted_folder

# Decrypt a folder
./mhl_demo decrypt-folder --matrix "1,2,3,4" --key "5,6" encrypted_folder --output decrypted_folder
```

## Algorithm

The encryption uses a matrix-based approach with the following steps:

1. Data is processed in blocks of 2 bytes
2. Random noise vector (N) is generated
3. Input vector (X) is transformed using the matrix (M) and key (K)
4. Intermediate vector (T) = MX + K
5. Result vector (R) = T + N
6. Noise vector is preserved for decryption

## Security Notes

- Keep your matrix and key secure
- Use different matrix/key combinations for different files
- The security strength depends on the matrix values chosen