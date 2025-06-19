#include "CurrencyManager.h"

#include "RE/Misc.h"

namespace CurrencyManager
{
	bool GetPlayerGold(RE::Actor* a_player, int32_t& a_out) {
		auto* manager = CurrencyManager::GetSingleton();
		return manager && manager->GetPlayerGold(a_player, a_out);
	}

	bool GetVendorGold(RE::InventoryChanges* a_player, int32_t& a_out) {
		auto* manager = CurrencyManager::GetSingleton();
		return manager && manager->GetVendorGold(a_player, a_out);
	}

	bool GetGoldFromPurchase(RE::InventoryChanges* a_inventoryChanges,
		RE::Actor* a_buyer,
		int32_t a_value,
		RE::ItemList* a_itemList)
	{
		auto* manager = CurrencyManager::GetSingleton();
		return manager && manager->GetGoldFromPurchase(a_inventoryChanges,
			a_buyer,
			a_value,
			a_itemList);
	}

	bool ProcessRawDeal(uint64_t a_itemValue,
		uint64_t a_merchantGold,
		std::string& a_out)
	{
		auto* manager = CurrencyManager::GetSingleton();
		if (manager) {
			return manager->SendRawDealWarning(a_itemValue, a_merchantGold, a_out);
		}
		return false;
	}

	bool ProcessRejectedDeal()
	{
		auto* manager = CurrencyManager::GetSingleton();
		if (manager) {
			return manager->SendNotEnoughGoldWarning();
		}
		return false;
	}

	bool SendCustomSaleEvent(RE::TESForm*& a_out)
	{
		auto* manager = CurrencyManager::GetSingleton();
		if (!manager) {
			return false;
		}

		if (manager) {
			manager->SendSaleEvent();
		}
		if (auto* currency = manager->GetCurrency(); currency) {
			a_out = currency;
			return true;
		}
		return false;
	}

	void SendRejectedDealEvent()
	{
		auto* manager = CurrencyManager::GetSingleton();
		if (manager) {
			manager->SendRejectedDealEvent();
		}
	}

	void SendBarterMenuEvent(RE::TESObjectREFR* a_ref)
	{
		auto* manager = CurrencyManager::GetSingleton();
		if (manager) {
			manager->SendCustomMenuEvent(a_ref);
		}
	}

	RE::TESForm* GetCurrency() {
		auto* manager = CurrencyManager::GetSingleton();
		if (manager) {
			return manager->GetCurrency();
		}
		return nullptr;
	}

	void RevertCurrency()
	{
		auto* manager = CurrencyManager::GetSingleton();
		if (manager) {
			manager->RevertCurrency();
		}
	}

	bool SetCurrency(RE::TESForm* a_currency)
	{
		auto* manager = CurrencyManager::GetSingleton();
		if (manager) {
			return manager->SetCurrency(a_currency);
		}
		return false;
	}

	void RegisterFormForEvents(RE::TESForm* a_form)
	{
		auto* manager = CurrencyManager::GetSingleton();
		if (manager) {
			manager->RegisterFormForAllEvents(a_form);
		}
	}

	void ResetVendorInfo()
	{
		auto* manager = CurrencyManager::GetSingleton();
		if (manager) {
			manager->ResetVendorInfo();
		}
	}
}

namespace CurrencyManager
{
	bool Initialize() {
		SECTION_SEPARATOR;
		logger::info("Initializing Currency Manager..."sv);

		auto* manager = CurrencyManager::GetSingleton();
		if (!manager) {
			logger::critical("  >Failed to retrieve internal manager."sv);
			return false;
		}
		return manager->Initialize();
	}

	bool Save(SKSE::SerializationInterface* a_intfc) {
		auto* manager = CurrencyManager::GetSingleton();
		if (!manager) {
			logger::error("Save: Failed to retrieve internal Currency Manager."sv);
			return false;
		}
		return manager->Save(a_intfc);
	}

	bool Load(SKSE::SerializationInterface* a_intfc) {
		auto* manager = CurrencyManager::GetSingleton();
		if (!manager) {
			logger::error("Load: Failed to retrieve internal Currency Manager."sv);
			return false;
		}
		return manager->Load(a_intfc);
	}

	void Revert(SKSE::SerializationInterface* a_intfc) {
		auto* manager = CurrencyManager::GetSingleton();
		if (!manager) {
			logger::error("Revert: Failed to retrieve internal Currency Manager."sv);
			return;
		}
		manager->Revert(a_intfc);
	}

	bool CurrencyManager::Save(SKSE::SerializationInterface* a_intfc) {
		return true;
	}

	bool CurrencyManager::Load(SKSE::SerializationInterface* a_intfc) {
		return true;
	}

	void CurrencyManager::Revert(SKSE::SerializationInterface* a_intfc) {
		(void)a_intfc;
	}


	bool CurrencyManager::Initialize() {
		auto* sourceEventHolder = RE::UI::GetSingleton();
		if (!sourceEventHolder) {
			logger::critical("  >Failed to get the game's source event holder."sv);
			return false;
		}
		sourceEventHolder->AddEventSink(this);
		return true;
	}

