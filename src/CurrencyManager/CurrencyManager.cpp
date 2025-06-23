#include "CurrencyManager.h"

#include "RE/Misc.h"
#include "Settings/INI/INISettings.h"

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

	bool ProcessTrainingDeal(RE::Actor* a_player, int32_t a_value)
	{
		auto* manager = CurrencyManager::GetSingleton();
		if (manager) {
			return manager->ProcessTrainingDeal(a_player, a_value);
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

	void SendCustomTrainingMenuEvent(RE::TrainingMenu* a_menu)
	{
		if (!a_menu) {
			return;
		}
		auto* manager = CurrencyManager::GetSingleton();
		if (manager) {
			manager->SendCustomTrainingMenuEvent(a_menu);
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

	bool RequestTrainingCost(float a_SkillLevel, float& a_out)
	{
		auto* manager = CurrencyManager::GetSingleton();
		if (manager) {
			return manager->RequestTrainingCost(a_SkillLevel, a_out);
		}
		return false;
	}

	void ReloadIniSettings() {
		auto* manager = CurrencyManager::GetSingleton();
		if (manager) {
			manager->ReloadINISettings();
		}
	}

	void RegisterFormForEvents(RE::TESForm* a_form)
	{
		auto* manager = CurrencyManager::GetSingleton();
		if (manager) {
			manager->RegisterFormForAllEvents(a_form);
		}
	}

	void ResetVendorInfo(RE::GFxValue* a_updateObj)
	{
		auto* manager = CurrencyManager::GetSingleton();
		if (manager) {
			manager->ResetVendorInfo(a_updateObj);
		}
	}

	void SetTrainingOverrides(bool a_overrideMult, float a_multiplier, bool a_overrideBase, float a_base) {
		auto* manager = CurrencyManager::GetSingleton();
		if (manager) {
			manager->SetTrainingOverrides(a_overrideMult, a_multiplier, a_overrideBase, a_base);
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
		logger::info("Saving currency data..."sv);
		if (!a_intfc->OpenRecord(StoredCurrency, Version)) {
			logger::error("  >Failed to open record for currency data."sv);
			return false;
		}

		RE::FormID currencyID = customCurrency ? customCurrency->GetFormID() : 0;
		if (!a_intfc->WriteRecordData(currencyID)) {
			logger::error("  >Failed to write currency ID: {}"sv, currencyID);
			return false;
		}

		if (!a_intfc->WriteRecordData(overrideTrainingCostBase)) {
			logger::error("  >Failed to write overrideTrainingCostBase."sv);
			return false;
		}
		if (!a_intfc->WriteRecordData(trainingCostBaseOverride)) {
			logger::error("  >Failed to write trainingCostBaseOverride."sv);
			return false;
		}
		if (!a_intfc->WriteRecordData(overrideTrainingCostMultiplier)) {
			logger::error("  >Failed to write overrideTrainingCostMultiplier."sv);
			return false;
		}
		if (!a_intfc->WriteRecordData(trainingCostMultiplierOverride)) {
			logger::error("  >Failed to write trainingCostMultiplierOverride."sv);
			return false;
		}
		return true;
	}

	bool CurrencyManager::Load(SKSE::SerializationInterface* a_intfc) {
		std::uint32_t type;
		std::uint32_t version;
		std::uint32_t length;
		while (a_intfc->GetNextRecordInfo(type, version, length)) {
			if (type != StoredCurrency) {
				continue;
			}

			logger::info("Loading currency data..."sv);
			if (version == 1) {
				logger::info("  >Legacy 1"sv);
				RE::FormID oldID;
				if (!a_intfc->ReadRecordData(oldID)) {
					logger::error("  >Failed to read FormID"sv);
					return false;
				}
				RE::FormID newID;
				if (!a_intfc->ResolveFormID(oldID, newID)) {
					logger::error("  >Failed to resolve FormID ({:08X})"sv, oldID);
					return false;
				}
				customCurrency = RE::TESForm::LookupByID<RE::TESBoundObject>(newID);
			}
			else if (version == 2) {
				RE::FormID oldID;
				if (!a_intfc->ReadRecordData(oldID)) {
					logger::error("  >Failed to read FormID"sv);
					return false;
				}
				RE::FormID newID;
				if (!a_intfc->ResolveFormID(oldID, newID)) {
					logger::error("  >Failed to resolve FormID ({:08X})"sv, oldID);
					return false;
				}
				customCurrency = RE::TESForm::LookupByID<RE::TESBoundObject>(newID);

				if (!a_intfc->ReadRecordData(overrideTrainingCostBase)) {
					logger::error("  >Failed to read overrideTrainingCostBase."sv);
					return false;
				}
				if (!a_intfc->ReadRecordData(trainingCostBaseOverride)) {
					logger::error("  >Failed to read trainingCostBaseOverride."sv);
					return false;
				}
				if (!a_intfc->ReadRecordData(overrideTrainingCostMultiplier)) {
					logger::error("  >Failed to read overrideTrainingCostMultiplier."sv);
					return false;
				}
				if (!a_intfc->ReadRecordData(trainingCostMultiplierOverride)) {
					logger::error("  >Failed to read trainingCostMultiplierOverride."sv);
					return false;
				}
			}
			else {
				logger::error("Unsupported version {} for type {}"sv, version, type);
				return false;
			}
			return true;
		}
		return true;
	}

	void CurrencyManager::Revert(SKSE::SerializationInterface* a_intfc) {
		(void)a_intfc;
		customCurrency = nullptr;
		overrideTrainingCostBase = false;
		overrideTrainingCostMultiplier = false;
		trainingCostBaseOverride = 0.0f;
		trainingCostMultiplierOverride = 0.0f;
	}


	bool CurrencyManager::Initialize() {
		auto* sourceEventHolder = RE::UI::GetSingleton();
		if (!sourceEventHolder) {
			logger::critical("  >Failed to get the game's source event holder."sv);
			return false;
		}

		auto* settingsHolder = Settings::INI::Holder::GetSingleton();
		if (!settingsHolder) {
			logger::critical("  >Failed to get the settings holder."sv);
			return false;
		}

		auto yLabelOffsetRaw = settingsHolder->GetStoredSetting<float>(Settings::INI::TRAINING_MENU_LABEL_OFFSET_Y);
		trainingLabelOffsety = yLabelOffsetRaw.has_value() ? yLabelOffsetRaw.value() : -18.0f;
		if (!yLabelOffsetRaw.has_value()) {
			logger::warn("  >Failed to retrieve training label offset from settings, using default value: {}"sv, trainingLabelOffsety);
		}

		auto vendorLabelOverrideRaw = settingsHolder->GetStoredSetting<std::string>(Settings::INI::BARTER_MENU_VENDOR_LABEL_OVERWRITE);
		auto playerLabelOverrideRaw = settingsHolder->GetStoredSetting<std::string>(Settings::INI::BARTER_MENU_PLAYER_LABEL_OVERWRITE);

		playerLabelOverride = playerLabelOverrideRaw.has_value() ? playerLabelOverrideRaw.value() : "DEFAULT";
		vendorLabelOverride = vendorLabelOverrideRaw.has_value() ? vendorLabelOverrideRaw.value() : "DEFAULT";
		if (playerLabelOverride == "DEFAULT" || vendorLabelOverride == "DEFAULT") {
			playerLabelOverride = "";
			vendorLabelOverride = "";
		}
		else {
			logger::info("  >Overrode default paths with:"sv);
			logger::info("    Player Label: {}"sv, playerLabelOverride);
			logger::info("    Vendor Label: {}"sv, vendorLabelOverride);
		}
		sourceEventHolder->AddEventSink(this);
		return true;
	}

	void CurrencyManager::RevertCurrency() {
		auto* oldCurrency = customCurrency ? skyrim_cast<RE::TESForm*>(customCurrency) : nullptr;
		customCurrency = nullptr;
		customRevert.QueueEvent(oldCurrency);

		overrideTrainingCostBase = false;
		overrideTrainingCostMultiplier = false;
		trainingCostBaseOverride = 0.0f;
		trainingCostMultiplierOverride = 0.0f;
	}

	bool CurrencyManager::SetCurrency(RE::TESForm* a_newCurrency) {
		auto* bound = a_newCurrency ? skyrim_cast<RE::TESBoundObject*>(a_newCurrency) : nullptr;
		if (!bound) {
			logger::warn("Failed to set new currency. Provided form: {}"sv, a_newCurrency ? a_newCurrency->GetName() : "NULL");
			return false;
		}

		overrideTrainingCostBase = false;
		overrideTrainingCostMultiplier = false;
		trainingCostBaseOverride = 0.0f;
		trainingCostMultiplierOverride = 0.0f;

		auto* old = customCurrency ? skyrim_cast<RE::TESForm*>(customCurrency) : nullptr;
		customCurrency = bound;
		currencySwap.QueueEvent(old, customCurrency);
		return true;
	}

	void CurrencyManager::SetTrainingOverrides(bool a_overrideMult, float a_multiplier, bool a_overrideBase, float a_base)
	{
		if (a_overrideMult) {
			overrideTrainingCostMultiplier = true;
			trainingCostMultiplierOverride = a_multiplier;
		}
		else {
			overrideTrainingCostMultiplier = false;
			trainingCostMultiplierOverride = 0.0f;
		}

		if (a_overrideBase) {
			overrideTrainingCostBase = true;
			trainingCostBaseOverride = a_base;
		}
		else {
			overrideTrainingCostBase = false;
			trainingCostBaseOverride = 0.0f;
		}
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

	void CurrencyManager::ResetVendorInfo(RE::GFxValue* a_updateObj)
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
			const RE::GFxValue args[4]{ playerGold, merchantGold, vendorActor->GetName(), *a_updateObj };
			var.Invoke("UpdatePlayerInfo", nullptr, args, 4);
		}
		else {
			const RE::GFxValue args[4]{ playerGold, merchantGold, "Vendor", *a_updateObj };
			var.Invoke("UpdatePlayerInfo", nullptr, args, 4);
		}
		if (useSkyUIPaths) {
			UpdateSkyUIText(a_updateObj);
		}
		else {
			UpdateVanillaText(a_updateObj);
		}
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

		if (!playerLabelOverride.empty() && !vendorLabelOverride.empty()) {
			menu->GetVariable(&var, playerLabelOverride.c_str());
			if (var.IsUndefined()) {
				return Control::kContinue;
			}
			var.SetText(currencyName.c_str());

			menu->GetVariable(&var, vendorLabelOverride.c_str());
			if (var.IsUndefined()) {
				return Control::kContinue;
			}
		}
		else if (useSkyUIPaths) {
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

	void CurrencyManager::UpdateSkyUIText(RE::GFxValue* a_updateObj) {
		(void)a_updateObj;
		auto* ui = RE::UI::GetSingleton();
		auto barterMenu = ui ? ui->GetMenu<RE::BarterMenu>() : nullptr;
		if (!barterMenu) {
			LOG_DEBUG("  >UpdateSkyUIText: Failed to get BarterMenu."sv);
			return;
		}

		auto& root = barterMenu->root;
		if (root.IsUndefined() || root.IsNull()) {
			LOG_DEBUG("  >UpdateSkyUIText: Failed to get root."sv);
			return;
		}

		RE::GFxValue bottomBar;
		if (!root.GetMember("bottomBar", &bottomBar)) {
			LOG_DEBUG("  >UpdateSkyUIText: Failed to get bottomBar."sv);
			return;
		}
		if (bottomBar.IsUndefined() || bottomBar.IsNull()) {
			LOG_DEBUG("  >UpdateSkyUIText: bottomBar is undefined or null."sv);
			return;
		}

		// playerInfoCard
		RE::GFxValue playerInfoCard;
		if (!bottomBar.GetMember("playerInfoCard", &playerInfoCard)) {
			LOG_DEBUG("  >UpdateSkyUIText: Failed to get playerInfoCard."sv);
			return;
		}
		if (playerInfoCard.IsUndefined() || playerInfoCard.IsNull()) {
			LOG_DEBUG("  >UpdateSkyUIText: playerInfoCard is undefined or null."sv);
			return;
		}

		// PlayerGoldValue
		RE::GFxValue playerGoldValue;
		if (!playerInfoCard.GetMember("PlayerGoldValue", &playerGoldValue)) {
			LOG_DEBUG("  >UpdateSkyUIText: Failed to get PlayerGoldValue."sv);
			return;
		}
		if (playerGoldValue.IsUndefined() || playerGoldValue.IsNull()) {
			LOG_DEBUG("  >UpdateSkyUIText: PlayerGoldValue is undefined or null."sv);
			return;
		}
		RE::GFxValue playerGoldValueX;
		if (!playerGoldValue.GetMember("_x", &playerGoldValueX)) {
			LOG_DEBUG("  >UpdateSkyUIText: Failed to get PlayerGoldValue._x."sv);
			return;
		}
		if (playerGoldValueX.IsUndefined() || playerGoldValueX.IsNull()) {
			LOG_DEBUG("  >UpdateSkyUIText: PlayerGoldValue._x is undefined or null."sv);
			return;
		}

		// PlayerGoldLabel
		RE::GFxValue playerGoldLabel;
		if (!playerInfoCard.GetMember("PlayerGoldLabel", &playerGoldLabel)) {
			LOG_DEBUG("  >UpdateSkyUIText: Failed to get PlayerGoldLabel."sv);
			return;
		}
		if (playerGoldLabel.IsUndefined() || playerGoldLabel.IsNull()) {
			LOG_DEBUG("  >UpdateSkyUIText: PlayerGoldLabel is undefined or null."sv);
			return;
		}
		RE::GFxValue playerGoldLabelX;
		if (!playerGoldLabel.GetMember("_x", &playerGoldLabelX)) {
			LOG_DEBUG("  >UpdateSkyUIText: Failed to get PlayerGoldLabel._x."sv);
			return;
		}
		if (playerGoldLabelX.IsUndefined() || playerGoldLabelX.IsNull()) {
			LOG_DEBUG("  >UpdateSkyUIText: PlayerGoldLabel._x is undefined or null."sv);
			return;
		}

		// CarryWeightValue
		RE::GFxValue carryWeightValue;
		if (!playerInfoCard.GetMember("CarryWeightValue", &carryWeightValue)) {
			LOG_DEBUG("  >UpdateSkyUIText: Failed to get CarryWeightValue."sv);
			return;
		}
		if (carryWeightValue.IsUndefined() || carryWeightValue.IsNull()) {
			LOG_DEBUG("  >UpdateSkyUIText: CarryWeightValue is undefined or null."sv);
			return;
		}
		RE::GFxValue carryWeightValueX;
		if (!carryWeightValue.GetMember("_x", &carryWeightValueX)) {
			LOG_DEBUG("  >UpdateSkyUIText: Failed to get CarryWeightValue._x."sv);
			return;
		}

		// CarryWeightLabel
		RE::GFxValue carryWeightLabel;
		if (!playerInfoCard.GetMember("CarryWeightLabel", &carryWeightLabel)) {
			LOG_DEBUG("  >UpdateSkyUIText: Failed to get CarryWeightLabel."sv);
			return;
		}
		if (carryWeightLabel.IsUndefined() || carryWeightLabel.IsNull()) {
			LOG_DEBUG("  >UpdateSkyUIText: CarryWeightLabel is undefined or null."sv);
			return;
		}
		RE::GFxValue carryWeightLabelX;
		if (!carryWeightLabel.GetMember("_x", &carryWeightLabelX)) {
			LOG_DEBUG("  >UpdateSkyUIText: Failed to get CarryWeightLabel._x."sv);
			return;
		}
		if (carryWeightLabelX.IsUndefined() || carryWeightLabelX.IsNull()) {
			LOG_DEBUG("  >UpdateSkyUIText: CarryWeightLabel._x is undefined or null."sv);
			return;
		}

		// VendorGoldValue
		RE::GFxValue vendorGoldValue;
		if (!playerInfoCard.GetMember("VendorGoldValue", &vendorGoldValue)) {
			LOG_DEBUG("  >UpdateSkyUIText: Failed to get VendorGoldValue."sv);
			return;
		}
		if (vendorGoldValue.IsUndefined() || vendorGoldValue.IsNull()) {
			LOG_DEBUG("  >UpdateSkyUIText: VendorGoldValue is undefined or null."sv);
			return;
		}
		RE::GFxValue vendorGoldValueWidth;
		if (!vendorGoldValue.GetMember("_width", &vendorGoldValueWidth)) {
			LOG_DEBUG("  >UpdateSkyUIText: Failed to get VendorGoldValue._x."sv);
			return;
		}
		if (vendorGoldValueWidth.IsUndefined() || vendorGoldValueWidth.IsNull()) {
			LOG_DEBUG("  >UpdateSkyUIText: VendorGoldValue._x is undefined or null."sv);
			return;
		}

		double playerGoldValueXNum = playerGoldValueX.GetNumber();
		double playerGoldLabelXNum = playerGoldLabelX.GetNumber();
		double vendorGoldLabelWidthNum = vendorGoldValueWidth.GetNumber();
		double carryWeightValueXNum = carryWeightValueX.GetNumber();
		double carryWeightLabelXNum = carryWeightLabelX.GetNumber();

		playerGoldValueXNum -= vendorGoldLabelWidthNum / 2.0 - 10.0;
		playerGoldLabelXNum -= vendorGoldLabelWidthNum / 2.0 - 10.0;
		carryWeightValueXNum -= vendorGoldLabelWidthNum / 2.0 - 10.0;
		carryWeightLabelXNum -= vendorGoldLabelWidthNum / 2.0 - 10.0;
		playerGoldLabelX.SetNumber(playerGoldLabelXNum);
		playerGoldValueX.SetNumber(playerGoldValueXNum);
		carryWeightValueX.SetNumber(carryWeightValueXNum);
		carryWeightLabelX.SetNumber(carryWeightLabelXNum);


		if (!playerGoldLabel.SetMember("_x", playerGoldLabelX)) {
			LOG_DEBUG("  >UpdateSkyUIText: Failed to set PlayerGoldLabel._x."sv);
			return;
		}
		if (!playerGoldValue.SetMember("_x", playerGoldValueX)) {
			LOG_DEBUG("  >UpdateSkyUIText: Failed to set PlayerGoldValue._x."sv);
			return;
		}
		if (!playerInfoCard.SetMember("PlayerGoldLabel", playerGoldLabel)) {
			LOG_DEBUG("  >UpdateSkyUIText: Failed to set PlayerGoldLabel in playerInfoCard."sv);
			return;
		}
		if (!playerInfoCard.SetMember("PlayerGoldValue", playerGoldValue)) {
			LOG_DEBUG("  >UpdateSkyUIText: Failed to set PlayerGoldValue in playerInfoCard."sv);
			return;
		}
		if (!carryWeightValue.SetMember("_x", carryWeightValueX)) {
			LOG_DEBUG("  >UpdateSkyUIText: Failed to set CarryWeightValue._x."sv);
			return;
		}
		if (!playerInfoCard.SetMember("CarryWeightValue", carryWeightValue)) {
			LOG_DEBUG("  >UpdateSkyUIText: Failed to set CarryWeightValue in playerInfoCard."sv);
			return;
		}
		if (!carryWeightLabel.SetMember("_x", carryWeightLabelX)) {
			LOG_DEBUG("  >UpdateSkyUIText: Failed to set CarryWeightLabel._x."sv);
			return;
		}
		if (!playerInfoCard.SetMember("CarryWeightLabel", carryWeightLabel)) {
			LOG_DEBUG("  >UpdateSkyUIText: Failed to set CarryWeightLabel in playerInfoCard."sv);
			return;
		}
		
		LOG_DEBUG("  >UpdateSkyUIText: Updated PlayerGoldLabel and PlayerGoldValue X positions by {}."sv, vendorGoldLabelWidthNum);
	}

	void CurrencyManager::UpdateVanillaText(RE::GFxValue* a_updateObj)
	{
		(void)a_updateObj;
		// TODO: Implement Vanilla text updates if needed.
	}

	void CurrencyManager::SendCustomTrainingMenuEvent(RE::TrainingMenu* a_menu)
	{
		if (!customCurrency) {
			return;
		}

		auto& trainingMenuObj = a_menu->trainingMenuObj;
		if (trainingMenuObj.IsUndefined() || trainingMenuObj.IsNull()) {
			return;
		}

		RE::GFxValue trainingCard;
		if (!trainingMenuObj.GetMember("TrainingCard", &trainingCard)) {
			return;
		}

		bool managedReplacement = false;
		int i = 6;
		trainingCard.VisitMembers([&](const char* a_name, RE::GFxValue a_value) {
			if (i == 0) {
				a_value.SetMember("textAutoSize", { "shrink" });
				RE::GFxValue verticalHeight;
				if (a_value.GetMember("_y", &verticalHeight)) {
					const auto height = verticalHeight.GetNumber();
					const auto diff = height - trainingLabelOffsety;
					a_value.SetMember("_y", { diff });
				}
				a_value.SetText(customCurrency->GetName());
				trainingCard.SetMember(a_name, a_value);
				managedReplacement = true;
			}
			--i;
			});
		if (!managedReplacement) {
			return;
		}

		auto* trainingActor = a_menu->trainer;
		if (!trainingActor) {
			return;
		}
		trainerActorID = trainingActor->formID;

		auto& currentGold = a_menu->currentGold;
		const auto currencyCount = RE::PlayerCharacter::GetSingleton()->GetItemCount(customCurrency);
		const auto currencyCountStr = std::to_string(currencyCount);
		currentGold.SetText(currencyCountStr.c_str());
	}

	bool CurrencyManager::ProcessTrainingDeal(RE::Actor* a_player, int32_t a_value)
	{
		if (!customCurrency) {
			return false;
		}
		auto* trainer = RE::TESForm::LookupByID<RE::Actor>(trainerActorID);
		if (!trainer || !a_player) {
			return false;
		}
		a_player->RemoveItem(customCurrency, a_value, RE::ITEM_REMOVE_REASON::kRemove, nullptr, nullptr);
		trainer->AddObjectToContainer(customCurrency, nullptr, a_value, nullptr);
		return true;
	}

	bool CurrencyManager::RequestTrainingCost(float a_SkillLevel, float& a_out)
	{
		if (!customCurrency) {
			return false;
		}
		if (!overrideTrainingCostBase && !overrideTrainingCostMultiplier) {
			return false;
		}

		auto* gameSettings = RE::GameSettingCollection::GetSingleton();
		if (!gameSettings) {
			return false;
		}
		auto* baseCostMult = gameSettings->GetSetting("fTrainingBaseCost");
		auto* multiplier = gameSettings->GetSetting("fTrainingMultCost");
		if (!baseCostMult || !multiplier) {
			return false;
		}

		auto* journeymanCost = gameSettings->GetSetting("iTrainingJourneymanCost");
		auto* expertCost = gameSettings->GetSetting("iTrainingExpertCost");
		auto* masterCost = gameSettings->GetSetting("iTrainingMasterCost");
		auto* journeymanSkill = gameSettings->GetSetting("iTrainingJourneymanSkill");
		auto* expertSkill = gameSettings->GetSetting("iTrainingExpertSkill");
		auto* masterSkill = gameSettings->GetSetting("iTrainingMasterSkill");
		if (!journeymanCost || !expertCost || !masterCost || !journeymanSkill || !expertSkill || !masterSkill) {
			return false;
		}
		LOG_DEBUG("  >Requesting training cost for skill level: {}"sv, a_SkillLevel);
		LOG_DEBUG("    >Journeyman Cost: {}, Journeyman Skill: {}"sv,
			journeymanCost->GetSInt(), journeymanSkill->GetSInt());
		LOG_DEBUG("    >Expert Cost: {}, Expert Skill: {}"sv,
			expertCost->GetSInt(), expertSkill->GetSInt());
		LOG_DEBUG("    >Master Cost: {}, Master Skill: {}"sv,
			masterCost->GetSInt(), masterSkill->GetSInt());

		float base = 0.0f;
		if (overrideTrainingCostBase) {
			base = trainingCostBaseOverride;
		}
		else {
			if (a_SkillLevel < journeymanSkill->GetSInt()) {
				base = static_cast<float>(journeymanCost->GetSInt());
			}
			else if (a_SkillLevel < expertSkill->GetSInt()) {
				base = static_cast<float>(expertCost->GetSInt());
			}
			else {
				base = static_cast<float>(masterCost->GetSInt());
			}
		}

		float multiplierValue = 0.0f;
		if (overrideTrainingCostMultiplier) {
			multiplierValue = trainingCostMultiplierOverride;
		}
		else {
			multiplierValue = multiplier->GetFloat();
		}
		LOG_DEBUG("  >Training cost base: {}, multiplier: {}"sv, base, multiplierValue);

		float result = base + (a_SkillLevel * multiplierValue);
		if (!std::isfinite(result)) {
			return false;
		}
		result = ceil(result);
		if (result < 0.0f) {
			result = 0.0f;
		}
		a_out = result;
		return true;
	}

	void CurrencyManager::ReloadINISettings() {
		auto* settingsHolder = Settings::INI::Holder::GetSingleton();
		if (!settingsHolder) {
			logger::warn("  >Failed to retrieve settings holder for INI reload."sv);
			return;
		}
		settingsHolder->Reload();
		auto yLabelOffsetRaw = settingsHolder->GetStoredSetting<float>(Settings::INI::TRAINING_MENU_LABEL_OFFSET_Y);
		if (yLabelOffsetRaw.has_value()) {
			trainingLabelOffsety = yLabelOffsetRaw.value();
			logger::info("  >Training label offset updated to: {}"sv, trainingLabelOffsety);
		}
		else {
			logger::warn("  >Failed to retrieve training label offset from settings, using default value: {}"sv, trainingLabelOffsety);
		}

		auto vendorLabelOverrideRaw = settingsHolder->GetStoredSetting<std::string>(Settings::INI::BARTER_MENU_VENDOR_LABEL_OVERWRITE);
		auto playerLabelOverrideRaw = settingsHolder->GetStoredSetting<std::string>(Settings::INI::BARTER_MENU_PLAYER_LABEL_OVERWRITE);
		playerLabelOverride = playerLabelOverrideRaw.has_value() ? playerLabelOverrideRaw.value() : "DEFAULT";
		vendorLabelOverride = vendorLabelOverrideRaw.has_value() ? vendorLabelOverrideRaw.value() : "DEFAULT";
		if (playerLabelOverride == "DEFAULT" || vendorLabelOverride == "DEFAULT") {
			playerLabelOverride = "";
			vendorLabelOverride = "";
		}
		else {
			logger::info("  >Overrode default paths with:"sv);
			logger::info("    Player Label: {}"sv, playerLabelOverride);
			logger::info("    Vendor Label: {}"sv, vendorLabelOverride);
		}
	}
}