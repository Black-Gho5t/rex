package com.nautic.rex.engine

import android.app.Notification
import android.app.Service
import android.content.pm.ServiceInfo
import android.os.Build

object ServiceWrapper {

    /**
     * Adapts legacy startForeground calls for Android 14+ (API 34+)
     * where foreground services require explicit foregroundServiceType declarations.
     */
    fun startForegroundSafely(
        service: Service,
        id: Int,
        notification: Notification
    ) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
            try {
                service.startForeground(id, notification, ServiceInfo.FOREGROUND_SERVICE_TYPE_DATA_SYNC)
            } catch (_: Exception) {
                try {
                    service.startForeground(id, notification, ServiceInfo.FOREGROUND_SERVICE_TYPE_LOCATION)
                } catch (_: Exception) {
                    service.startForeground(id, notification)
                }
            }
        } else {
            service.startForeground(id, notification)
        }
    }
}
