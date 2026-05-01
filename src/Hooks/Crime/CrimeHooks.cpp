#include "CrimeHooks.h"

#include "CurrencyManager/CurrencyManager.h"

namespace Hooks::Crime {
	bool RemovePlayerCrimeGoldHook::InstallCrimeHook() {
		logger::info("  >Installing the PayCrime hook..."sv);
		REL::Relocation<std::uintptr_t> target{ REL::ID(40659), 0x12B };
		if (!REL::make_pattern<"E8">().match(target.address())) {
			logger::critical("    >Failed to validate the hook pattern."sv);
			return false;
		}
		auto& trampoline = SKSE::GetTrampoline();
		_removeBountyCurrency = trampoline.write_call<5>(target.address(), &RemoveBountyCurrency);
		return true;
	}

	inline void RemovePlayerCrimeGoldHook::RemoveBountyCurrency(RE::PlayerCharacter* a_this, std::int32_t a_zero, std::int32_t a_amount) {
		auto* altCurrency = CurrencyManager::GetCurrency();
		auto* boundCurrency = altCurrency ? skyrim_cast<RE::TESBoundObject*>(altCurrency) : nullptr;
		if (!boundCurrency || !a_this) {
			_removeBountyCurrency(a_this, a_zero, a_amount);
			return;
		}
		a_this->RemoveItem(boundCurrency, a_amount, RE::ITEM_REMOVE_REASON::kRemove, nullptr, nullptr);
	}

	bool CanPayCrimeGoldHook::InstallCanPayCrimeGoldHook() {
		logger::info("  >Installing the CanPayBounty hook..."sv);
		REL::Relocation<std::uintptr_t> target{ REL::ID(21704), 0x3C };
		if (!REL::make_pattern<"E8">().match(target.address())) {
			logger::critical("    >Failed to validate the hook pattern."sv);
			return false;
		}
		auto& trampoline = SKSE::GetTrampoline();
		_getGoldAmount = trampoline.write_call<5>(target.address(), &GetGoldAmount);
		return true;
	}

	inline std::int32_t CanPayCrimeGoldHook::GetGoldAmount(RE::PlayerCharacter* a_this) {
		auto* altCurrency = CurrencyManager::GetCurrency();
		auto* altCurrencyBound = altCurrency ? skyrim_cast<RE::TESBoundObject*>(altCurrency) : nullptr;
		if (!altCurrencyBound) {
			return _getGoldAmount(a_this);
		}
		return a_this->GetItemCount(altCurrencyBound);
	}
}