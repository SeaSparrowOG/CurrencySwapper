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

	inline uint64_t CalculateTrainingCostHook::CalculateTrainingCost(uint32_t a_skillLevel) {
		LOG_DEBUG("Training: Called CalculateTrainingCost with skill level: {}"sv, a_skillLevel);
		auto response = _calculateTrainingCost(a_skillLevel);
		LOG_DEBUG("  >Calculated training cost: {}"sv, response);
		return response;
	}

	inline void RemovePlayerGoldHook::RemovePlayerGold(RE::PlayerCharacter* a_this, void* a2, int32_t a_amount) {
		if (!CurrencyManager::ProcessTrainingDeal(a_this, a_amount)) {
			_removePlayerGold(a_this, a2, a_amount);
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
}