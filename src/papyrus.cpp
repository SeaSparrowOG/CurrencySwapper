#include "papyrus.h"

#include "hooks.h"

namespace Papyrus {

	void ResetCurrency(STATIC_ARGS) {
		Hooks::BarterHooks::GetSingleton()->RevertCurrency();
	}

	bool SetCurrency(STATIC_ARGS, RE::TESForm* a_newCurrency) {
		return Hooks::BarterHooks::GetSingleton()->SetCurrency(a_newCurrency);
	}

	bool SetCurrencyConsole(STATIC_ARGS, RE::BSString a_newCurrencyEDID) {
		if (a_newCurrencyEDID.empty()) return false;

		auto* foundForm = RE::TESForm::LookupByEditorID<RE::TESBoundObject>(a_newCurrencyEDID);
		if (!foundForm) return false;

		return Hooks::BarterHooks::GetSingleton()->SetCurrency(foundForm);
	}

	RE::TESForm* GetCurrency(STATIC_ARGS) {
		return Hooks::BarterHooks::GetSingleton()->GetCurrency();
	}

	void RegisterFormForAllEvents(STATIC_ARGS, RE::TESForm* a_form) {
		if (!a_form) return;
		Hooks::BarterHooks::GetSingleton()->RegisterFormForAllEvents(a_form);
	}

	bool Bind(VM& a_vm)
	{
		BIND(SetCurrencyConsole);
		BIND(SetCurrency);
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