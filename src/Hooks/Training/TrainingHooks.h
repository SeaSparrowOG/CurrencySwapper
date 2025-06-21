#pragma once

namespace Hooks
{
	namespace Training
	{
		inline static constexpr size_t allocSize = 7u * 14u;
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
			inline static float CalculateTrainingCost(uint32_t a_skillLevel);
			inline static float CalculateTrainingCostTextUpdate(uint32_t a_skillLevel);
			inline static REL::Relocation<decltype(CalculateTrainingCost)> _calculateTrainingCost;
			inline static REL::Relocation<decltype(CalculateTrainingCostTextUpdate)> _calculateTrainingCostTextUpdate;
		};

		struct RemovePlayerGoldHook {
			inline static bool Install();
			inline static void RemovePlayerGold(RE::PlayerCharacter* a_this, void* a2, int32_t a_amount);
			inline static REL::Relocation<decltype(RemovePlayerGold)> _removePlayerGold;
		};

		struct SendNotEnoughGoldMessageHook {
			inline static bool Install();
			inline static void SendNotEnoughGoldMessage(const char* a_message,
				const char* a_sound,
				bool a_cancelIfQueued);
			inline static REL::Relocation<decltype(SendNotEnoughGoldMessage)> _sendNotEnoughGoldMessage;
		};

		struct UpdateTrainingCurrencyHook {
			inline static bool Install();
			inline static int32_t UpdateTrainingCurrency(RE::Actor* a_player);
			inline static REL::Relocation<decltype(UpdateTrainingCurrency)> _updateTrainingCurrency;
		};
	}
}