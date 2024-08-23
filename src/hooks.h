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

		static RE::TESForm* GetGoldFromSale(int a_formID);

		inline static REL::Relocation<decltype(&GetPlayerGold)>       _getPlayerGold;
		inline static REL::Relocation<decltype(&GetVendorGold)>       _getVendorGold;
		inline static REL::Relocation<decltype(&GetGoldFromSale)>     _getGoldFromSale;
		inline static REL::Relocation<decltype(&GetGoldFromPurchase)> _getGoldFromPurchase;

		inline static RE::TESBoundObject* currency{ nullptr };
	};
}