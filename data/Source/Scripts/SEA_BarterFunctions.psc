Scriptname SEA_BarterFunctions

Function ResetCurrency() Global Native
Function RegisterFormForAllEvents(Form a_kForm) Global Native
Bool Function SetCurrency(Form a_kNewCurrency) Global Native
Bool Function SetCurrencyConsole(String a_sNewCurrencyEditorID) Global Native
Form Function GetCurrency() Global Native

;/
Events
/;
Event OnCustomPurchase(Form a_kCurrency)

EndEvent

Event OnCustomSale(Form a_kCurrency)

EndEvent

Event OnCustomCurrencyFail(Form a_kCurrency)

EndEvent

Event OnCurrencySwap(Form a_kOldCurrency, Form a_kNewCurrency)

EndEvent

Event OnCurrencyRevert(Form a_kOldCurrency)

EndEvent

Event OnCustomBarterMenu(Actor a_kSeller)

EndEvent