#pragma once

namespace Hooks
{
	namespace Notifications
	{
		inline static constexpr size_t allocSize = 14u * 1u;
		bool Install();

		struct NotificationPatch
		{
			static bool Install();

			inline static void ItemAddedNotification(RE::TESForm* a_item, uint32_t a_count, bool a_3, bool a_4, char* a_name);
			inline static REL::Relocation<decltype(ItemAddedNotification)> _itemAddedNotification;
		};
	}
}