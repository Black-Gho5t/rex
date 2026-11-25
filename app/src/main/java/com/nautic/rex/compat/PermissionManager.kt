package com.nautic.rex.compat

import android.Manifest
import android.content.Context
import android.content.pm.PackageManager
import android.os.Build
import androidx.core.content.ContextCompat

object PermissionManager {

    val LEGACY_PERMISSIONS = arrayOf(
        Manifest.permission.READ_EXTERNAL_STORAGE,
        Manifest.permission.WRITE_EXTERNAL_STORAGE,
        Manifest.permission.CAMERA,
        Manifest.permission.RECORD_AUDIO,
        Manifest.permission.ACCESS_FINE_LOCATION,
        Manifest.permission.ACCESS_COARSE_LOCATION
    )

    fun getMissingPermissions(context: Context): List<String> {
        val missing = mutableListOf<String>()
        for (perm in LEGACY_PERMISSIONS) {
            // Skip storage permissions on Android 13+ if not applicable
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU &&
                (perm == Manifest.permission.READ_EXTERNAL_STORAGE || perm == Manifest.permission.WRITE_EXTERNAL_STORAGE)
            ) {
                continue
            }
            if (ContextCompat.checkSelfPermission(context, perm) != PackageManager.PERMISSION_GRANTED) {
                missing.add(perm)
            }
        }
        return missing
    }
}
