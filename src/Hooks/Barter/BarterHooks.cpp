#include "BarterHooks.h"

#include "CurrencyManager/CurrencyManager.h"

namespace Hooks::Barter
{
	inline int32_t GetPlayerGoldHook::GetPlayerGold(RE::Actor* a_player) {
		LOG_DEBUG("Hook: GetPlayerGold"sv);
		int32_t out = 0;
		if (CurrencyManager::GetPlayerGold(a_player, out)) {
			LOG_DEBUG("  >Alt Currency, result: {}", out);
			return out;
		}
		return _getPlayerGold(a_player);
	}

	inline int32_t GetVendorGoldHook::GetVendorGold(RE::InventoryChanges* a_vendorInventory)
	{
		LOG_DEBUG("Hook: GetVendorGold"sv);
		int32_t out = 0;
		if (CurrencyManager::GetVendorGold(a_vendorInventory, out)) {
			LOG_DEBUG("  >Alt Currency, result: {}", out);
			return out;
		}
		return _getVendorGold(a_vendorInventory);
	}

	inline RE::TESForm* GetGoldFromSaleHook::GetGoldFromSale(RE::FormID a_id)
	{
		LOG_DEBUG("Hook: GetGoldFromSale"sv);
		RE::TESForm* response = nullptr;
		if (CurrencyManager::SendCustomSaleEvent(response)) {
			LOG_DEBUG("  >Custom, returning {}"sv, response ? response->GetName() : "NULL");
			return response;
		}
		return _getGoldFromSale(a_id);
	}

	inline int32_t GetGoldFromPurchaseHook::GetGoldFromPurchase(RE::InventoryChanges* a_inventoryChanges,
		RE::Actor* a_buyer,
		int32_t a_value,
		RE::ItemList* a_itemList)
	{
		LOG_DEBUG("Hook: GetGoldFromPurchase"sv);
		if (CurrencyManager::GetGoldFromPurchase(a_inventoryChanges, a_buyer, a_value, a_itemList)) {
			return 0;
		}
		return _getGoldFromPurchase(a_inventoryChanges, a_buyer, a_value, a_itemList);
	}

	inline RE::TESForm* RawDealHook::ProcessRawDeal(uint64_t* param_1,
		const char* a_message,
		uint64_t a_concatResult,
		uint64_t param_4)
	{
		LOG_DEBUG("Hook: ProcessRawDeal"sv);
		std::string out = "";
		if (CurrencyManager::ProcessRawDeal(a_concatResult, param_4, out)) {
			LOG_DEBUG("  >Custom Currency, raw deal warning: {}", out);
			return _processRawDeal(param_1, out.c_str(), a_concatResult, param_4);
		}
		return _processRawDeal(param_1, a_message, a_concatResult, param_4);
	}

	inline RE::TESForm* RejectedDealHook::ProcessRejectedDeal(const char* a_message, const char* a_functionName, uint64_t a_value)
	{
		LOG_DEBUG("Hook: ProcessRejectedDeal"sv);
		CurrencyManager::SendRejectedDealEvent();
		if (CurrencyManager::ProcessRejectedDeal()) {
			return CurrencyManager::GetCurrency();
		}
		return _processRejectedDeal(a_message, a_functionName, a_value);
	}

	inline bool RecalcVendorGoldHook::RecalcVendorGold(RE::GFxMovieView* a_barterMovie,
		const char* a_functionName,
		RE::GFxValue* a_updateObj)
	{
		LOG_DEBUG("Hook: RecalcVendorGold (GFx): {}"sv, a_functionName);
		bool response = _recalcVendorGold(a_barterMovie, a_functionName, a_updateObj);
		if (response) {
			CurrencyManager::ResetVendorInfo(a_updateObj);
		}
		return response;
	}

	inline void* ShowBarterMenuHook::ShowBarterMenu(RE::TESObjectREFR* a_actor, void* arg2)
	{
		LOG_DEBUG("Hook: Custom Menu"sv);
		CurrencyManager::SendBarterMenuEvent(a_actor);
		return _showBarterMenu(a_actor, arg2);
	}

	/*
	=========================================================================================================
	                                     INSTALLATION PAST HERE
	=========================================================================================================
	*/

	bool Install() {
		bool nominal = true;
		if (!GetPlayerGoldHook::Install()) {
			nominal = false;
		}
		if (!GetVendorGoldHook::Install()) {
			nominal = false;
		}
		if (!GetGoldFromSaleHook::Install()) {
			nominal = false;
		}
		if (!GetGoldFromPurchaseHook::Install()) {
			nominal = false;
		}
		if (!RawDealHook::Install()) {
			nominal = false;
		}
		if (!RejectedDealHook::Install()) {
			nominal = false;
		}
		if (!RecalcVendorGoldHook::Install()) {
			nominal = false;
		}
		if (!ShowBarterMenuHook::Install()) {
			nominal = false;
		}
		return nominal;
	}

