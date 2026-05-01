#pragma once

namespace Hooks
{
	namespace Crime
	{
		inline static constexpr size_t allocSize = 2u * 14u;

		struct RemovePlayerCrimeGoldHook
		{
			static bool InstallCrimeHook();

			// 1.6.1170 -> 140747a10 + 0x12B
			inline static void RemoveBountyCurrency(RE::PlayerCharacter* a_this, std::int32_t a_zero, std::int32_t a_amount);
			inline static REL::Relocation<decltype(RemoveBountyCurrency)> _removeBountyCurrency;
		};

		struct CanPayCrimeGoldHook
		{
			static bool InstallCanPayCrimeGoldHook();

			// 1.6.1170 -> 140336480 + 0x3C
			inline static std::int32_t GetGoldAmount(RE::PlayerCharacter* a_this);
			inline static REL::Relocation<decltype(GetGoldAmount)> _getGoldAmount;
		};

		static bool InstallCrimeHooks()
		{
			bool success = true;
			success &= CanPayCrimeGoldHook::InstallCanPayCrimeGoldHook();
			success &= RemovePlayerCrimeGoldHook::InstallCrimeHook();
			return success;
		}
	}
}