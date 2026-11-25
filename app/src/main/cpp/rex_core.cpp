#include "rex_core.h"
#include <android/log.h>

#define LOG_TAG "RexEngineCore"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

namespace rex {

RexEngine& RexEngine::getInstance() {
    static RexEngine instance;
    return instance;
}

bool RexEngine::initialize(JNIEnv* env, jobject context, const char* sandboxDir) {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_initialized.load()) {
        LOGI("RexEngine already initialized.");
        return true;
    }

    if (!sandboxDir) {
        LOGE("Sandbox directory path is null.");
        return false;
    }

    m_sandboxDir = std::string(sandboxDir);
    m_initialized.store(true);

    LOGI("RexEngine initialized successfully for mid-range device optimization.");
    LOGI("Sandbox directory registered at: %s", m_sandboxDir.c_str());

    return true;
}

void RexEngine::shutdown() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_sandboxDir.clear();
    m_allocatedMemory.store(0);
    m_initialized.store(false);
    LOGI("RexEngine shutdown complete.");
}

} // namespace rex
