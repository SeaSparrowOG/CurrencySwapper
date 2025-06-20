#pragma once

namespace Hooks
{
	namespace Training
	{
		inline static constexpr size_t allocSize = 4u * 14u;
		bool Install();

		struct SetupTrainingMenuHook {
			inline static bool Install();
			inline static void SetupTrainingMenu(RE::TrainingMenu* a_this);
			inline static REL::Relocation<decltype(SetupTrainingMenu)> _setupTrainingMenu;
		};

		struct GetPlayerGoldHook {
			inline static bool Install();
			inline static int32_t GetPlayerGold(RE::Actor* a_player);
			inline static REL::Relocation<decltype(GetPlayerGold)> _getPlayerGold;
		};

		struct CalculateTrainingCostHook {
			inline static bool Install();
			inline static uint64_t CalculateTrainingCost(uint32_t a_skillLevel);
			inline static REL::Relocation<decltype(CalculateTrainingCost)> _calculateTrainingCost;
		};

		struct RemovePlayerGoldHook {
			inline static bool Install();
			inline static void RemovePlayerGold(RE::PlayerCharacter* a_this, void* a2, int32_t a_amount);
			inline static REL::Relocation<decltype(RemovePlayerGold)> _removePlayerGold;
		};
	}
}