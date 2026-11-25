#ifndef REX_HIDDEN_API_HOOK_H
#define REX_HIDDEN_API_HOOK_H

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Unseals Android ART Hidden APIs on Android 9+ (API 28 to 36+)
 * allowing legacy apps to invoke internal Android framework APIs without throwing SecurityException/NoSuchMethodError.
 */
bool unseal_hidden_apis(JNIEnv *env);

#ifdef __cplusplus
}
#endif

#endif // REX_HIDDEN_API_HOOK_H
