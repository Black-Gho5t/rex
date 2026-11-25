#ifndef REX_ART_RUNTIME_HOOK_H
#define REX_ART_RUNTIME_HOOK_H

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Low-level ART AccessFlag Modifier:
 * Modifies AccessFlags on Java Method reflection object to bypass private/hidden access restrictions.
 */
bool make_method_public(JNIEnv *env, jobject reflectMethod);

/**
 * Disables ART runtime hidden API checks directly in process memory.
 */
bool bypass_art_hidden_api_restrictions(JNIEnv *env);

#ifdef __cplusplus
}
#endif

#endif // REX_ART_RUNTIME_HOOK_H
