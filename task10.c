#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

#define ITERATIONS 25
#define PASSWORD_LENGTH 64
#define SALT_LENGTH 16
#define ENCRYPTED_LENGTH 16
#define NUM_PASSWORDS 10


void encrypt_password(const char *password, const unsigned char *salt, unsigned char *output) {
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, NULL, NULL);

    // Combine pass and salt
    unsigned char key[16];
    memset(key, 0, sizeof(key));
    strncpy((char *)key, password, 8);
    memcpy(key + 8, salt, 8);
    EVP_EncryptInit_ex(ctx, NULL, NULL, key, NULL);

    // Salt as input
    memcpy(output, salt, 16);

    // Apply encryption 25 times
    int out_len;
    for (int i = 0; i < ITERATIONS; i++) {
        EVP_EncryptUpdate(ctx, output, &out_len, output, 16);
    }

    EVP_CIPHER_CTX_free(ctx);
}

int validate_pass(const char *input_password, const unsigned char *stored_salt, const unsigned char *stored_encrypted_password) {
    unsigned char encrypted_input[ENCRYPTED_LENGTH];

    encrypt_password(input_password, stored_salt, encrypted_input);

    printf("Stored salt: ");
    for (int i = 0; i < SALT_LENGTH; i++) {
        printf("%02x", stored_salt[i]);
    }
    printf("\n Encrypted input password: ");
    for (int i = 0; i < ENCRYPTED_LENGTH; i++) {
        printf("%02x", encrypted_input[i]);
    }
    printf("\n");

    // Compare encrypted input and encrypted stored 
    return (memcmp(encrypted_input, stored_encrypted_password, ENCRYPTED_LENGTH) == 0);
}

int main() {
    char passwords[NUM_PASSWORDS][PASSWORD_LENGTH];
    unsigned char salts[NUM_PASSWORDS][SALT_LENGTH];
    unsigned char encrypted_passwords[NUM_PASSWORDS][ENCRYPTED_LENGTH];

    // Generate 10 passwords
    for (int i = 0; i < NUM_PASSWORDS; i++) {
        printf("Enter passwrd %d: ", i + 1);
        scanf("%60s", passwords[i]);

        // Get random salt
        RAND_bytes(salts[i], SALT_LENGTH);

        encrypt_password(passwords[i], salts[i], encrypted_passwords[i]);

        printf("Pass %d:\n", i + 1);
        printf("Encrypted pass: ");
        for (int j = 0; j < ENCRYPTED_LENGTH; j++) {
            printf("%02x", encrypted_passwords[i][j]);
        }
        printf("\nSalt: ");
        for (int j = 0; j < SALT_LENGTH; j++) {
            printf("%02x", salts[i][j]);
        }
        printf("\n");
    }

    // Validation
    char test_password[PASSWORD_LENGTH];
    for (int i = 0; i < NUM_PASSWORDS; i++) {
        printf("Enter password to validate %d: ", i + 1);
        scanf("%63s", test_password);

        if (validate_pass(test_password, salts[i], encrypted_passwords[i])) {
            printf("Password %d valid\n", i + 1);
        } else {
            printf("Password %d invalid.\n", i + 1);
        }
    }

    return 0;
}
