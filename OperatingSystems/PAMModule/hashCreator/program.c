// gcc -o program program.c -lcrypt


#define _XOPEN_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <crypt.h>


/*
// Функция для хеширования имени пользователя с использованием SHA-256
void hash_username(const char* username, char* hashed_username) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256_ctx;
    SHA256_Init(&sha256_ctx);
    SHA256_Update(&sha256_ctx, username, strlen(username));
    SHA256_Final(hash, &sha256_ctx);

    // Преобразование хеша в строку
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        sprintf(&hashed_username[i * 2], "%02x", hash[i]);
    }
}
*/
int main() {
    const char *username = "filter";  
    const char *password = "admin";  
    const char *salt = "45vvSThS3hVB351fsder";   

    char *hashed_password = crypt(password,salt);
    printf("Hashed Password: %s\n", hashed_password);
    
    char *hashed_username = crypt(username, salt);
    printf("Hashed Username: %s\n", hashed_username);

    return 0;
}
