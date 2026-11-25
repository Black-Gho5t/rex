#include "vfs_hook.h"
#include <string>
#include <cstdlib>
#include <android/log.h>

#define LOG_TAG "RexVFS"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

static std::string g_sandbox_dir = "";

void init_vfs_sandbox(JNIEnv *env, jstring baseSandboxPath) {
    if (!env || !baseSandboxPath) return;

    const char *pathChars = env->GetStringUTFChars(baseSandboxPath, nullptr);
    if (pathChars) {
        g_sandbox_dir = std::string(pathChars);
        env->ReleaseStringUTFChars(baseSandboxPath, pathChars);
    }

    // Set legacy environment variable EXTERNAL_STORAGE to internal sandbox path
    if (!g_sandbox_dir.empty()) {
        setenv("EXTERNAL_STORAGE", g_sandbox_dir.c_str(), 1);
        LOGI("VFS Sandbox initialized. EXTERNAL_STORAGE set to %s", g_sandbox_dir.c_str());
    }
}

jstring translate_legacy_path(JNIEnv *env, jstring originalPath) {
    if (!env || !originalPath) return originalPath;

    const char *pathChars = env->GetStringUTFChars(originalPath, nullptr);
    if (!pathChars) return originalPath;

    std::string path(pathChars);
    env->ReleaseStringUTFChars(originalPath, pathChars);

    // Redirect legacy storage paths
    static const std::string legacyPrefix1 = "/sdcard";
    static const std::string legacyPrefix2 = "/storage/emulated/0";
    static const std::string legacyPrefix3 = "/mnt/sdcard";

    std::string resultPath = path;

    if (!g_sandbox_dir.empty()) {
        if (path.rfind(legacyPrefix2, 0) == 0) {
            resultPath = g_sandbox_dir + path.substr(legacyPrefix2.length());
        } else if (path.rfind(legacyPrefix1, 0) == 0) {
            resultPath = g_sandbox_dir + path.substr(legacyPrefix1.length());
        } else if (path.rfind(legacyPrefix3, 0) == 0) {
            resultPath = g_sandbox_dir + path.substr(legacyPrefix3.length());
        }
    }

    return env->NewStringUTF(resultPath.c_str());
}
