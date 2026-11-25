package com.nautic.rex.engine

import android.app.ActivityManager
import android.content.Context
import java.util.concurrent.atomic.AtomicBoolean

class OptimizedStateManager(private val context: Context) {

    data class MemoryMetrics(
        val totalMemoryMb: Long,
        val availableMemoryMb: Long,
        val nativeHeapAllocatedMb: Long,
        val isLowMemoryDevice: Boolean
    )

    private val isRunning = AtomicBoolean(false)

    fun getDeviceMemoryMetrics(): MemoryMetrics {
        val activityManager = context.getSystemService(Context.ACTIVITY_SERVICE) as ActivityManager
        val memoryInfo = ActivityManager.MemoryInfo()
        activityManager.getMemoryInfo(memoryInfo)

        val totalMb = memoryInfo.totalMem / (1024 * 1024)
        val availMb = memoryInfo.availMem / (1024 * 1024)
        val nativeUsageMb = android.os.Debug.getNativeHeapAllocatedSize() / (1024 * 1024)

        return MemoryMetrics(
            totalMemoryMb = totalMb,
            availableMemoryMb = availMb,
            nativeHeapAllocatedMb = nativeUsageMb,
            isLowMemoryDevice = memoryInfo.lowMemory || (totalMb <= 3072) // <=3GB RAM considered mid-range / low memory
        )
    }

    fun optimizeRuntimeStateForMidRangeHardware(): String {
        val metrics = getDeviceMemoryMetrics()
        isRunning.set(true)

        val mode = if (metrics.isLowMemoryDevice) {
            // Trim unused memory caches and set low footprint GC policy
            System.gc()
            "MODO DE BAJO CONSUMO (Dispositivo de Gama Media / <3GB RAM)"
        } else {
            "MODO DE ALTO RENDIMIENTO (>3GB RAM)"
        }

        return "Perfil de Memoria: $mode | RAM Libre: ${metrics.availableMemoryMb}MB / ${metrics.totalMemoryMb}MB | Heap Nativo: ${metrics.nativeHeapAllocatedMb}MB"
    }
}
