#pragma once

// Research-only CObjectCar -> PhysX 2.8.1 boundary + turning-response audit.
// It observes the live player-car NxActor/NxWheelShape state, including wheel
// steering, chassis velocities and wheel contacts, without changing game or
// PhysX state.  The earlier tire-timing experiment remains deliberately off.
bool InstallVehicleTimingDiag();
