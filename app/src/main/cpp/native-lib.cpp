#include <jni.h>
#include <string>
#include "rex_core.h"
#include "elf_trampoline.h"
#include "art_runtime_hook.h"
#include "hidden_api_hook.h"
#include "vfs_hook.h"
#include "isa_translator.h"
#include "elf_loader.h"

extern "C" JNIEXPORT jstring JNICALL
Java_com_nautic_rex_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string engineInfo = "Rex Native Engine (v1.0 - Real-Time ISA Translator Active)";
    return env->NewStringUTF(engineInfo.c_str());
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_nautic_rex_engine_NativeBridge_initNativeCore(
        JNIEnv* env,
        jobject /* this */,
        jobject context,
        jstring sandboxPath) {
    const char *path = env->GetStringUTFChars(sandboxPath, nullptr);
    bool initialized = rex::RexEngine::getInstance().initialize(env, context, path);
    if (path) {
        env->ReleaseStringUTFChars(sandboxPath, path);
    }

    init_elf_trampoline_layer();
    bypass_art_hidden_api_restrictions(env);
    rex::IsaTranslator::getInstance().initializeTranslator();

    return initialized ? JNI_TRUE : JNI_FALSE;
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_nautic_rex_engine_Arm32CompatEngine_loadNativeLibraryNative(
        JNIEnv* env,
        jobject /* this */,
        jstring apkPath,
        jstring soName) {
    const char* cApkPath = env->GetStringUTFChars(apkPath, nullptr);
    const char* cSoName = env->GetStringUTFChars(soName, nullptr);

    bool loaded = rex::ElfLoader::getInstance().loadNativeLibraryFromApk(env, cApkPath, cSoName);

    if (cApkPath) env->ReleaseStringUTFChars(apkPath, cApkPath);
    if (cSoName) env->ReleaseStringUTFChars(soName, cSoName);

    return loaded ? JNI_TRUE : JNI_FALSE;
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_nautic_rex_compat_ApkCompatibilityEngine_unsealHiddenApisNative(
        JNIEnv* env,
        jobject /* this */) {
    return unseal_hidden_apis(env) ? JNI_TRUE : JNI_FALSE;
}

extern "C" JNIEXPORT void JNICALL
Java_com_nautic_rex_compat_ApkCompatibilityEngine_initVfsSandboxNative(
        JNIEnv* env,
        jobject /* this */,
        jstring sandboxPath) {
    init_vfs_sandbox(env, sandboxPath);
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_nautic_rex_compat_ApkCompatibilityEngine_translatePathNative(
        JNIEnv* env,
        jobject /* this */,
        jstring originalPath) {
    return translate_legacy_path(env, originalPath);
}

extern "C" JNIEXPORT jlong JNICALL
Java_com_nautic_rex_engine_NativeBridge_getNativeMemoryUsage(
        JNIEnv* /* env */,
        jobject /* this */) {
    return static_cast<jlong>(rex::RexEngine::getInstance().getMemoryUsage());
}
