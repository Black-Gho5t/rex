#ifndef REX_APK_SIGNER_H
#define REX_APK_SIGNER_H

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Validates and injects test signature metadata (v1/v2) into an APK file
 * so Android Package Installer doesn't reject it with "INSTALL_PARSE_FAILED_NO_CERTIFICATES".
 */
bool sign_apk_file_native(JNIEnv *env, const char *inputApkPath, const char *outputApkPath);

#ifdef __cplusplus
}
#endif

#endif // REX_APK_SIGNER_H
