#ifndef REX_ELF_LOADER_H
#define REX_ELF_LOADER_H

#include <jni.h>
#include <string>

namespace rex {

/**
 * Low-Latency Native ELF Shared Library Loader
 * Loads 32-bit legacy .so libraries into memory, maps ELF segments,
 * and binds JNI export functions directly.
 */
class ElfLoader {
public:
    static ElfLoader& getInstance();

    bool loadNativeLibraryFromApk(JNIEnv* env, const char* apkPath, const char* soName);
    void* findSymbol(const char* symbolName);

private:
    ElfLoader() = default;
    ~ElfLoader() = default;
    ElfLoader(const ElfLoader&) = delete;
    ElfLoader& operator=(const ElfLoader&) = delete;

    void* m_loadedHandle{nullptr};
};

} // namespace rex

#endif // REX_ELF_LOADER_H
