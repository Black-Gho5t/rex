package com.nautic.rex.compat

import android.content.Context
import java.io.File

object VfsManager {

    fun prepareSandbox(context: Context): File {
        val sandboxDir = File(context.filesDir, "virtual_sdcard")
        if (!sandboxDir.exists()) {
            sandboxDir.mkdirs()
        }
        val DCIM = File(sandboxDir, "DCIM")
        val Documents = File(sandboxDir, "Documents")
        val Download = File(sandboxDir, "Download")
        val Pictures = File(sandboxDir, "Pictures")

        DCIM.mkdirs()
        Documents.mkdirs()
        Download.mkdirs()
        Pictures.mkdirs()

        return sandboxDir
    }
}
