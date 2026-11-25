package com.nautic.rex.compat

import android.content.Context
import android.content.pm.PackageInfo
import android.content.pm.PackageManager
import android.util.Log

class ApkManifestFixer(private val context: Context) {

    companion object {
        private const val TAG = "ApkManifestFixer"
    }

    data class CompatibilityReport(
        val packageName: String,
        val targetSdkVersion: Int,
        val requiresExportedFix: Boolean,
        val requiresStorageRedirection: Boolean,
        val requiresHiddenApiUnseal: Boolean,
        val declaredPermissions: List<String>
    )

    fun inspectApk(apkPath: String): CompatibilityReport? {
        return try {
            val packageInfo: PackageInfo? = if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.TIRAMISU) {
                context.packageManager.getPackageArchiveInfo(
                    apkPath,
                    PackageManager.PackageInfoFlags.of(
                        (PackageManager.GET_ACTIVITIES or
                                PackageManager.GET_SERVICES or
                                PackageManager.GET_RECEIVERS or
                                PackageManager.GET_PERMISSIONS).toLong()
                    )
                )
            } else {
                @Suppress("DEPRECATION")
                context.packageManager.getPackageArchiveInfo(
                    apkPath,
                    PackageManager.GET_ACTIVITIES or
                            PackageManager.GET_SERVICES or
                            PackageManager.GET_RECEIVERS or
                            PackageManager.GET_PERMISSIONS
                )
            }

            if (packageInfo == null) return null

            val targetSdk = packageInfo.applicationInfo?.targetSdkVersion ?: 24
            val packageName = packageInfo.packageName
            val permissions = packageInfo.requestedPermissions?.toList() ?: emptyList()

            val requiresExportedFix = targetSdk < 31
            val requiresStorageRedirection = targetSdk < 29
            val requiresHiddenApiUnseal = targetSdk < 28

            Log.i(TAG, "Inspected APK $packageName - Target SDK: $targetSdk")

            CompatibilityReport(
                packageName = packageName,
                targetSdkVersion = targetSdk,
                requiresExportedFix = requiresExportedFix,
                requiresStorageRedirection = requiresStorageRedirection,
                requiresHiddenApiUnseal = requiresHiddenApiUnseal,
                declaredPermissions = permissions
            )
        } catch (e: Exception) {
            Log.e(TAG, "Error inspecting APK manifest: ${e.message}", e)
            null
        }
    }
}
