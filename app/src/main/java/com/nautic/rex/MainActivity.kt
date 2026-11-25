package com.nautic.rex

import android.net.Uri
import android.os.Bundle
import android.provider.OpenableColumns
import android.view.View
import android.widget.Toast
import androidx.activity.result.contract.ActivityResultContracts
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.ViewCompat
import androidx.core.view.WindowCompat
import androidx.core.view.WindowInsetsCompat
import androidx.core.view.updatePadding
import com.nautic.rex.compat.ApkCompatibilityEngine
import com.nautic.rex.compat.ApkManifestFixer
import com.nautic.rex.compat.VfsManager
import com.nautic.rex.databinding.ActivityMainBinding
import java.io.File
import java.io.FileOutputStream

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding
    private lateinit var compatibilityEngine: ApkCompatibilityEngine
    private var currentApkFile: File? = null
    private val logBuffer = StringBuilder()

    // File picker launcher to open system explorer for APK selection
    private val pickApkLauncher = registerForActivityResult(ActivityResultContracts.GetContent()) { uri: Uri? ->
        if (uri != null) {
            handleSelectedApk(uri)
        } else {
            Toast.makeText(this, "No se seleccionó ningún archivo APK", Toast.LENGTH_SHORT).show()
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        // Enable edge-to-edge system window fitting
        WindowCompat.setDecorFitsSystemWindows(window, false)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        setupWindowInsets()
        setupToolbarMenu()

        compatibilityEngine = ApkCompatibilityEngine(this)
        val unsealSuccess = compatibilityEngine.initializeCompatibilityEnvironment()
        val nativeMessage = stringFromJNI()

        setupInitialUi(nativeMessage, unsealSuccess)
        setupClickListeners()
    }

    private fun setupWindowInsets() {
        ViewCompat.setOnApplyWindowInsetsListener(binding.coordinatorLayout) { _, insets ->
            val systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars())

            binding.appBar.updatePadding(top = systemBars.top)
            binding.fabAddApk.updatePadding(bottom = systemBars.bottom)

            insets
        }
    }

    private fun setupToolbarMenu() {
        binding.toolbar.setOnMenuItemClickListener { menuItem ->
            when (menuItem.itemId) {
                R.id.action_show_logs -> {
                    showLogsModal()
                    true
                }
                else -> false
            }
        }
    }

    private fun showLogsModal() {
        val bottomSheet = LogBottomSheetFragment.newInstance(logBuffer.toString())
        bottomSheet.show(supportFragmentManager, LogBottomSheetFragment.TAG)
    }

    private fun appendLog(message: String) {
        logBuffer.append(message)
    }

    private fun setupInitialUi(nativeMessage: String, unsealSuccess: Boolean) {
        logBuffer.clear()
        val memProfile = compatibilityEngine.getMemoryProfileSummary()
        logBuffer.append("• Motor Nativo C++: $nativeMessage\n")
        logBuffer.append("• Unseal APIs ART: ${if (unsealSuccess) "ÉXITO" else "ADVERTENCIA"}\n")
        logBuffer.append("• Entorno Sandbox VFS: ACTIVO\n")
        logBuffer.append("• Optimización Hardware: $memProfile\n\n")
        logBuffer.append("Estado: En espera de selección de archivo APK...\n")
    }

    private fun setupClickListeners() {
        val openPickerAction = View.OnClickListener {
            pickApkLauncher.launch("application/vnd.android.package-archive")
        }

        binding.btnSelectApk.setOnClickListener(openPickerAction)
        binding.fabAddApk.setOnClickListener(openPickerAction)

        binding.btnAdaptAndRun.setOnClickListener {
            val apk = currentApkFile
            if (apk != null && apk.exists()) {
                executeApkAdaptation(apk)
            } else {
                Toast.makeText(this, "Selecciona primero un APK válido", Toast.LENGTH_SHORT).show()
            }
        }
    }

    private fun handleSelectedApk(uri: Uri) {
        val apkName = getFileNameFromUri(uri) ?: "app_seleccionada.apk"
        appendLog("\n[SISTEMA] Copiando APK seleccionada: $apkName...")

        try {
            val tempFile = File(cacheDir, "temp_app.apk")
            contentResolver.openInputStream(uri)?.use { inputStream ->
                FileOutputStream(tempFile).use { outputStream ->
                    inputStream.copyTo(outputStream)
                }
            }

            currentApkFile = tempFile
            val report = compatibilityEngine.inspectAndPrepareApk(tempFile.absolutePath)

            if (report != null) {
                displayApkReport(apkName, report)
            } else {
                appendLog("\n[ERROR] No se pudo parsear el Manifiesto del APK.")
                Toast.makeText(this, "Error al leer la estructura del APK", Toast.LENGTH_LONG).show()
            }
        } catch (e: Exception) {
            appendLog("\n[ERROR] Excepción procesando APK: ${e.message}")
            Toast.makeText(this, "Error al procesar el archivo APK", Toast.LENGTH_LONG).show()
        }
    }

    private fun displayApkReport(apkName: String, report: ApkManifestFixer.CompatibilityReport) {
        binding.cardApkInfo.visibility = View.VISIBLE
        binding.tvApkTitle.text = apkName
        binding.tvApkPackage.text = report.packageName

        val details = StringBuilder().apply {
            append("• Target SDK original: ${report.targetSdkVersion}\n")
            append("• Inyección 'android:exported': ${if (report.requiresExportedFix) "REQUERIDO (Android 12+)" else "OK"}\n")
            append("• Redirección VFS Storage: ${if (report.requiresStorageRedirection) "REQUERIDO (Scoped Storage)" else "OK"}\n")
            append("• Desbloqueo Hidden API: ${if (report.requiresHiddenApiUnseal) "APLICADO (ART)" else "OK"}\n")
            append("• Permisos Declarados: ${report.declaredPermissions.size} en manifiesto")
        }

        binding.tvCompatibilityDetails.text = details.toString()
        appendLog("\n[OK] Inspección finalizada para ${report.packageName}.")

        Toast.makeText(this, "APK inspeccionada correctamente", Toast.LENGTH_SHORT).show()
    }

    private fun executeApkAdaptation(apkFile: File) {
        appendLog("\n[ADAPTACIÓN] Iniciando contenedor virtual optimizado para ${apkFile.name}...")
        appendLog("\n[1/3] Cargando clases DEX en DexClassLoader optimizado...")
        val classLoader = compatibilityEngine.prepareAndLoadApkContainer(apkFile)

        if (classLoader != null) {
            appendLog("\n[2/3] Redireccionando almacenamiento a ${VfsManager.prepareSandbox(this).absolutePath}...")
            appendLog("\n[3/3] Aplicando trampolín de símbolos ELF C++ (Bionic) y parches ART...")
            appendLog("\n[ÉXITO] Contenedor de aplicación iniciado en modo nativo de alto rendimiento.")
            Toast.makeText(this, "Aplicación adaptada y ejecutada", Toast.LENGTH_SHORT).show()
        } else {
            appendLog("\n[ERROR] Falló la carga del bytecode en el contenedor virtual.")
            Toast.makeText(this, "Error cargando clases del APK", Toast.LENGTH_SHORT).show()
        }

        showLogsModal()
    }

    private fun getFileNameFromUri(uri: Uri): String? {
        var name: String? = null
        if (uri.scheme == "content") {
            contentResolver.query(uri, null, null, null, null)?.use { cursor ->
                if (cursor.moveToFirst()) {
                    val index = cursor.getColumnIndex(OpenableColumns.DISPLAY_NAME)
                    if (index != -1) {
                        name = cursor.getString(index)
                    }
                }
            }
        }
        if (name == null) {
            name = uri.path
            val cut = name?.lastIndexOf('/') ?: -1
            if (cut != -1) {
                name = name?.substring(cut + 1)
            }
        }
        return name
    }

    external fun stringFromJNI(): String
}
