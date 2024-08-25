#pragma once

namespace Hooks {
	class BarterHooks : public ISingleton<BarterHooks>,
		public RE::BSTEventSink<RE::MenuOpenCloseEvent> {
	public:
		bool Install();

		void         RevertCurrency();
		bool         SetCurrency(RE::TESForm* a_newCurrency);
		RE::TESForm* GetCurrency();
	private:
		RE::BSEventNotifyControl ProcessEvent(
			const RE::MenuOpenCloseEvent* a_event,
			RE::BSTEventSource<RE::MenuOpenCloseEvent>*) override;

		// Returns the player's total CURRENCY, based on what the actual currency currently is.
		static int          GetPlayerGold(RE::Actor* a_player);
		// Returns the vendor's total CURRENCY, again based on the actual currency.
		static int          GetVendorGold(RE::InventoryChanges* a_changes);
		// Function that moves gold from the player to the merchant when a purchase is complete.
		static void         GetGoldFromPurchase(RE::InventoryChanges* a_inventoryChanges,
			                     RE::Actor* a_buyer, int a_value, RE::ItemList* unknown);
		// Function that moves gold from the merchant's inventory to the player's when a sale is complete.
		static RE::TESForm* GetGoldFromSale(int a_formID);
		// "Raw Deals" are deals where the vendor has less currency than the value of the thing being sold.
		static void         ProcessRawDeal(uint64_t* param_1, const char* a_message, uint64_t a_concatResult, uint64_t param_4);

		// Original function calls.
		inline static REL::Relocation<decltype(&ProcessRawDeal)>      _processRawDeal;
		inline static REL::Relocation<decltype(&GetPlayerGold)>       _getPlayerGold;
		inline static REL::Relocation<decltype(&GetVendorGold)>       _getVendorGold;
		inline static REL::Relocation<decltype(&GetGoldFromSale)>     _getGoldFromSale;
		inline static REL::Relocation<decltype(&GetGoldFromPurchase)> _getGoldFromPurchase;

		// Current currency. If nullptr, gold is used. Otherwise it is whatever form is specified.
		inline static RE::TESBoundObject* currency{ nullptr };
		inline static std::string defaultMessage{ "" };
		inline static std::string defaultReplacement{ "" };

		//The following are game functions. I call them from the GetGoldFromPurchase function.
		static void         GoldRemovedMessage(RE::TESForm* a_formToRemove, int a_ammount,
			bool arg3, bool arg4, const char* arg5);
		static uint32_t*    MoveGoldBetweenContainers(RE::InventoryChanges* a_inventoryChanges, uint32_t* param_2, 
			RE::Actor* a_actor, RE::TESForm* a_form, uint64_t a_concatResult, int arg6, long long** arg7, 
			RE::ItemList* arg8, long long** arg9, long long** arg10);
	};
}