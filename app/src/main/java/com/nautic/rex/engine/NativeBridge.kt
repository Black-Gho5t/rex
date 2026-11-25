package com.nautic.rex.engine

import android.content.Context
import android.util.Log

object NativeBridge {

    private const val TAG = "RexNativeBridge"

    init {
        try {
            System.loadLibrary("rex")
            Log.i(TAG, "Native Rex library loaded for low-level execution engine.")
        } catch (e: UnsatisfiedLinkError) {
            Log.e(TAG, "UnsatisfiedLinkError loading native library: ${e.message}")
        }
    }

    external fun initNativeCore(context: Context, sandboxPath: String): Boolean
    external fun getNativeMemoryUsage(): Long
}