	void CurrencyManager::RevertCurrency() {
		auto* oldCurrency = customCurrency ? skyrim_cast<RE::TESForm*>(customCurrency) : nullptr;
		customCurrency = nullptr;
		customRevert.QueueEvent(oldCurrency);
	}

	bool CurrencyManager::SetCurrency(RE::TESForm* a_newCurrency) {
		auto* bound = a_newCurrency ? skyrim_cast<RE::TESBoundObject*>(a_newCurrency) : nullptr;
		if (!bound) {
			logger::warn("Failed to set new currency. Provided form: {}"sv, a_newCurrency ? a_newCurrency->GetName() : "NULL");
			return false;
		}

		auto* old = customCurrency ? skyrim_cast<RE::TESForm*>(customCurrency) : nullptr;
		customCurrency = bound;
		currencySwap.QueueEvent(old, customCurrency);
		return true;
	}

	void CurrencyManager::RegisterFormForAllEvents(RE::TESForm* a_form) {
		if (!a_form) {
			return;
		}
		customRevert.Register(a_form);
		customPurchase.Register(a_form);
		customSale.Register(a_form);
		customPurchaseFail.Register(a_form);
		customBarterMenu.Register(a_form);
		currencySwap.Register(a_form);
	}

	void CurrencyManager::UnRegisterFormForAllEvents(RE::TESForm* a_form) {
		if (!a_form) {
			return;
		}
		customRevert.Unregister(a_form);
		customPurchase.Unregister(a_form);
		customSale.Unregister(a_form);
		customPurchaseFail.Unregister(a_form);
		customBarterMenu.Unregister(a_form);
		currencySwap.Unregister(a_form);
	}

	RE::TESForm* CurrencyManager::GetCurrency() {
		return customCurrency ? skyrim_cast<RE::TESForm*>(customCurrency) : nullptr;
	}

	bool CurrencyManager::GetPlayerGold(RE::Actor* a_player, int32_t& a_out) {
		if (customCurrency) {
			auto inventory = a_player->GetInventoryCounts();
			a_out = inventory.contains(customCurrency) ?
				inventory.at(customCurrency) : 0;
			return true;
		}
		return false;
	}

	bool CurrencyManager::GetVendorGold(RE::InventoryChanges* a_vendorInventoryChanges,
		int32_t& a_out) 
	{
		if (customCurrency) {
			for (auto* entry : *a_vendorInventoryChanges->entryList) {
				if (entry->object == customCurrency) {
					a_out = entry->countDelta;
					return true;
				}
			}
			a_out = 0;
			return true;
		}
		return false;
	}
	bool CurrencyManager::GetGoldFromPurchase(RE::InventoryChanges* a_inventoryChanges,
		RE::Actor* a_buyer,
		int a_value,
		RE::ItemList* param_4)
	{
		auto* customForm = customCurrency ? skyrim_cast<RE::TESForm*>(customCurrency) : nullptr;
		if (customForm) {
			RE::GoldRemovedMessage(customForm, a_value, false, true, "");
			RE::MoveGoldBetweenContainers(a_inventoryChanges, new uint32_t, a_buyer, customForm, a_value, 4, 0, param_4, 0, 0);
			customPurchase.QueueEvent(customCurrency);
			LOG_DEBUG("  >Alt Currency, sent event and moved gold."sv);
			return true;
		}
		return false;
	}

	void CurrencyManager::SendSaleEvent()
	{
		if (customCurrency) {
			customSale.QueueEvent(customCurrency);
		}
	}

	void CurrencyManager::SendCustomMenuEvent(RE::TESObjectREFR* a_ref)
	{
		barterActorID = 0;
		if (customCurrency) {
			auto* actor = a_ref ? a_ref->As<RE::Actor>() : nullptr;
			barterActorID = actor ? actor->formID : 0;
			if (actor) {
				customBarterMenu.QueueEvent(actor);
			}
			LOG_DEBUG("  >Custom currency, updated menu with Actor: {}"sv, actor ? actor->GetName() : "NULL");
		}
	}

	void CurrencyManager::SendRejectedDealEvent()
	{
		customPurchaseFail.QueueEvent(customCurrency);
	}

	bool CurrencyManager::SendRawDealWarning(uint64_t a_value, uint64_t a_merchantGold, std::string&  a_out)
	{
		if (customCurrency) {
			a_out = std::string(fmt::format("Transaction Value: {}\nVendor {}: {}",
				a_value,
				customCurrency->GetName(),
				a_merchantGold).c_str());
			return true;
		}
		return false;
	}

	bool CurrencyManager::SendNotEnoughGoldWarning() {
		if (customCurrency) {
			customPurchaseFail.QueueEvent(customCurrency);
			RE::DebugNotification(fmt::format("You don't have enough {}", customCurrency->GetName()).c_str());
			LOG_DEBUG("  >deal was rejected, returning: {}"sv, fmt::format("You don't have enough {}", customCurrency->GetName()));
			return true;
		}
		return false;
	}

