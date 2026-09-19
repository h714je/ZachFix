#pragma once

// Static-RE-derived player-car cadence repair.
// The Xbox 360 original gates the post-timer game update to a discrete 30 Hz
// cadence before the CObjectCar phase-5 vehicle dispatcher runs. Director's Cut
// calls the homologous dispatcher at render cadence. This hook restores only
// that live player-car scheduling contract while leaving rendering and other
// CObjectCar phases untouched.
bool InstallVehicleXboxCadenceFix();
