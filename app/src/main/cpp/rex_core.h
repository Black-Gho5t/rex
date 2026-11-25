#ifndef REX_CORE_H
#define REX_CORE_H

#include <jni.h>
#include <string>
#include <mutex>
#include <atomic>

namespace rex {

/**
 * Lightweight Engine State & Native Memory Manager
 * Optimized for mid-range Android hardware with low memory consumption.
 */
class RexEngine {
public:
    static RexEngine& getInstance();

    bool initialize(JNIEnv* env, jobject context, const char* sandboxDir);
    void shutdown();

    bool isInitialized() const { return m_initialized.load(); }
    const std::string& getSandboxDir() const { return m_sandboxDir; }

    // Memory stats for mid-range device monitoring
    size_t getMemoryUsage() const { return m_allocatedMemory.load(); }
    void trackAllocation(size_t size) { m_allocatedMemory.fetch_add(size); }
    void trackDeallocation(size_t size) { m_allocatedMemory.fetch_sub(size); }

private:
    RexEngine() = default;
    ~RexEngine() = default;
    RexEngine(const RexEngine&) = delete;
    RexEngine& operator=(const RexEngine&) = delete;

    std::atomic<bool> m_initialized{false};
    std::string m_sandboxDir;
    std::atomic<size_t> m_allocatedMemory{0};
    mutable std::mutex m_mutex;
};

} // namespace rex

#endif // REX_CORE_H