	void CurrencyManager::ResetVendorInfo()
	{
		if (!customCurrency) {
			return;
		}

		auto* player = RE::PlayerCharacter::GetSingleton();
		auto* ui = RE::UI::GetSingleton();
		auto* dh = RE::TESDataHandler::GetSingleton();
		assert(player && ui && dh);
		if (!player || !ui || !dh) return;

		auto menu = ui->GetMovieView(RE::BarterMenu::MENU_NAME);
		assert(menu);
		if (!menu) return;

		RE::GFxValue var;
		bool useSkyUIPaths = dh->LookupLoadedModByName("SkyUI_SE.esp"sv);
		if (useSkyUIPaths) {
			menu->GetVariable(&var, pathToVendorLabel_SkyUI);
		}
		else {
			menu->GetVariable(&var, pathToVendorLabel);
		}

		if (var.IsUndefined()) return;
		std::string vendorInfo = "";
		auto* dialogueTargetRef = RE::TESForm::LookupByID<RE::Actor>(barterActorID);
		if (dialogueTargetRef && dialogueTargetRef->Is3DLoaded()) {
			vendorInfo = fmt::format("{} {}", dialogueTargetRef->GetName(), customCurrency->GetName());
		}
		else {
			vendorInfo = fmt::format("Vendor's {}", customCurrency->GetName());
		}
		var.SetText(vendorInfo.c_str());

		auto& merchantEntries = *RE::TESDataHandler::GetSingleton()->merchantInventory->entryList;
		uint32_t merchantGold = 0;
		for (auto* entry : merchantEntries) {
			//Yoda
			if (entry->object == customCurrency) {
				merchantGold = entry->countDelta;
				break;
			}
		}

		auto inventoryCounts = player->GetInventoryCounts();
		auto playerGold = inventoryCounts.contains(customCurrency) ? inventoryCounts.at(customCurrency) : 0;

		if (const auto* vendorActor = RE::TESForm::LookupByID<RE::Actor>(barterActorID); vendorActor) {
			const RE::GFxValue args[4]{ playerGold, merchantGold, vendorActor->GetName(), nullptr};
			var.Invoke("UpdatePlayerInfo", nullptr, args, 4);
		}
		else {
			const RE::GFxValue args[4]{ playerGold, merchantGold, "Vendor", nullptr };
			var.Invoke("UpdatePlayerInfo", nullptr, args, 4);
		}
		LOG_DEBUG("  >Alt currency, updated menu."sv);
	}

	RE::BSEventNotifyControl CurrencyManager::ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>*)
	{
		using Control = RE::BSEventNotifyControl;

		if (!customCurrency || 
			!a_event || 
			!a_event->opening ||
			a_event->menuName != RE::BarterMenu::MENU_NAME)
		{
			return Control::kContinue;
		}

		auto* ui = RE::UI::GetSingleton();
		auto* dh = RE::TESDataHandler::GetSingleton();
		auto* player = RE::PlayerCharacter::GetSingleton();
		assert(ui && dh && player);
		if (!ui || !dh || !player) {
			return Control::kContinue;
		}

		auto menu = ui->GetMovieView(RE::BarterMenu::MENU_NAME);
		if (!menu) {
			return Control::kContinue;
		}

		std::string currencyName = customCurrency->GetName();
		bool useSkyUIPaths = dh->LookupLoadedModByName("SkyUI_SE.esp"sv);
		RE::GFxValue var;
		if (useSkyUIPaths) {
			menu->GetVariable(&var, pathToPlayerLabel_SkyUI);
			if (var.IsUndefined()) {
				return Control::kContinue;
			}
			var.SetText(currencyName.c_str());

			menu->GetVariable(&var, pathToVendorLabel_SkyUI);
			if (var.IsUndefined()) {
				return Control::kContinue;
			}
		}
		else {
			menu->GetVariable(&var, pathToPlayerLabel);
			if (var.IsUndefined()) {
				return Control::kContinue;
			}
			var.SetText(currencyName.c_str());

			menu->GetVariable(&var, pathToVendorLabel);
			if (var.IsUndefined()) {
				return Control::kContinue;
			}
		}

		auto* dialogueTargetRef = RE::TESForm::LookupByID<RE::Actor>(barterActorID);
		if (dialogueTargetRef && dialogueTargetRef->Is3DLoaded()) {
			currencyName = fmt::format("{} {}", dialogueTargetRef->GetName(), currencyName);
		}
		else {
			currencyName = fmt::format("Vendor {}", currencyName);
		}
		var.SetText(currencyName.c_str());

		auto trainingMenu = ui->GetMenu<RE::BarterMenu>();
		if (!trainingMenu) {
			return Control::kContinue;
		}

		RE::UpdateBottomBar(trainingMenu.get()); //Note that this triggers UpdatePlayerInfo. Do not call it there.
		return Control::kContinue;
	}
}