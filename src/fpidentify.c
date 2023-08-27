// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2023 Droidian Project
//
// Authors:
// Bardia Moshiri <fakeshell@bardia.tech>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <batman/wlrdisplay.h>

int wlrdisplay_status() {
    int result = wlrdisplay(0, NULL);
    return result != 0;
}

int delay(double seconds) {
    return usleep(seconds * 1000000);
}

int main() {
    system("/usr/bin/binder-wait android.hardware.biometrics.fingerprint@2.1::IBiometricsFingerprint/default");

    char SESSIONID[64] = {0};

    while (1) {
        FILE *fp = popen("loginctl list-sessions | awk '/tty7/{print $1}'", "r");
        if (fp) {
            fgets(SESSIONID, sizeof(SESSIONID), fp);
            pclose(fp);
        }

        if (strlen(SESSIONID) > 0) {
            break;
        }

        delay(1);
    }

    int OLDSTAT = -1;

    while (1) {
        int DISPSTAT = wlrdisplay_status() == 0 ? 1 : 0;

        if (OLDSTAT != DISPSTAT) {
            OLDSTAT = DISPSTAT;

            if (DISPSTAT == 1) {
                int UNLOCKED = 0;
                while (UNLOCKED == 0) {
                    char cmd[128];
                    snprintf(cmd, sizeof(cmd), "/usr/libexec/fpd/droidian-fpd-unlocker %s", SESSIONID);
                    int result = system(cmd);

                    if (result != 0) {
                        delay(0.2);
                    } else {
                        if (access("/usr/lib/droidian/device/fpd-gkr-unlock", F_OK) != -1) {
                            system("(pgrep -f \"journalctl -q -f --since\" | xargs kill) || true");
                        }

                        UNLOCKED = 1;
                    }
                }
            }
        } else {
            delay(1);
        }
    }

    return 0;
}
