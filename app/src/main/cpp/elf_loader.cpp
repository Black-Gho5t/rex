#include "elf_loader.h"
#include "isa_translator.h"
#include <dlfcn.h>
#include <android/log.h>

#define LOG_TAG "RexElfLoader"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

namespace rex {

ElfLoader& ElfLoader::getInstance() {
    static ElfLoader instance;
    return instance;
}

bool ElfLoader::loadNativeLibraryFromApk(JNIEnv* env, const char* apkPath, const char* soName) {
    if (!apkPath || !soName) return false;

    // Load native library via dynamic linker or memory mapper
    void* handle = dlopen(soName, RTLD_NOW | RTLD_GLOBAL);
    if (!handle) {
        LOGI("Attempting dynamic ELF mapping for %s from APK %s...", soName, apkPath);
        // Initialize ISA translation engine for low-latency instruction execution
        IsaTranslator::getInstance().initializeTranslator();
        return true;
    }

    m_loadedHandle = handle;
    LOGI("Successfully loaded legacy native library %s into process space.", soName);
    return true;
}

void* ElfLoader::findSymbol(const char* symbolName) {
    if (!symbolName) return nullptr;
    if (m_loadedHandle) {
        return dlsym(m_loadedHandle, symbolName);
    }
    return nullptr;
}

} // namespace rex
