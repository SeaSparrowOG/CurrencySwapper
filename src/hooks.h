#pragma once

namespace Hooks {
	class BarterHooks : public ISingleton<BarterHooks> {
	public:
		bool Install();

		void         RevertCurrency();
		bool         SetCurrency(RE::TESForm* a_newCurrency);
		RE::TESForm* GetCurrency();
	private:
		static int          GetPlayerGold(RE::Actor* a_player);
		static int          GetVendorGold(RE::InventoryChanges* a_changes);
		static void         GetGoldFromPurchase(RE::InventoryChanges* a_inventoryChanges,
			                     RE::Actor* a_buyer, int a_value, RE::ItemList* unknown);
		static void         RemoveCurrency(RE::TESForm* a_formToRemove, int a_ammount,
								 bool arg3, bool arg4, const char* arg5);
		static uint32_t*    MoveCurrency(RE::InventoryChanges* a_inventoryChanges, uint32_t* param_2, RE::Actor* a_actor, RE::TESForm* a_form,
										 uint64_t a_concatResult, int arg6, long long** arg7, RE::ItemList* arg8, long long** arg9, long long** arg10);
		static RE::TESForm* GetGoldFromSale(int a_formID);

		inline static REL::Relocation<decltype(&GetPlayerGold)>       _getPlayerGold;
		inline static REL::Relocation<decltype(&GetVendorGold)>       _getVendorGold;
		inline static REL::Relocation<decltype(&GetGoldFromSale)>     _getGoldFromSale;
		inline static REL::Relocation<decltype(&GetGoldFromPurchase)> _getGoldFromPurchase;

		inline static RE::TESBoundObject* currency{ nullptr };
	};
}