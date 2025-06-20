#include "TrainingHooks.h"

#include "CurrencyManager/CurrencyManager.h"
#include "RE/Misc.h"

namespace Hooks::Training
{
	void SetupTrainingMenuHook::SetupTrainingMenu(RE::TrainingMenu* a_this)
	{
		_setupTrainingMenu(a_this);
		CurrencyManager::SendCustomTrainingMenuEvent(a_this);
	}

	int32_t GetPlayerGoldHook::GetPlayerGold(RE::Actor* a_player) {
		LOG_DEBUG("Training: Called GetPlayerGold."sv);
		int32_t response = 0;
		if (CurrencyManager::GetPlayerGold(a_player, response)) {
			LOG_DEBUG("  >Player gold retrieved successfully: {}"sv, response);
			return response;
		}
		return _getPlayerGold(a_player);
	}

	inline float CalculateTrainingCostHook::CalculateTrainingCost(uint32_t a_skillLevel) {
		LOG_DEBUG("Training: Called CalculateTrainingCost with skill level: {}"sv, a_skillLevel);
		auto response = _calculateTrainingCost(a_skillLevel);
		LOG_DEBUG("  >Calculated training cost: {}"sv, response);
		return response;
	}

	inline void RemovePlayerGoldHook::RemovePlayerGold(RE::PlayerCharacter* a_this, void* a2, int32_t a_amount) {
		if (!CurrencyManager::ProcessTrainingDeal(a_this, a_amount)) {
			_removePlayerGold(a_this, a2, a_amount);
		}
		LOG_DEBUG("  >Removed {} currency."sv, a_amount);
	}

	inline void SendNotEnoughGoldMessageHook::SendNotEnoughGoldMessage(const char* a_message,
		const char* a_sound,
		bool a_cancelIfQueued)
	{
		auto* currency = CurrencyManager::GetCurrency();
		if (currency) {
			RE::DebugNotification(
				fmt::format("You don't have enough {}", currency->GetName()).c_str(),
				a_sound,
				a_cancelIfQueued);
		}
		else {
			_sendNotEnoughGoldMessage(a_message, a_sound, a_cancelIfQueued);
		}
	}

	inline int32_t UpdateTrainingCurrencyHook::UpdateTrainingCurrency(RE::Actor* a_player)
	{
		auto* currency = CurrencyManager::GetCurrency();
		auto* bound = currency ? skyrim_cast<RE::TESBoundObject*>(currency) : nullptr;
		if (bound) {
			auto inventoryCounts = a_player->GetInventoryCounts();
			return inventoryCounts.contains(bound) ? inventoryCounts.at(bound) : 0;
		}
		else {
			_updateTrainingCurrency(a_player);
		}
	}

	/*
	=========================================================================================================
										 INSTALLATION PAST HERE
	=========================================================================================================
	*/

	bool Install() {		
		bool nominal = true;
		if (!SetupTrainingMenuHook::Install()) {
			nominal = false;
		}
		if (!GetPlayerGoldHook::Install()) {
			nominal = false;
		}
		if (!CalculateTrainingCostHook::Install()) {
			nominal = false;
		}
		if (!RemovePlayerGoldHook::Install()) {
			nominal = false;
		}
		if (!SendNotEnoughGoldMessageHook::Install()) {
			nominal = false;
		}
		if (!UpdateTrainingCurrencyHook::Install()) {
			nominal = false;
		}
		return nominal;
	}

	inline bool SetupTrainingMenuHook::Install() {
		logger::info("  >Installing the Setup Training Menu hook..."sv);
		REL::Relocation<std::uintptr_t> target{ REL::ID(52666), 0x17E };
		if (!REL::make_pattern<"E8">().match(target.address())) {
			logger::critical("    >Failed to validate the hook pattern."sv);
			return false;
		}
		auto& trampoline = SKSE::GetTrampoline();
		_setupTrainingMenu = trampoline.write_call<5>(target.address(), &SetupTrainingMenu);
		return true;
	}

	inline bool GetPlayerGoldHook::Install() {
		logger::info("  >Installing the Get Player Gold (Training) hook..."sv);
		REL::Relocation<std::uintptr_t> target{ REL::ID(52667), 0x96 };
		if (!REL::make_pattern<"E8">().match(target.address())) {
			logger::critical("    >Failed to validate the hook pattern."sv);
			return false;
		}
		auto& trampoline = SKSE::GetTrampoline();
		_getPlayerGold = trampoline.write_call<5>(target.address(), &GetPlayerGold);
		return true;
	}

	inline bool CalculateTrainingCostHook::Install() {
		logger::info("  >Installing the Calculate Training hook..."sv);
		REL::Relocation<std::uintptr_t> target{ REL::ID(52667), 0x87 };
		if (!REL::make_pattern<"E8">().match(target.address())) {
			logger::critical("    >Failed to validate the hook pattern."sv);
			return false;
		}
		auto& trampoline = SKSE::GetTrampoline();
		_calculateTrainingCost = trampoline.write_call<5>(target.address(), &CalculateTrainingCost);
		return true;
	}

	inline bool RemovePlayerGoldHook::Install() {
		logger::info("  >Installing the Remove Player Gold hook..."sv);
		REL::Relocation<std::uintptr_t> target{ REL::ID(52667), 0xC3 };
		if (!REL::make_pattern<"E8">().match(target.address())) {
			logger::critical("    >Failed to validate the hook pattern."sv);
			return false;
		}
		auto& trampoline = SKSE::GetTrampoline();
		_removePlayerGold = trampoline.write_call<5>(target.address(), &RemovePlayerGold);
		return true;
	}

	inline bool SendNotEnoughGoldMessageHook::Install() {
		logger::info("  >Installing the Send Not Enough Gold Message hook..."sv);
		REL::Relocation<std::uintptr_t> target{ REL::ID(52667), 0x1BB };
		if (!REL::make_pattern<"E8">().match(target.address())) {
			logger::critical("    >Failed to validate the hook pattern."sv);
			return false;
		}
		auto& trampoline = SKSE::GetTrampoline();
		_sendNotEnoughGoldMessage = trampoline.write_call<5>(target.address(), &SendNotEnoughGoldMessage);
		return true;
	}

	inline bool UpdateTrainingCurrencyHook::Install() {
		logger::info("  >Installing the Update Training Currency hook..."sv);
		REL::Relocation<std::uintptr_t> target{ REL::ID(52668), 0x31B };
		if (!REL::make_pattern<"E8">().match(target.address())) {
			logger::critical("    >Failed to validate the hook pattern."sv);
			return false;
		}
		auto& trampoline = SKSE::GetTrampoline();
		_updateTrainingCurrency = trampoline.write_call<5>(target.address(), &UpdateTrainingCurrency);
		return true;
	}
}