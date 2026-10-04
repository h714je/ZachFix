#pragma once

// Native ZachFix Settings bridge for the 0.3.0 development line.
// The visible entry is an externally tracked pseudo-row inside stock COption
// page 2, between retail row 8 and row 9 Exit. COption's real +0x1FC selector
// always remains inside its stock 0..9 domain. Confirm opens a manager-owned
// selector-0 CRdObject child; native Back/Cancel closes it through the retail
// deferred-removal lifecycle. There is no global hotkey/F9 opening path.
bool InstallNativeSettingsUiBridge();
bool IsNativeSettingsUiAvailable();
