## Currency Swapper
SKSE Framework for dynamically swapping the currency vendors trade in. Very useful for mods such as C.O.I.N. that add multiple currencies.

## Usage
### Papyrus
Currently, the framework relies on Papyrus to swap the trading currency. In order to swap anything, you need to import SEA_BarterFunctions and then simply use the SetCurrency function, passing in the form you want to use as currency. I recommend storing the previous currency somewhere in your script (retrieve it using GetCurrency()) in order to not overwrite changes other authors might make.
In order to use the new functions in your scripts, first import the script "SEA_BarterFunctions" in your script. This is done like this in papyrus:
```
Import SEA_BarterFunctions
```
After that, you can call functions as needed. Short brief:
**Functions:**
- `SetCurrency`
SetCurrency sets the currency of the game to the specified form. The form itself is stored in your SKSE cosave, so you do not need to change it every game load.
- `GetCurrency`
Returns the current currency of the game. Note that if the game is using the default "Gold" currency, this returns NONE. I recommend calling this before changing currencies to know what other currency to swap to (or revert in case of NONE).
- `ResetCurrency`
Resets the currency to Gold.
- `SetCurrencyConsole`
While you can use this to specify a form by EditorID (requires PO3's Tweaks), this is mostly used by the Custom Console integration.
- `RegisterFormForAllEvents`
Registers a given form (usually a quest) for all of the mod's events. The registration is NOT saved in your SKSE cosave, so you have to re-register your forms every game load.

**Events:**
- `OnCustomPurchase(Form a_kCurrency)`
Fires when a purchase is completed while there is a custom currency in place.
- `OnCustomSale(Form a_kCurrency)`
Fires when the player sells an item while a custom currency is in place.
- `OnCustomCurrencyFail(Form a_kCurrency)`
Fires when a purchase fails because the player doesn't have enough of the custom currency.
- `OnCurrencySwap(Form a_kCurrency)`
Fires when the currency swaps to a custom currency.
- `OnCurrencyRevert(Form a_kOldCurrency)`
Fires when the currency is reverted.
### Custom Console
Custom Console is a very useful SKSE plugin for mod authors to quickly test things. If you are using Custom Console, you get access to two new functions that should make your life a lot easier.
- `crs sc [EDID]`
Sets the currency to the specified form, using the EditorID. Example: `crs sc IronSword` will set the currency to Iron Swords. Requires PO3's Tweaks to work.
- `crs rc`
Resets the currency to Gold.
## Building
### Requirements:
- CMake
- VCPKG
- Visual Studio (with desktop C++ development)
---
### Instructions:
```
git clone https://github.com/SeaSparrowOG/CurrencySwapper
cd CurrencySwapper
git submodule innit
git submodule update --recursive
cmake --preset vs2022-windows-vcpkg 
cmake --build Release --config Release
```
---
### Automatic deployment to MO2:
You can automatically deploy to MO2's mods folder by defining an [Environment Variable](https://learn.microsoft.com/en-us/powershell/module/microsoft.powershell.core/about/about_environment_variables?view=powershell-7.4) named SKYRIM_MODS_FOLDER and pointing it to your MO2 mods folder. It will create a new mod with the appropriate name. After that, simply refresh MO2 and enable the mod. Image:
![Example of Environment Variable setup](https://cdn.discordapp.com/attachments/625292279468523522/1204193482600615936/Screenshot_61.png?ex=65d3d793&is=65c16293&hm=ed710c138bc02ead7ca11d85963c164feb1ea39e501ca46ffb1bac8609008473&)
