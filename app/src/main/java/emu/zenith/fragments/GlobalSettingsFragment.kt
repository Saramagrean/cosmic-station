package emu.zenith.fragments

import android.os.Bundle
import androidx.preference.PreferenceFragmentCompat
import emu.zenith.R

class GlobalSettingsFragment : PreferenceFragmentCompat() {
    override fun onCreatePreferences(savedInstanceState: Bundle?, rootKey: String?) {
        addPreferencesFromResource(R.xml.global_settings)
    }
}