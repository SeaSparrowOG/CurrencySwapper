#include "Serde.h"

#include "CurrencyManager/CurrencyManager.h"

namespace Serialization {
	void SaveCallback(SKSE::SerializationInterface* a_intfc)
	{
		CurrencyManager::Save(a_intfc);
	}

	void LoadCallback(SKSE::SerializationInterface* a_intfc)
	{
		CurrencyManager::Load(a_intfc);
	}

	void RevertCallback(SKSE::SerializationInterface* a_intfc)
	{
		CurrencyManager::Revert(a_intfc);
	}
}