package com.nautic.rex

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import com.google.android.material.bottomsheet.BottomSheetDialogFragment
import com.nautic.rex.databinding.DialogLogsBottomSheetBinding

class LogBottomSheetFragment : BottomSheetDialogFragment() {

    private var _binding: DialogLogsBottomSheetBinding? = null
    private val binding get() = _binding!!

    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View {
        _binding = DialogLogsBottomSheetBinding.inflate(inflater, container, false)
        return binding.root
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        val logsText = arguments?.getString(ARG_LOGS_TEXT) ?: "No hay registros disponibles."
        binding.tvModalLogOutput.text = logsText

        binding.btnCloseLogs.setOnClickListener {
            dismiss()
        }
    }

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
    }

    companion object {
        const val TAG = "LogBottomSheetFragment"
        private const val ARG_LOGS_TEXT = "arg_logs_text"

        fun newInstance(logs: String): LogBottomSheetFragment {
            val fragment = LogBottomSheetFragment()
            val args = Bundle().apply {
                putString(ARG_LOGS_TEXT, logs)
            }
            fragment.arguments = args
            return fragment
        }
    }
}
