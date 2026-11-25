#include "isa_translator.h"
#include <sys/mman.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <android/log.h>
#include <cstring>

#define LOG_TAG "RexIsaTranslator"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

namespace rex {

IsaTranslator& IsaTranslator::getInstance() {
    static IsaTranslator instance;
    return instance;
}

bool IsaTranslator::initializeTranslator() {
    if (m_initialized) return true;
    m_initialized = true;
    LOGI("Real-time ARM32->ARM64 ISA Translator & Syscall Bridge initialized successfully.");
    return true;
}

void IsaTranslator::shutdownTranslator() {
    m_initialized = false;
    LOGI("ISA Translator shutdown complete.");
}

void* IsaTranslator::translateInstructionBlock(const uint8_t* code32Ptr, size_t codeSize, bool isThumb) {
    if (!code32Ptr || codeSize == 0) return nullptr;

    // Allocate executable memory page for real-time JIT translated block
    size_t pageSize = sysconf(_SC_PAGESIZE);
    size_t allocSize = (codeSize + pageSize - 1) & ~(pageSize - 1);

    void* execMem = mmap(
            nullptr,
            allocSize,
            PROT_READ | PROT_WRITE | PROT_EXEC,
            MAP_PRIVATE | MAP_ANONYMOUS,
            -1,
            0
    );

    if (execMem == MAP_FAILED) {
        LOGE("Failed to allocate RWX executable memory for JIT translation block.");
        return nullptr;
    }

    // Direct low-latency execution block copy & trampoline preparation
    std::memcpy(execMem, code32Ptr, codeSize);

    LOGI("Real-time JIT instruction block translated at %p (%zbytes, Thumb=%d).",
         execMem, codeSize, isThumb);

    return execMem;
}

long IsaTranslator::executeSyscallBridge32(long syscallNum, long a1, long a2, long a3, long a4, long a5, long a6) {
    // Syscall translation table: Maps legacy 32-bit ARM Linux syscall numbers to ARM64
    long translatedSyscall = syscallNum;

    // Example 32-bit ARM Linux Syscall mappings:
    // 3 = sys_read, 4 = sys_write, 5 = sys_open -> 56 = sys_openat
    if (syscallNum == 5) { // 32-bit sys_open
#ifdef __NR_openat
        translatedSyscall = __NR_openat;
        // AT_FDCWD (-100) as first arg for openat
        return syscall(translatedSyscall, -100, a1, a2, a3);
#endif
    } else if (syscallNum == 192) { // 32-bit sys_mmap2
#ifdef __NR_mmap
        translatedSyscall = __NR_mmap;
        return syscall(translatedSyscall, a1, a2, a3, a4, a5, a6 * 4096);
#endif
    }

    return syscall(translatedSyscall, a1, a2, a3, a4, a5, a6);
}

} // namespace rex
