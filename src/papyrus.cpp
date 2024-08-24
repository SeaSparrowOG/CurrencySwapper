#include "papyrus.h"

#include "hooks.h"

namespace Papyrus {

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

	bool Bind(VM& a_vm)
	{
		BIND(SetCurrencyConsole);
		BIND(SetCurrency);
		BIND(GetCurrency);
		return true;
	}

	bool RegisterFunctions(VM* a_vm) {
		Bind(*a_vm);
		return true;
	}
}