	inline bool GetPlayerGoldHook::Install() {
		logger::info("  >Installing the Get Player Gold hook..."sv);
		REL::Relocation<std::uintptr_t> target{ REL::ID(50957), 0x69 };
		if (!REL::make_pattern<"E8">().match(target.address())) {
			logger::critical("    >Failed to validate the hook pattern."sv);
			return false;
		}
		auto& trampoline = SKSE::GetTrampoline();
		_getPlayerGold = trampoline.write_call<5>(target.address(), &GetPlayerGold);
		return true;
	}

	inline bool GetVendorGoldHook::Install() {
		logger::info("  >Installing the Get Vendor Gold hook..."sv);
		REL::Relocation<std::uintptr_t> target{ REL::ID(50957), 0x139 };
		if (!REL::make_pattern<"E8">().match(target.address())) {
			logger::critical("    >Failed to validate the hook pattern."sv);
			return false;
		}
		auto& trampoline = SKSE::GetTrampoline();
		_getVendorGold = trampoline.write_call<5>(target.address(), &GetVendorGold);
		return true;
	}

	inline bool GetGoldFromSaleHook::Install() {
		logger::info("  >Installing the Get Gold from Sale hook..."sv);
		REL::Relocation<std::uintptr_t> target{ REL::ID(50951), 0x257 };
		if (!REL::make_pattern<"E8">().match(target.address())) {
			logger::critical("    >Failed to validate the hook pattern."sv);
			return false;
		}
		auto& trampoline = SKSE::GetTrampoline();
		_getGoldFromSale = trampoline.write_call<5>(target.address(), &GetGoldFromSale);
		return true;
	}

	inline bool GetGoldFromPurchaseHook::Install() {
		logger::info("  >Installing the Get Gold from Purchase hook..."sv);
		REL::Relocation<std::uintptr_t> target{ REL::ID(50951), 0x121 };
		if (!REL::make_pattern<"E8">().match(target.address())) {
			logger::critical("    >Failed to validate the hook pattern."sv);
			return false;
		}
		auto& trampoline = SKSE::GetTrampoline();
		_getGoldFromPurchase = trampoline.write_call<5>(target.address(), &GetGoldFromPurchase);
		return true;
	}

	inline bool RawDealHook::Install() {
		logger::info("  >Installing the Raw Deal hook..."sv);
		REL::Relocation<std::uintptr_t> target{ REL::ID(50952), 0xB1 };
		if (!REL::make_pattern<"E8">().match(target.address())) {
			logger::critical("    >Failed to validate the hook pattern."sv);
			return false;
		}
		auto& trampoline = SKSE::GetTrampoline();
		_processRawDeal = trampoline.write_call<5>(target.address(), &ProcessRawDeal);
		return true;
	}

	inline bool RejectedDealHook::Install() {
		logger::info("  >Installing the Rejected Deal hook..."sv);
		REL::Relocation<std::uintptr_t> target{ REL::ID(50951), 0x1A7 };
		if (!REL::make_pattern<"E8">().match(target.address())) {
			logger::critical("    >Failed to validate the hook pattern."sv);
			return false;
		}
		auto& trampoline = SKSE::GetTrampoline();
		_processRejectedDeal = trampoline.write_call<5>(target.address(), &ProcessRejectedDeal);
		return true;
	}

	inline bool RecalcVendorGoldHook::Install() {
		logger::info("  >Installing the Recalc Vendor Gold hook..."sv);
		REL::Relocation<std::uintptr_t> target{ REL::ID(50957), 0x2F7 };
		if (!REL::make_pattern<"E8">().match(target.address())) {
			logger::critical("    >Failed to validate the hook pattern."sv);
			return false;
		}
		auto& trampoline = SKSE::GetTrampoline();
		_recalcVendorGold = trampoline.write_call<5>(target.address(), &RecalcVendorGold);
		return true;
	}

	inline bool ShowBarterMenuHook::Install() {
		logger::info("  >Installing the Show Barter Menu hook..."sv);
		REL::Relocation<std::uintptr_t> target{ REL::ID(50955), 0x21 };
		if (!REL::make_pattern<"E8">().match(target.address())) {
			logger::critical("    >Failed to validate the hook pattern."sv);
			return false;
		}
		auto& trampoline = SKSE::GetTrampoline();
		_showBarterMenu = trampoline.write_call<5>(target.address(), &ShowBarterMenu);
		return true;
	}
}