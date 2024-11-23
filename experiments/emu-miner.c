/**
 * Compilation: gcc emu-miner.c -lssl -lcrypto
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <openssl/sha.h> // Using OpenSSL for SHA-256

// Define a function for SHA-256 hashing
void sha256(unsigned char *input, size_t len, unsigned char output[SHA256_DIGEST_LENGTH]) {
    // TODO: SHA256_* methods are deprecated...
    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, input, len);
    SHA256_Final(output, &ctx);
}

// Convert a byte array to a hex string for readability (optional)
void to_hex(unsigned char *hash, char output[]) {
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(output + (i * 2), "%02x", hash[i]);
    }
}

// Perform double SHA-256 hashing
void double_sha256(unsigned char *input, size_t len, uint8_t output[SHA256_DIGEST_LENGTH]) {
    unsigned char temp[SHA256_DIGEST_LENGTH];
    sha256(input, len, temp);
    sha256(temp, SHA256_DIGEST_LENGTH, output);
}

int main() {
    // Example: Simplified Bitcoin block header
    uint8_t block_header[80] = { /* 80 bytes of block header data */ };

    // Check if hash meets the difficulty target
    unsigned char target[SHA256_DIGEST_LENGTH] = { /* Difficulty target */ };
    target[0] = 0xf;

    char target_output[SHA256_DIGEST_LENGTH * 2 + 1];
    to_hex(target, target_output);
    printf("Target:      %s\n\n", target_output);

    unsigned char hash[SHA256_DIGEST_LENGTH];
    char hex_output[SHA256_DIGEST_LENGTH * 2 + 1];
    for (int i = 0; i < 10; i++) {
        block_header[0] = i;
        double_sha256(block_header, sizeof(block_header), hash);

        // Output the hash as a hexadecimal string
        to_hex(hash, hex_output);
        printf("Result Hash: %s\n", hex_output);

        if (memcmp(hash, target, SHA256_DIGEST_LENGTH) < 0) {
            printf("\nBlock header satisfies the difficulty target!\n");
            break;
        } else {
            printf("Keep hashing...\n");
        }
    }

    return 0;
}
