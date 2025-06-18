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

	bool Bind(VM& a_vm)
	{
		BIND(ResetCurrency);
		BIND(SetCurrencyConsole);
		BIND(SetCurrency);
		BIND(GetCurrency);
		BIND(RegisterFormForAllEvents);
		return true;
	}

	bool RegisterFunctions(VM* a_vm) {
		Bind(*a_vm);
		return true;
	}
}