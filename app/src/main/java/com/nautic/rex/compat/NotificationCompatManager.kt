package com.nautic.rex.compat

import android.app.NotificationChannel
import android.app.NotificationManager
import android.content.Context
import android.os.Build

object NotificationCompatManager {

    const val DEFAULT_CHANNEL_ID = "legacy_app_default_channel"
    private const val DEFAULT_CHANNEL_NAME = "Legacy Application Notifications"

    fun ensureDefaultChannelCreated(context: Context) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            val notificationManager =
                context.getSystemService(Context.NOTIFICATION_SERVICE) as? NotificationManager
            if (notificationManager != null && notificationManager.getNotificationChannel(DEFAULT_CHANNEL_ID) == null) {
                val channel = NotificationChannel(
                    DEFAULT_CHANNEL_ID,
                    DEFAULT_CHANNEL_NAME,
                    NotificationManager.IMPORTANCE_DEFAULT
                ).apply {
                    description = "Fallback notification channel for legacy Android apps"
                }
                notificationManager.createNotificationChannel(channel)
            }
        }
    }
}
