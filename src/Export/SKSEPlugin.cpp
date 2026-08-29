#include "CurrencyManager/CurrencyManager.h"
#include "Hooks/Hooks.h"
#include "Papyrus/Papyrus.h"
#include "Serialization/Serde.h"
#include "Settings/INI/INISettings.h"

#ifdef SKYRIM_AE
extern "C" DLLEXPORT constinit auto SKSEPlugin_Version = []()
	{
		SKSE::PluginVersionData v{};

		v.PluginVersion(Plugin::VERSION);
		v.PluginName(Plugin::NAME);
		v.AuthorName("SeaSparrow"sv);
		v.UsesAddressLibrary();
		v.UsesUpdatedStructs();

		return v;
	}();
#endif

#ifndef NDEBUG
void SetupLog() {
	auto logsFolder = SKSE::log::log_directory();
	if (!logsFolder) REX::FAIL("SKSE log_directory not provided, logs disabled.");

	auto pluginName = Plugin::NAME;
	auto logFilePath = *logsFolder / std::format("{}.log", pluginName);
	auto fileLoggerPtr = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFilePath.string(), true);
	auto loggerPtr = std::make_shared<spdlog::logger>("log", std::move(fileLoggerPtr));

	spdlog::set_default_logger(std::move(loggerPtr));
	spdlog::set_level(spdlog::level::debug);
	spdlog::flush_on(spdlog::level::debug);

	//Pattern
	spdlog::set_pattern("%v");
}
#endif

SKSE_PLUGIN_QUERY(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
{
	a_info->infoVersion = SKSE::PluginInfo::kVersion;
	a_info->name = Plugin::NAME.data();
	a_info->version = Plugin::VERSION[0];

	if (a_skse->IsEditor()) {
		logger::CRITICAL("Loaded in editor, marking as incompatible"sv);
		return false;
	}

	const auto ver = a_skse->RuntimeVersion();
#ifdef SKYRIM_AE
	if (ver < SKSE::RUNTIME_SSE_1_6_1130) {
#else
	if (ver < SKSE::RUNTIME_1_5_39) {
#endif
		logger::CRITICAL("Unsupported runtime version {}", ver.string());
		return false;
	}

	return true;
}

SKSE_PLUGIN_LOAD(const SKSE::LoadInterface * a_skse)
{
#ifndef NDEBUG
	SetupLog();
	SKSE::Init(a_skse, false);
#else
	constexpr size_t allocSize = 7u * 14u + 14u * 1u; + 2u * 14u + 8u * 14u;
	SKSE::InitInfo info;
	info.hook = true;
	info.log = true;
	info.logLevel = REX::ELogLevel::Trace;
	info.logName = Plugin::NAME.data();
	info.trampoline = true;
	info.trampolineSize = allocSize;

	SKSE::Init(a_skse, info);
#endif
	SECTION_SEPARATOR;
	logger::INFO("{} v{}"sv, Plugin::NAME, Plugin::VERSION.string());
	logger::INFO("Author: SeaSparrow"sv);
	SECTION_SEPARATOR;

#ifdef SKYRIM_AE
	const auto ver = a_skse->RuntimeVersion();
	if (ver < SKSE::RUNTIME_SSE_1_7_104) {
		return false;
	}
#endif

	logger::INFO("Performing startup tasks..."sv);

	if (!Settings::INI::Read()) {
		REX::FAIL("Failed to load INI settings. Check the log for details."sv);
	}
	if (!Hooks::Install()) {
		REX::FAIL("Failed to install hooks. Check the log for more information."sv);
	}
	if (!CurrencyManager::Initialize()) {
		REX::FAIL("Failed to install Currency Manager. Check the log for details."sv);
	}
	SKSE::GetPapyrusInterface()->Register(Papyrus::RegisterFunctions);

	SECTION_SEPARATOR;
	logger::INFO("Setting up serialization system..."sv);
	const auto serialization = SKSE::GetSerializationInterface();
	serialization->SetUniqueID(Serialization::ID);
	serialization->SetSaveCallback(&Serialization::SaveCallback);
	serialization->SetLoadCallback(&Serialization::LoadCallback);
	serialization->SetRevertCallback(&Serialization::RevertCallback);
	logger::INFO("  >Registered necessary functions."sv);
	SECTION_SEPARATOR;

	return true;
}