## Currency Swapper
SKSE Framework for dynamically swapping the currency vendors trade in. Very useful for mods such as C.O.I.N. that add multiple currencies.

## Usage
### Papyrus
Currently, the framework relies on Papyrus to swap the trading currency. In order to swap anything, you need to import SEA_BarterFunctions and then simply use the SetCurrency function, passing in the form you want to use as currency. I recommend storing the previous currency somewhere in your script (retrieve it using GetCurrency()) in order to not overwrite changes other authors might make.
In the future you will be able to introduce keywords and formlists for bartering, but that comes much later.
_
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
