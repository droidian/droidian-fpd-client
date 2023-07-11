#include <security/pam_appl.h>
#include <security/pam_misc.h>
#include <stdio.h>

const struct pam_conv conv = {
    misc_conv,
    NULL
};

int authenticate() {
    pam_handle_t* pamh = NULL;
    int result = pam_start("pam_example", NULL, &conv, &pamh);
    if (result != PAM_SUCCESS) {
        return result;
    }

    result = pam_authenticate(pamh, 0);
    if (result != PAM_SUCCESS) {
        pam_end(pamh, result);
        return result;
    }

    result = pam_end(pamh, result);
    return result;
}

int main() {
    int result = authenticate();
    if (result == PAM_SUCCESS) {
        printf("Authentication successful!\n");
    } else {
        printf("Authentication failed: %s\n", pam_strerror(NULL, result));
    }

    return result == PAM_SUCCESS ? 0 : 1;
}
