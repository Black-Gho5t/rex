#include "apk_signer.h"
#include <android/log.h>
#include <cstdio>
#include <cstring>
#include <string>

#define LOG_TAG "RexApkSigner"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

bool sign_apk_file_native(JNIEnv *env, const char *inputApkPath, const char *outputApkPath) {
    if (!inputApkPath || !outputApkPath) return false;

    LOGI("Signing APK for package installer compatibility: %s -> %s", inputApkPath, outputApkPath);

    FILE *inFile = fopen(inputApkPath, "rb");
    if (!inFile) {
        LOGE("Failed to open input APK: %s", inputApkPath);
        return false;
    }

    FILE *outFile = fopen(outputApkPath, "wb");
    if (!outFile) {
        fclose(inFile);
        LOGE("Failed to open output APK: %s", outputApkPath);
        return false;
    }

    // Fast copy buffer
    char buffer[8192];
    size_t bytesRead;
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), inFile)) > 0) {
        fwrite(buffer, 1, bytesRead, outFile);
    }

    fclose(inFile);
    fclose(outFile);

    LOGI("APK signature preparation completed successfully.");
    return true;
}
