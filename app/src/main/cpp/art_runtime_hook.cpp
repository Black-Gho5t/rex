#include "art_runtime_hook.h"
#include <android/log.h>
#include <cstdint>

#define LOG_TAG "RexArtHook"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// Access flags definitions matching ART internal ArtMethod
static const uint32_t kAccPublic = 0x0001;
static const uint32_t kAccPrivate = 0x0002;
static const uint32_t kAccProtected = 0x0004;

bool make_method_public(JNIEnv *env, jobject reflectMethod) {
    if (!env || !reflectMethod) return false;

    // Get Method.artMethod pointer via JNI GetMethodID / FromReflectedMethod
    jmethodID methodId = env->FromReflectedMethod(reflectMethod);
    if (!methodId) {
        LOGE("Failed to extract jmethodID from reflected method.");
        return false;
    }

    // Direct pointer cast to method structure
    uintptr_t artMethodPtr = reinterpret_cast<uintptr_t>(methodId);
    if (artMethodPtr == 0) return false;

    // Set method accessible flag via standard Java Reflection setAccessible
    jclass methodClass = env->FindClass("java/lang/reflect/AccessibleObject");
    if (methodClass) {
        jmethodID setAccessible = env->GetMethodID(methodClass, "setAccessible", "(Z)V");
        if (setAccessible) {
            env->CallVoidMethod(reflectMethod, setAccessible, JNI_TRUE);
            if (!env->ExceptionCheck()) {
                LOGI("Successfully marked method as accessible via ART reflection bridge.");
                return true;
            } else {
                env->ExceptionClear();
            }
        }
    }

    return false;
}

bool bypass_art_hidden_api_restrictions(JNIEnv *env) {
    if (!env) return false;

    jclass vmRuntimeClass = env->FindClass("dalvik/system/VMRuntime");
    if (!vmRuntimeClass) {
        env->ExceptionClear();
        return false;
    }

    jmethodID getRuntimeMethod = env->GetStaticMethodID(
            vmRuntimeClass,
            "getRuntime",
            "()Ldalvik/system/VMRuntime;"
    );
    if (!getRuntimeMethod) {
        env->ExceptionClear();
        return false;
    }

    jobject vmRuntimeInstance = env->CallStaticObjectMethod(vmRuntimeClass, getRuntimeMethod);
    if (!vmRuntimeInstance) {
        env->ExceptionClear();
        return false;
    }

    jmethodID setHiddenApiExemptions = env->GetMethodID(
            vmRuntimeClass,
            "setHiddenApiExemptions",
            "([Ljava/lang/String;)V"
    );
    if (!setHiddenApiExemptions) {
        env->ExceptionClear();
        return false;
    }

    jclass stringClass = env->FindClass("java/lang/String");
    jobjectArray exemptions = env->NewObjectArray(1, stringClass, env->NewStringUTF("L"));

    env->CallVoidMethod(vmRuntimeInstance, setHiddenApiExemptions, exemptions);

    if (env->ExceptionCheck()) {
        env->ExceptionClear();
        return false;
    }

    LOGI("ART Hidden API Restrictions bypassed directly in runtime memory.");
    return true;
}
