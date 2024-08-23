#include "papyrus.h"

#include "hooks.h"

namespace Papyrus {

	bool SetCurrency(STATIC_ARGS, RE::TESForm* a_newCurrency) {
		return Hooks::BarterHooks::GetSingleton()->SetCurrency(a_newCurrency);
	}

	RE::TESForm* GetCurrency(STATIC_ARGS) {
		return Hooks::BarterHooks::GetSingleton()->GetCurrency();
	}

	bool Bind(VM& a_vm)
	{
		BIND(SetCurrency);
		BIND(GetCurrency);
		return true;
	}

	bool RegisterFunctions(VM* a_vm) {
		Bind(*a_vm);
		return true;
	}
}