#ifndef REX_VFS_HOOK_H
#define REX_VFS_HOOK_H

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes virtual filesystem hooks and sets up environment variables
 * to map legacy SD card paths (/sdcard, /storage/emulated/0) to local app sandbox.
 */
void init_vfs_sandbox(JNIEnv *env, jstring baseSandboxPath);

/**
 * Translates a legacy path string into its sandboxed equivalent.
 */
jstring translate_legacy_path(JNIEnv *env, jstring originalPath);

#ifdef __cplusplus
}
#endif

#endif // REX_VFS_HOOK_H
