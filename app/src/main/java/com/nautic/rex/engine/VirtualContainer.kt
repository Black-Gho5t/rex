package com.nautic.rex.engine

import android.content.Context
import android.util.Log
import dalvik.system.PathClassLoader
import java.io.File

class VirtualContainer(private val context: Context) {

    companion object {
        private const val TAG = "RexVirtualContainer"
    }

    private var loadedClassLoader: ClassLoader? = null
    private var isEngineReady = false

    fun prepareContainer(sandboxPath: String): Boolean {
        Log.i(TAG, "Initializing Low-Level Rex Container for mid-range device performance...")
        isEngineReady = NativeBridge.initNativeCore(context, sandboxPath)
        val ramUsage = NativeBridge.getNativeMemoryUsage()
        Log.i(TAG, "Container initialized. Current native memory footprint: $ramUsage bytes.")
        return isEngineReady
    }

    fun loadApkClasses(apkFile: File): ClassLoader? {
        if (!apkFile.exists()) {
            Log.e(TAG, "APK file does not exist at ${apkFile.absolutePath}")
            return null
        }

        try {
            // Android 14+ (API 34+) Security Enforcement:
            // DexFile / ClassLoader rejects any writable file. Must mark read-only.
            apkFile.setWritable(false, false)
            apkFile.setReadOnly()

            val nativeLibDir = File(context.codeCacheDir, "native_libs_32")

            Log.i(TAG, "Loading DEX bytecode into container (Read-Only enforced: ${!apkFile.canWrite()})...")

            val classLoader = PathClassLoader(
                apkFile.absolutePath,
                nativeLibDir.absolutePath,
                context.classLoader
            )

            loadedClassLoader = classLoader
            Log.i(TAG, "Successfully loaded legacy app bytecode into Virtual Container.")
            return classLoader
        } catch (e: Exception) {
            Log.e(TAG, "Failed to load APK classes into container: ${e.message}", e)
            return null
        }
    }

    fun getLoadedClassLoader(): ClassLoader? = loadedClassLoader
}
