#define _XOPEN_SOURCE

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <security/pam_modules.h>
#include <crypt.h>


PAM_EXTERN int pam_sm_authenticate(pam_handle_t* pamh, int flags, int argc, const char** argv) {   
    const char *expected_password_hash = "45VWJbfUGfRqM";  // Заданный хеш пароля admin
    const char *expected_username= "filter"; 
    const char *salt = "45vvSThS3hVB351fsder";  // Соль

    const char *username = NULL;   
    const char *password = NULL;

	/*
    const char *expected_password_hash_fromFile=NULL;
    const char *username_from_file =NULL; 
    const char *salt_fromFile=NULL;
    	*/

    if (pam_get_user(pamh, &username, "Username: ") != PAM_SUCCESS) {
        return PAM_AUTH_ERR;
    }

    if (pam_prompt(pamh, PAM_PROMPT_ECHO_OFF, &password, "Password: ") != PAM_SUCCESS) {
        return PAM_AUTH_ERR;
    }

    char *hashed_password = crypt(password, salt);  // Хешируем введенный пароль с солью

    if (hashed_password == NULL) {
        return PAM_AUTH_ERR;
    }
    
    if (strcmp(username, expected_username) == 0 && strcmp(hashed_password, expected_password_hash) == 0) {
        return PAM_SUCCESS;  // Возвращаем успешный результат аутентификации
    } else {
        return PAM_AUTH_ERR;  // Возвращаем ошибку аутентификации
    }
}

PAM_EXTERN int pam_sm_setcred(pam_handle_t *pamh, int flags, int argc, const char **argv) {
    int retval;
    retval = PAM_SUCCESS;
    return retval;
}

#ifdef PAM_STATIC
struct pam_module _pam_unix_auth_modstruct = {
    "pam_unix_auth",
    pam_sm_authenticate,
    pam_sm_setcred,
    NULL,
    NULL,
    NULL,
    NULL,
};
#endif

