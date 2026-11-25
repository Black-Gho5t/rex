#include "hidden_api_hook.h"
#include <android/log.h>

#define LOG_TAG "RexNativeCompat"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

bool unseal_hidden_apis(JNIEnv *env) {
    if (!env) return false;

    // Retrieve dalvik.system.VMRuntime class
    jclass vmRuntimeClass = env->FindClass("dalvik/system/VMRuntime");
    if (!vmRuntimeClass) {
        LOGE("Failed to find VMRuntime class.");
        env->ExceptionClear();
        return false;
    }

    // Get getRuntime() static method
    jmethodID getRuntimeMethod = env->GetStaticMethodID(
            vmRuntimeClass,
            "getRuntime",
            "()Ldalvik/system/VMRuntime;"
    );
    if (!getRuntimeMethod) {
        LOGE("Failed to get VMRuntime.getRuntime method.");
        env->ExceptionClear();
        return false;
    }

    jobject vmRuntimeInstance = env->CallStaticObjectMethod(vmRuntimeClass, getRuntimeMethod);
    if (!vmRuntimeInstance) {
        LOGE("Failed to invoke VMRuntime.getRuntime().");
        env->ExceptionClear();
        return false;
    }

    // Get setHiddenApiExemptions method
    jmethodID setHiddenApiExemptions = env->GetMethodID(
            vmRuntimeClass,
            "setHiddenApiExemptions",
            "([Ljava/lang/String;)V"
    );
    if (!setHiddenApiExemptions) {
        LOGE("Failed to get setHiddenApiExemptions method.");
        env->ExceptionClear();
        return false;
    }

    // Create signature pattern "L" to match all class methods and fields
    jclass stringClass = env->FindClass("java/lang/String");
    jobjectArray exemptions = env->NewObjectArray(1, stringClass, env->NewStringUTF("L"));

    env->CallVoidMethod(vmRuntimeInstance, setHiddenApiExemptions, exemptions);

    if (env->ExceptionCheck()) {
        LOGE("Exception occurred while exempting hidden APIs.");
        env->ExceptionClear();
        return false;
    }

    LOGI("Successfully unsealed ART Hidden APIs for legacy application compatibility.");
    return true;
}
