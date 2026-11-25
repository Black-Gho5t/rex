package com.nautic.rex.compat

import android.content.Context
import android.util.Log
import com.nautic.rex.engine.Arm32CompatEngine
import com.nautic.rex.engine.OptimizedStateManager
import com.nautic.rex.engine.VirtualContainer
import java.io.File

class ApkCompatibilityEngine(private val context: Context) {

    companion object {
        private const val TAG = "ApkCompatEngine"

        init {
            try {
                System.loadLibrary("rex")
                Log.i(TAG, "Native C++ Rex library loaded successfully.")
            } catch (e: UnsatisfiedLinkError) {
                Log.e(TAG, "Failed to load native Rex C++ library: ${e.message}")
            }
        }
    }

    private val virtualContainer = VirtualContainer(context)
    private val stateManager = OptimizedStateManager(context)
    private val arm32Engine = Arm32CompatEngine(context)

    private external fun unsealHiddenApisNative(): Boolean
    private external fun initVfsSandboxNative(sandboxPath: String)
    private external fun translatePathNative(originalPath: String): String

    fun initializeCompatibilityEnvironment(): Boolean {
        Log.i(TAG, "Initializing Rex Low-Level Engine & Compatibility Layer...")

        // 1. Unseal ART Hidden APIs via C++ JNI
        val hiddenApisUnsealed = unsealHiddenApisNative()
        Log.i(TAG, "Hidden API Unseal result: $hiddenApisUnsealed")

        // 2. Prepare Virtual File System Sandbox & Native Engine
        val sandboxDir = VfsManager.prepareSandbox(context)
        initVfsSandboxNative(sandboxDir.absolutePath)
        virtualContainer.prepareContainer(sandboxDir.absolutePath)

        // 3. Ensure Default Notification Channel is present
        NotificationCompatManager.ensureDefaultChannelCreated(context)

        // 4. Optimize memory footprint for mid-range hardware
        val memProfile = stateManager.optimizeRuntimeStateForMidRangeHardware()
        Log.i(TAG, "Engine State: $memProfile")

        return hiddenApisUnsealed
    }

    fun prepareAndLoadApkContainer(apkFile: File): ClassLoader? {
        // 1. Bind and translate 32-bit ARM native libraries from APK
        val extractedLibs = arm32Engine.extractAndBind32BitNativeLibs(apkFile)
        Log.i(TAG, "Extracted and translated ${extractedLibs.size} 32-bit native libraries: $extractedLibs")

        // 2. Load DEX bytecode into optimized virtual container
        return virtualContainer.loadApkClasses(apkFile)
    }

    fun getMemoryProfileSummary(): String {
        return stateManager.optimizeRuntimeStateForMidRangeHardware()
    }

    fun translateLegacyPath(path: String): String {
        return translatePathNative(path)
    }

    fun inspectAndPrepareApk(apkPath: String): ApkManifestFixer.CompatibilityReport? {
        val fixer = ApkManifestFixer(context)
        return fixer.inspectApk(apkPath)
    }
}
