#include "hooks.h"

namespace Hooks {
	bool BarterHooks::Install()
	{
		SKSE::AllocTrampoline(56); // 4 * 14
		auto& trampoline = SKSE::GetTrampoline();

		//1.6.1170 -> 1408ec1d9 (Actor::GetGoldAmount)
		REL::Relocation<std::uintptr_t> playerGoldTarget{ REL::ID(50957), 0x69 };
		_getPlayerGold = trampoline.write_call<5>(playerGoldTarget.address(), &GetPlayerGold);

		//1.6.1170 -> 1408ec2a9 (ExtraContainerChanges__Data::GetGoldAmount)
		REL::Relocation<std::uintptr_t> vendorGoldTarget{ REL::ID(50957), 0x139 };
		_getVendorGold = trampoline.write_call<5>(vendorGoldTarget.address(), &GetVendorGold);

		//1.6.1170 -> 1408eb997
		REL::Relocation<std::uintptr_t> saleGoldTarget{ REL::ID(50951), 0x257 };
		_getGoldFromSale = trampoline.write_call<5>(saleGoldTarget.address(), &GetGoldFromSale);

		//1.6.1170 -> 1408eb861
		REL::Relocation<std::uintptr_t> purchaseGoldTarget{ REL::ID(50951), 0x121 };
		_getGoldFromPurchase = trampoline.write_call<5>(purchaseGoldTarget.address(), &GetGoldFromPurchase);

		return true;
	}

	void BarterHooks::RevertCurrency()
	{
		currency = nullptr;
	}

	bool BarterHooks::SetCurrency(RE::TESForm* a_newCurrency)
	{
		if (!a_newCurrency) return false;
		auto* newCurrency = static_cast<RE::TESBoundObject*>(a_newCurrency);
		if (!newCurrency) return false;

		this->currency = newCurrency;
		return true;
	}

	RE::TESForm* BarterHooks::GetCurrency()
	{
		return this->currency->As<RE::TESForm>();
	}

	//Hook code
	int BarterHooks::GetPlayerGold(RE::Actor* a_player)
	{
		if (currency) {
			if (a_player->GetInventoryCounts().contains(currency)) {
				return a_player->GetInventoryCounts().at(currency);
			}
			else {
				return 0;
			}
		}
		return _getPlayerGold(a_player);
	}

	int BarterHooks::GetVendorGold(RE::InventoryChanges* a_changes)
	{
		if (!currency) return _getVendorGold(a_changes);

		for (auto* entry : *a_changes->entryList) {
			if (entry->object == currency) {
				return entry->countDelta;
			}
		}
		return 0;
	}

	RE::TESForm* BarterHooks::GetGoldFromSale(int a_formID)
	{
		if (currency) {
			return currency;
		}
		return _getGoldFromSale(a_formID);
	}

	void BarterHooks::GetGoldFromPurchase(RE::InventoryChanges* a_inventoryChanges, RE::Actor* a_buyer, int a_value, RE::ItemList* unknown)
	{
		if (currency) {
			a_buyer->RemoveItem(currency, a_value, RE::ITEM_REMOVE_REASON::kRemove, nullptr, nullptr);
		}
		else {
			return _getGoldFromPurchase(a_inventoryChanges, a_buyer, a_value, unknown);
		}
		return _getGoldFromPurchase(a_inventoryChanges, a_buyer, 0, unknown);
	}
}