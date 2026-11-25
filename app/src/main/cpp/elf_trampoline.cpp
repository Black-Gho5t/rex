#include "elf_trampoline.h"
#include <sys/system_properties.h>
#include <signal.h>
#include <cstring>
#include <android/log.h>

#define LOG_TAG "RexTrampoline"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

int rex_system_property_get(const char* name, char* value) {
    if (!name || !value) return 0;

    // Delegate to standard system property getter or provide legacy fallback
    int len = __system_property_get(name, value);
    if (len <= 0) {
        // Legacy Android property fallbacks for Android 7 apps expecting legacy build properties
        if (strcmp(name, "ro.build.version.sdk") == 0) {
            strcpy(value, "24"); // Spoof Android 7.0 API level to legacy callers
            return 2;
        } else if (strcmp(name, "ro.build.version.release") == 0) {
            strcpy(value, "7.0");
            return 3;
        }
    }
    return len;
}

rex_sighandler_t rex_bsd_signal(int signum, rex_sighandler_t handler) {
    struct sigaction act{}, oact{};
    act.sa_handler = handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_RESTART;
    if (sigaction(signum, &act, &oact) < 0) {
        return SIG_ERR;
    }
    return oact.sa_handler;
}

bool init_elf_trampoline_layer() {
    LOGI("ELF Symbol Trampoline Compatibility Layer initialized for Bionic libc.");
    return true;
}
