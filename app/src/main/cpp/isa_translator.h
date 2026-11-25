#ifndef REX_ISA_TRANSLATOR_H
#define REX_ISA_TRANSLATOR_H

#include <cstdint>
#include <cstddef>

namespace rex {

/**
 * Real-Time Low-Latency 32-bit ARM (ARMv7-A / Thumb-2) to 64-bit ARM64 (AArch64)
 * Instruction & Syscall Translation Engine.
 *
 * Designed to execute 32-bit legacy game native code on 64-bit only SOCs
 * without requiring APK reverse engineering or decompilation.
 */
class IsaTranslator {
public:
    static IsaTranslator& getInstance();

    bool initializeTranslator();
    void shutdownTranslator();

    // Real-time instruction stream JIT translator
    void* translateInstructionBlock(const uint8_t* code32Ptr, size_t codeSize, bool isThumb);

    // 32-bit to 64-bit Syscall Bridge
    long executeSyscallBridge32(long syscallNum, long a1, long a2, long a3, long a4, long a5, long a6);

private:
    IsaTranslator() = default;
    ~IsaTranslator() = default;
    IsaTranslator(const IsaTranslator&) = delete;
    IsaTranslator& operator=(const IsaTranslator&) = delete;

    bool m_initialized{false};
};

} // namespace rex

#endif // REX_ISA_TRANSLATOR_H
