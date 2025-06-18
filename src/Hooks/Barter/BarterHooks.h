#pragma once

namespace Hooks
{
	namespace Barter
	{
		inline static constexpr size_t allocSize = 8u * 14u;

		bool Install();

		struct GetPlayerGoldHook {
			inline static bool Install();
			inline static int32_t GetPlayerGold(RE::Actor* a_player);
			inline static REL::Relocation<decltype(GetPlayerGold)> _getPlayerGold;
		};

		struct GetVendorGoldHook {
			inline static bool Install();
			inline static int32_t GetVendorGold(RE::InventoryChanges* a_vendorInventory);
			inline static REL::Relocation<decltype(GetVendorGold)> _getVendorGold;
		};

		struct GetGoldFromSaleHook {
			inline static bool Install();
			inline static RE::TESForm* GetGoldFromSale(RE::FormID a_id);
			inline static REL::Relocation<decltype(GetGoldFromSale)> _getGoldFromSale;
		};

		struct GetGoldFromPurchaseHook {
			inline static bool Install();
			inline static int32_t GetGoldFromPurchase(RE::InventoryChanges* a_inventoryChanges,
				RE::Actor* a_buyer, 
				int32_t a_value,
				RE::ItemList* a_itemList);
			inline static REL::Relocation<decltype(GetGoldFromPurchase)> _getGoldFromPurchase;
		};

		struct RawDealHook {
			inline static bool Install();
			inline static RE::TESForm* ProcessRawDeal(uint64_t* param_1,
				const char* a_message,
				uint64_t a_concatResult,
				uint64_t param_4);
			inline static REL::Relocation<decltype(ProcessRawDeal)> _processRawDeal;
		};

		struct RejectedDealHook {
			inline static bool Install();
			inline static RE::TESForm* ProcessRejectedDeal(const char* a_message,
				const char* a_functionName,
				uint64_t a_value);
			inline static REL::Relocation<decltype(ProcessRejectedDeal)> _processRejectedDeal;
		};

		struct RecalcVendorGoldHook {
			inline static bool Install();
			inline static RE::TESForm* RecalcVendorGold(const char* a_message,
				const char* a_functionName,
				uint64_t a_value);
			inline static REL::Relocation<decltype(RecalcVendorGold)> _recalcVendorGold;
		};

		struct ShowBarterMenuHook {
			inline static bool Install();
			inline static void* ShowBarterMenu(RE::TESObjectREFR* a_actor, void* arg2);
			inline static REL::Relocation<decltype(ShowBarterMenu)> _showBarterMenu;
		};
	}
}