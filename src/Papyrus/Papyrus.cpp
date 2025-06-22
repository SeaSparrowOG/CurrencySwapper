#include "Papyrus.h"

#include "CurrencyManager/CurrencyManager.h"

namespace Papyrus {

	static void ResetCurrency(STATIC_ARGS) {
		CurrencyManager::RevertCurrency();
	}

	static bool SetCurrency(STATIC_ARGS, RE::TESForm* a_newCurrency) {
		return CurrencyManager::SetCurrency(a_newCurrency);
	}

	bool SetCurrencyConsole(STATIC_ARGS, RE::BSString a_newCurrencyEDID) {
		if (a_newCurrencyEDID.empty()) return false;

		auto* foundForm = RE::TESForm::LookupByEditorID<RE::TESForm>(a_newCurrencyEDID);
		if (!foundForm) return false;

		return CurrencyManager::SetCurrency(foundForm);
	}

	RE::TESForm* GetCurrency(STATIC_ARGS) {
		return CurrencyManager::GetCurrency();
	}

	void RegisterFormForAllEvents(STATIC_ARGS, RE::TESForm* a_form) {
		if (!a_form) return;
		CurrencyManager::RegisterFormForEvents(a_form);
	}

	void SetTrainingOverrides(STATIC_ARGS, bool a_overrideMultiplier, float a_multiplier, bool a_overrideBase, float a_baseOverride) {
		CurrencyManager::SetTrainingOverrides(a_overrideMultiplier, a_multiplier, a_overrideBase, a_baseOverride);
	}

	inline std::string tolower(std::string_view a_str)
	{
		std::string result(a_str);
		std::ranges::transform(result, result.begin(), [](unsigned char ch) { return static_cast<unsigned char>(std::tolower(ch)); });
		return result;
	}

	void SetTrainingOverridesConsole(STATIC_ARGS, RE::BSFixedString a_overrideMultiplier, RE::BSFixedString a_multiplier, RE::BSFixedString a_overrideBase, RE::BSFixedString a_baseOverride) {
		if (a_overrideMultiplier.empty() || a_multiplier.empty() || a_overrideBase.empty() || a_baseOverride.empty()) return;
		
		auto sanitizeOverrideMult = tolower(std::string(a_overrideMultiplier.c_str()));
		auto sanitizedOverrideBase = tolower(std::string(a_overrideBase.c_str()));
		bool overrideMult = sanitizeOverrideMult == "true" || sanitizeOverrideMult == "1";
		bool overrideBase = sanitizedOverrideBase == "true" || sanitizedOverrideBase == "1";

		try {
			float multiplier = std::stof(a_multiplier.c_str());
			float baseOverride = std::stof(a_baseOverride.c_str());
			CurrencyManager::SetTrainingOverrides(overrideMult, multiplier, overrideBase, baseOverride);
		}
		catch (const std::exception&) {
			logger::warn("Caught exception when converting strings to floats in SetTrainingOverridesConsole");
		}
	}

	void ReloadINISettings(STATIC_ARGS) {
		CurrencyManager::ReloadIniSettings();
	}

	bool Bind(VM& a_vm)
	{
		BIND(ResetCurrency);
		BIND(SetCurrencyConsole);
		BIND(SetCurrency);
		BIND(GetCurrency);
		BIND(RegisterFormForAllEvents);
		BIND(SetTrainingOverrides);
		BIND(SetTrainingOverridesConsole);
		BIND(ReloadINISettings);
		return true;
	}

	bool RegisterFunctions(VM* a_vm) {
		Bind(*a_vm);
		return true;
	}
}