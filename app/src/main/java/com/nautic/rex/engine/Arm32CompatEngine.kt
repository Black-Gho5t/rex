package com.nautic.rex.engine

import android.content.Context
import android.util.Log
import java.io.File
import java.util.zip.ZipFile

class Arm32CompatEngine(private val context: Context) {

    companion object {
        private const val TAG = "Arm32CompatEngine"

        init {
            try {
                System.loadLibrary("rex")
            } catch (e: UnsatisfiedLinkError) {
                Log.e(TAG, "UnsatisfiedLinkError loading rex native library: ${e.message}")
            }
        }
    }

    private external fun loadNativeLibraryNative(apkPath: String, soName: String): Boolean

    fun extractAndBind32BitNativeLibs(apkFile: File): List<String> {
        val loadedLibs = mutableListOf<String>()
        if (!apkFile.exists()) return loadedLibs

        try {
            val zipFile = ZipFile(apkFile)
            val entries = zipFile.entries()

            val nativeLibDir = File(context.codeCacheDir, "native_libs_32")
            if (!nativeLibDir.exists()) {
                nativeLibDir.mkdirs()
            }

            while (entries.hasMoreElements()) {
                val entry = entries.nextElement()
                val name = entry.name

                // Look for 32-bit ARM or x86 libraries in the APK (e.g. lib/armeabi-v7a/libgame.so)
                if (name.startsWith("lib/armeabi-v7a/") || name.startsWith("lib/armeabi/") || name.startsWith("lib/x86/")) {
                    if (name.endsWith(".so")) {
                        val soName = File(name).name
                        val targetFile = File(nativeLibDir, soName)

                        zipFile.getInputStream(entry).use { inputStream ->
                            targetFile.outputStream().use { outputStream ->
                                inputStream.copyTo(outputStream)
                            }
                        }

                        Log.i(TAG, "Extracted 32-bit native library: $soName -> ${targetFile.absolutePath}")

                        // Bind and initialize real-time ISA translation for the 32-bit library
                        val success = loadNativeLibraryNative(apkFile.absolutePath, targetFile.absolutePath)
                        if (success) {
                            loadedLibs.add(soName)
                        }
                    }
                }
            }
            zipFile.close()
        } catch (e: Exception) {
            Log.e(TAG, "Error extracting 32-bit native libraries from APK: ${e.message}", e)
        }

        return loadedLibs
    }
}
