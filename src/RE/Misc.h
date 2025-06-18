#pragma once

#include "Offset.h"

namespace RE
{
	static void         GoldRemovedMessage(RE::TESForm* a_formToRemove,
		int a_ammount,
		bool arg3,
		bool arg4,
		const char* arg5)
	{
		using func_t = decltype(&GoldRemovedMessage);
		static REL::Relocation<func_t> func{ RE::Offset::PlayerCharacter::RemoveItem };
		return func(a_formToRemove, a_ammount, arg3, arg4, arg5);
	}

	static uint32_t* MoveGoldBetweenContainers(RE::InventoryChanges* a_inventoryChanges, 
		uint32_t* param_2,
		RE::Actor* a_actor,
		RE::TESForm* a_form,
		uint64_t a_concatResult,
		int arg6,
		long long** arg7,
		RE::ItemList* arg8,
		long long** arg9,
		long long** arg10)
	{
		using func_t = decltype(&MoveGoldBetweenContainers);
		static REL::Relocation<func_t> func{ RE::Offset::InventoryChanges::MoveItemBetweenContainers };
		return func(a_inventoryChanges, param_2, a_actor, a_form, a_concatResult, arg6, arg7, arg8, arg9, arg10);
	}
}