#include "vehicle_timing_diag.h"

#include "config.h"
#include "logging.h"
#include "main_exe.h"

#include <Windows.h>
#include <MinHook.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>

namespace
{
using PlayerWheelBoundaryFn = void (__thiscall*)(void* self);
using ActorGetNbShapesFn = unsigned int (__thiscall*)(void* self);
using ActorGetShapesFn = void** (__thiscall*)(void* self);
using ShapeGetTypeFn = int (__thiscall*)(void* self);
using SetWheelFloatFn = void (__thiscall*)(void* self, float value);
using GetWheelFloatFn = float (__thiscall*)(void* self);

// PhysX 2.8.1 NxTireFunctionDesc is polymorphic: one vptr followed by five
// NxReal values. The native x86 caller passes the full 0x18-byte object by
// value to NxWheelShape::set* TireForceFunction.
struct TireFunctionDescCompat
{
    uintptr_t vtable;
    float extremumSlip;
    float extremumValue;
    float asymptoteSlip;
    float asymptoteValue;
    float stiffnessFactor;
};
static_assert(sizeof(TireFunctionDescCompat) == 0x18, "PhysX 2.8.1 tire ABI mismatch");

struct Vec3Compat
{
    float x;
    float y;
    float z;
};
static_assert(sizeof(Vec3Compat) == 0x0C, "PhysX 2.8.1 NxVec3 ABI mismatch");

// NxMat33 stores the actor basis as three NxVec3 columns.  Wheel-shape docs
// define +Y as up and +Z as forward, so projecting the chassis velocity onto
// these columns gives lateral/up/forward velocity without relying on game-side
// transform fields.
struct Mat33Compat
{
    Vec3Compat col0;
    Vec3Compat col1;
    Vec3Compat col2;
};
static_assert(sizeof(Mat33Compat) == 0x24, "PhysX 2.8.1 NxMat33 ABI mismatch");

using ActorGetVec3Fn = Vec3Compat (__thiscall*)(void* self);
using ActorGetMat33Fn = Mat33Compat (__thiscall*)(void* self);

// PhysX 2.8.1 NxWheelContactData. NxMaterialIndex is 16-bit in this ABI;
// explicit padding preserves the following NxReal alignment.
struct WheelContactDataCompat
{
    Vec3Compat contactPoint;
    Vec3Compat contactNormal;
    Vec3Compat longitudalDirection;
    Vec3Compat lateralDirection;
    float contactForce;
    float longitudalSlip;
    float lateralSlip;
    float longitudalImpulse;
    float lateralImpulse;
    uint16_t otherShapeMaterialIndex;
    uint16_t padding;
    float contactPosition;
};
static_assert(sizeof(WheelContactDataCompat) == 0x4C, "PhysX 2.8.1 contact ABI mismatch");

using SetTireFunctionFn = void (__thiscall*)(void* self, TireFunctionDescCompat desc);
using GetWheelContactFn = void* (__thiscall*)(void* self, WheelContactDataCompat* dest);

constexpr unsigned int kMaxLoggedWindows = 20;
constexpr ULONGLONG kTurnWindowMs = 250;
constexpr unsigned int kMaxTurnWindows = kMaxLoggedWindows * (1000 / kTurnWindowMs);
constexpr size_t kMaxWheels = 8;
constexpr float kFloatEpsilon = 0.000001f;

PlayerWheelBoundaryFn g_originalPlayerWheelBoundary = nullptr;
SetWheelFloatFn g_originalSetMotorTorque = nullptr;
SetWheelFloatFn g_originalSetBrakeTorque = nullptr;
SetWheelFloatFn g_originalSetSteerAngle = nullptr;
SetTireFunctionFn g_originalSetLongTire = nullptr;
SetTireFunctionFn g_originalSetLatTire = nullptr;
GetWheelFloatFn g_originalGetAxleSpeed = nullptr;
GetWheelContactFn g_originalGetContact = nullptr;

// Read-only methods used by the turning audit.  These are not detoured.
GetWheelFloatFn g_getSteerAngle = nullptr;
GetWheelContactFn g_readContact = nullptr;
ActorGetMat33Fn g_actorGetGlobalOrientation = nullptr;
ActorGetVec3Fn g_actorGetLinearVelocity = nullptr;
ActorGetVec3Fn g_actorGetAngularVelocity = nullptr;
void* g_vehicleActor = nullptr;

thread_local unsigned int g_playerBoundaryDepth = 0;
thread_local void* g_boundaryCar = nullptr;

std::array<void*, kMaxWheels> g_wheelPtrs{};
std::array<unsigned int, kMaxWheels> g_wheelShapeIndices{};
size_t g_wheelCount = 0;
bool g_physxHooksInstalled = false;
bool g_physxHookInstallFailed = false;
uint32_t g_methodHookMask = 0;
bool g_motorBrakeNormalizeABEnabled = false;
uintptr_t g_frameDeltaAddress = 0;

struct ScalarStats
{
    unsigned long count = 0;
    double sum = 0.0;
    float min = 0.0f;
    float max = 0.0f;
    float last = 0.0f;

    void Add(float value)
    {
        if (!std::isfinite(value))
            return;
        if (count == 0)
        {
            min = value;
            max = value;
        }
        else
        {
            min = std::min(min, value);
            max = std::max(max, value);
        }
        last = value;
        sum += static_cast<double>(value);
        ++count;
    }

    double Average() const
    {
        return count != 0 ? sum / static_cast<double>(count) : 0.0;
    }

    void Reset()
    {
        count = 0;
        sum = 0.0;
        min = 0.0f;
        max = 0.0f;
        last = 0.0f;
    }
};

struct TireStats
{
    unsigned long calls = 0;
    unsigned long changed = 0;
    bool havePrevious = false;
    TireFunctionDescCompat previous{};
    TireFunctionDescCompat last{};

    static bool Different(const TireFunctionDescCompat& a, const TireFunctionDescCompat& b)
    {
        return std::fabs(a.extremumSlip - b.extremumSlip) > kFloatEpsilon ||
            std::fabs(a.extremumValue - b.extremumValue) > kFloatEpsilon ||
            std::fabs(a.asymptoteSlip - b.asymptoteSlip) > kFloatEpsilon ||
            std::fabs(a.asymptoteValue - b.asymptoteValue) > kFloatEpsilon ||
            std::fabs(a.stiffnessFactor - b.stiffnessFactor) > 0.01f;
    }

    void Add(const TireFunctionDescCompat& desc)
    {
        ++calls;
        if (havePrevious && Different(previous, desc))
            ++changed;
        previous = desc;
        last = desc;
        havePrevious = true;
    }

    void ResetWindow()
    {
        calls = 0;
        changed = 0;
        // Preserve previous across windows so a transition at the boundary is
        // still counted in the next one-second sample.
    }
};

struct ContactStats
{
    unsigned long calls = 0;
    unsigned long hits = 0;
    ScalarStats force;
    ScalarStats longitudalSlip;
    ScalarStats lateralSlip;
    ScalarStats longitudalImpulse;
    ScalarStats lateralImpulse;
    ScalarStats position;

    void Reset()
    {
        calls = 0;
        hits = 0;
        force.Reset();
        longitudalSlip.Reset();
        lateralSlip.Reset();
        longitudalImpulse.Reset();
        lateralImpulse.Reset();
        position.Reset();
    }
};

struct WheelStats
{
    ScalarStats motorTorque;
    ScalarStats motorTorquePassed;
    ScalarStats brakeTorque;
    ScalarStats brakeTorquePassed;
    ScalarStats steerAngle;
    ScalarStats axleSpeed;
    TireStats longitudalTire;
    TireStats lateralTire;
    ContactStats contact;

    void ResetWindow()
    {
        motorTorque.Reset();
        motorTorquePassed.Reset();
        brakeTorque.Reset();
        brakeTorquePassed.Reset();
        steerAngle.Reset();
        axleSpeed.Reset();
        longitudalTire.ResetWindow();
        lateralTire.ResetWindow();
        contact.Reset();
    }
};

std::array<WheelStats, kMaxWheels> g_wheelStats{};

struct TurningStats
{
    ScalarStats delta60;
    ScalarStats carSpeed;
    ScalarStats steerState;
    ScalarStats actualSteer;
    ScalarStats actorLinearSpeed;
    ScalarStats localLateralVelocity;
    ScalarStats localVerticalVelocity;
    ScalarStats localForwardVelocity;
    ScalarStats slipAngle;
    ScalarStats absSlipAngle;
    ScalarStats actorAngularX;
    ScalarStats actorYawRate;
    ScalarStats absActorYawRate;
    ScalarStats actorAngularZ;
    ScalarStats poseYawRate;
    ScalarStats absPoseYawRate;
    ScalarStats yawAcceleration;
    ScalarStats lateralAcceleration;
    ScalarStats yawGain;
    ScalarStats curvature;
    ScalarStats turnRadius;
    unsigned long samples = 0;

    void Reset()
    {
        delta60.Reset();
        carSpeed.Reset();
        steerState.Reset();
        actualSteer.Reset();
        actorLinearSpeed.Reset();
        localLateralVelocity.Reset();
        localVerticalVelocity.Reset();
        localForwardVelocity.Reset();
        slipAngle.Reset();
        absSlipAngle.Reset();
        actorAngularX.Reset();
        actorYawRate.Reset();
        absActorYawRate.Reset();
        actorAngularZ.Reset();
        poseYawRate.Reset();
        absPoseYawRate.Reset();
        yawAcceleration.Reset();
        lateralAcceleration.Reset();
        yawGain.Reset();
        curvature.Reset();
        turnRadius.Reset();
        samples = 0;
    }
};

TurningStats g_turningStats;
std::array<ContactStats, kMaxWheels> g_turnContactStats{};
ULONGLONG g_turnWindowStartMs = 0;
unsigned int g_loggedTurnWindows = 0;
LARGE_INTEGER g_qpcFrequency{};
LARGE_INTEGER g_previousTurnQpc{};
void* g_previousTurnCar = nullptr;
bool g_havePreviousTurnSample = false;
float g_previousRawYaw = 0.0f;
float g_previousLocalLateralVelocity = 0.0f;
float g_previousActorYawRate = 0.0f;

ScalarStats g_delta60Stats;
ScalarStats g_speedStats;
ScalarStats g_steerStateStats;
ScalarStats g_yawStats;
unsigned long g_boundaryCalls = 0;
ULONGLONG g_windowStartMs = 0;
unsigned int g_loggedWindows = 0;

float ReadFloat(const void* base, size_t offset)
{
    float value = 0.0f;
    if (base != nullptr)
        std::memcpy(&value, static_cast<const unsigned char*>(base) + offset, sizeof(value));
    return value;
}

void* ReadPointer(const void* base, size_t offset)
{
    void* value = nullptr;
    if (base != nullptr)
        std::memcpy(&value, static_cast<const unsigned char*>(base) + offset, sizeof(value));
    return value;
}

bool MatchesBytes(uintptr_t address, const unsigned char* bytes, size_t size)
{
    return address != 0 &&
        std::memcmp(reinterpret_cast<const void*>(address), bytes, size) == 0;
}

bool InPlayerBoundary()
{
    return g_playerBoundaryDepth != 0 && g_boundaryCar != nullptr;
}

int FindWheelIndex(const void* wheel)
{
    if (!InPlayerBoundary() || wheel == nullptr)
        return -1;
    for (size_t i = 0; i < g_wheelCount; ++i)
    {
        if (g_wheelPtrs[i] == wheel)
            return static_cast<int>(i);
    }
    return -1;
}

float Dot(const Vec3Compat& a, const Vec3Compat& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

void RecordContact(ContactStats& stats, void* contactShape, const WheelContactDataCompat& data)
{
    ++stats.calls;
    if (contactShape == nullptr)
        return;

    ++stats.hits;
    stats.force.Add(data.contactForce);
    stats.longitudalSlip.Add(data.longitudalSlip);
    stats.lateralSlip.Add(data.lateralSlip);
    stats.longitudalImpulse.Add(data.longitudalImpulse);
    stats.lateralImpulse.Add(data.lateralImpulse);
    stats.position.Add(data.contactPosition);
}

void ResetWindowStats()
{
    g_delta60Stats.Reset();
    g_speedStats.Reset();
    g_steerStateStats.Reset();
    g_yawStats.Reset();
    g_boundaryCalls = 0;
    for (WheelStats& stats : g_wheelStats)
        stats.ResetWindow();
}

void ResetTurningWindowStats()
{
    g_turningStats.Reset();
    for (ContactStats& stats : g_turnContactStats)
        stats.Reset();
}

float NormalizePlayerDriveProperty(float value)
{
    if (!g_motorBrakeNormalizeABEnabled)
        return value;

    if (g_frameDeltaAddress == 0)
        return value;

    const float delta60 = *reinterpret_cast<const float*>(g_frameDeltaAddress);
    if (!std::isfinite(delta60) || delta60 <= kFloatEpsilon)
        return value;

    const float normalized = value / delta60;
    return std::isfinite(normalized) ? normalized : value;
}

void __fastcall HookSetMotorTorque(void* self, void*, float value)
{
    const int index = FindWheelIndex(self);
    float passedValue = value;
    if (index >= 0)
    {
        passedValue = NormalizePlayerDriveProperty(value);
        WheelStats& stats = g_wheelStats[static_cast<size_t>(index)];
        stats.motorTorque.Add(value);
        stats.motorTorquePassed.Add(passedValue);
    }
    g_originalSetMotorTorque(self, passedValue);
}

void __fastcall HookSetBrakeTorque(void* self, void*, float value)
{
    const int index = FindWheelIndex(self);
    float passedValue = value;
    if (index >= 0)
    {
        passedValue = NormalizePlayerDriveProperty(value);
        WheelStats& stats = g_wheelStats[static_cast<size_t>(index)];
        stats.brakeTorque.Add(value);
        stats.brakeTorquePassed.Add(passedValue);
    }
    g_originalSetBrakeTorque(self, passedValue);
}

void __fastcall HookSetSteerAngle(void* self, void*, float value)
{
    const int index = FindWheelIndex(self);
    if (index >= 0)
        g_wheelStats[static_cast<size_t>(index)].steerAngle.Add(value);
    g_originalSetSteerAngle(self, value);
}

void __fastcall HookSetLongTire(void* self, void*, TireFunctionDescCompat desc)
{
    const int index = FindWheelIndex(self);
    if (index >= 0)
        g_wheelStats[static_cast<size_t>(index)].longitudalTire.Add(desc);
    g_originalSetLongTire(self, desc);
}

void __fastcall HookSetLatTire(void* self, void*, TireFunctionDescCompat desc)
{
    const int index = FindWheelIndex(self);
    if (index >= 0)
        g_wheelStats[static_cast<size_t>(index)].lateralTire.Add(desc);
    g_originalSetLatTire(self, desc);
}

float __fastcall HookGetAxleSpeed(void* self, void*)
{
    const float value = g_originalGetAxleSpeed(self);
    const int index = FindWheelIndex(self);
    if (index >= 0)
        g_wheelStats[static_cast<size_t>(index)].axleSpeed.Add(value);
    return value;
}

void* __fastcall HookGetContact(void* self, void*, WheelContactDataCompat* dest)
{
    void* contactShape = g_originalGetContact(self, dest);
    const int index = FindWheelIndex(self);
    if (index >= 0)
    {
        ContactStats& stats = g_wheelStats[static_cast<size_t>(index)].contact;
        if (dest != nullptr)
            RecordContact(stats, contactShape, *dest);
        else
            ++stats.calls;
    }
    return contactShape;
}

bool InstallMethodHook(
    void* target,
    void* detour,
    void** original,
    const char* label,
    uint32_t bit)
{
    if (target == nullptr)
        return false;

    const MH_STATUS createStatus = MH_CreateHook(target, detour, original);
    if (createStatus != MH_OK)
    {
        char text[256] = {};
        sprintf_s(text, "[VehiclePhysXBoundary] WARNING: MH_CreateHook(%s=%p) failed: %d.\n",
            label, target, static_cast<int>(createStatus));
        AppendLog(text);
        return false;
    }

    const MH_STATUS enableStatus = MH_EnableHook(target);
    if (enableStatus != MH_OK)
    {
        MH_RemoveHook(target);
        *original = nullptr;
        char text[256] = {};
        sprintf_s(text, "[VehiclePhysXBoundary] WARNING: MH_EnableHook(%s=%p) failed: %d.\n",
            label, target, static_cast<int>(enableStatus));
        AppendLog(text);
        return false;
    }

    g_methodHookMask |= bit;
    return true;
}

bool DiscoverWheelsAndInstallPhysXHooks(void* car)
{
    if (g_physxHooksInstalled)
        return true;
    if (g_physxHookInstallFailed || car == nullptr)
        return false;

    // CObjectCar+0x26C points to a small owner slot whose first dword is the
    // live NxActor*. This is the same chain used by the normal/player FUN_00555B50 path.
    void* actorSlot = ReadPointer(car, 0x26C);
    if (actorSlot == nullptr)
        return false;
    void* actor = ReadPointer(actorSlot, 0x0);
    if (actor == nullptr)
        return false;

    void** actorVtable = *reinterpret_cast<void***>(actor);
    if (actorVtable == nullptr)
        return false;

    auto getNbShapes = reinterpret_cast<ActorGetNbShapesFn>(actorVtable[0x4C / 4]);
    auto getShapes = reinterpret_cast<ActorGetShapesFn>(actorVtable[0x50 / 4]);
    g_actorGetGlobalOrientation = reinterpret_cast<ActorGetMat33Fn>(actorVtable[0x2C / 4]);
    g_actorGetLinearVelocity = reinterpret_cast<ActorGetVec3Fn>(actorVtable[0xE8 / 4]);
    g_actorGetAngularVelocity = reinterpret_cast<ActorGetVec3Fn>(actorVtable[0xEC / 4]);
    if (getNbShapes == nullptr || getShapes == nullptr ||
        g_actorGetGlobalOrientation == nullptr || g_actorGetLinearVelocity == nullptr ||
        g_actorGetAngularVelocity == nullptr)
        return false;

    g_vehicleActor = actor;

    const unsigned int shapeCount = getNbShapes(actor);
    void** shapes = getShapes(actor);
    if (shapes == nullptr || shapeCount == 0)
        return false;

    g_wheelCount = 0;
    for (unsigned int shapeIndex = 0; shapeIndex < shapeCount && g_wheelCount < kMaxWheels; ++shapeIndex)
    {
        void* shape = shapes[shapeIndex];
        if (shape == nullptr)
            continue;
        void** shapeVtable = *reinterpret_cast<void***>(shape);
        if (shapeVtable == nullptr)
            continue;
        auto getType = reinterpret_cast<ShapeGetTypeFn>(shapeVtable[0x5C / 4]);
        if (getType != nullptr && getType(shape) == 4)
        {
            g_wheelPtrs[g_wheelCount] = shape;
            g_wheelShapeIndices[g_wheelCount] = shapeIndex;
            ++g_wheelCount;
        }
    }

    if (g_wheelCount == 0)
        return false;

    void** wheelVtable = *reinterpret_cast<void***>(g_wheelPtrs[0]);
    if (wheelVtable == nullptr)
        return false;

    void* setLong = wheelVtable[0xB4 / 4];
    void* setLat = wheelVtable[0xB8 / 4];
    void* setMotor = wheelVtable[0xD8 / 4];
    void* setBrake = wheelVtable[0xDC / 4];
    void* setSteer = wheelVtable[0xE0 / 4];
    void* getSteer = wheelVtable[0xEC / 4];
    void* getAxle = wheelVtable[0xF4 / 4];
    void* getContact = wheelVtable[0xF8 / 4];

    // All wheel shapes of one actor should use the same NxWheelShape class.
    // Refuse to partially instrument an unexpected mixed implementation.
    for (size_t i = 1; i < g_wheelCount; ++i)
    {
        void** vt = *reinterpret_cast<void***>(g_wheelPtrs[i]);
        if (vt == nullptr ||
            vt[0xB4 / 4] != setLong || vt[0xB8 / 4] != setLat ||
            vt[0xD8 / 4] != setMotor || vt[0xDC / 4] != setBrake ||
            vt[0xE0 / 4] != setSteer || vt[0xEC / 4] != getSteer ||
            vt[0xF4 / 4] != getAxle || vt[0xF8 / 4] != getContact)
        {
            AppendLog("[VehiclePhysXBoundary] ERROR: mixed NxWheelShape vtables; method hooks not installed.\n");
            g_physxHookInstallFailed = true;
            return false;
        }
    }

    const void* targets[] = { setLong, setLat, setMotor, setBrake, setSteer, getAxle, getContact };
    constexpr size_t kMethodCount = 7;
    for (size_t i = 0; i < kMethodCount; ++i)
    {
        for (size_t j = i + 1; j < kMethodCount; ++j)
        {
            if (targets[i] == targets[j])
            {
                AppendLog("[VehiclePhysXBoundary] ERROR: unexpected aliased NxWheelShape methods; hooks not installed.\n");
                g_physxHookInstallFailed = true;
                return false;
            }
        }
    }

    enum : uint32_t
    {
        HookLongTire = 1u << 0,
        HookLatTire = 1u << 1,
        HookMotor = 1u << 2,
        HookBrake = 1u << 3,
        HookSteer = 1u << 4,
        HookAxle = 1u << 5,
        HookContact = 1u << 6
    };

    g_methodHookMask = 0;
    InstallMethodHook(setLong, reinterpret_cast<void*>(&HookSetLongTire),
        reinterpret_cast<void**>(&g_originalSetLongTire), "setLongitudalTireForceFunction", HookLongTire);
    InstallMethodHook(setLat, reinterpret_cast<void*>(&HookSetLatTire),
        reinterpret_cast<void**>(&g_originalSetLatTire), "setLateralTireForceFunction", HookLatTire);
    InstallMethodHook(setMotor, reinterpret_cast<void*>(&HookSetMotorTorque),
        reinterpret_cast<void**>(&g_originalSetMotorTorque), "setMotorTorque", HookMotor);
    InstallMethodHook(setBrake, reinterpret_cast<void*>(&HookSetBrakeTorque),
        reinterpret_cast<void**>(&g_originalSetBrakeTorque), "setBrakeTorque", HookBrake);
    InstallMethodHook(setSteer, reinterpret_cast<void*>(&HookSetSteerAngle),
        reinterpret_cast<void**>(&g_originalSetSteerAngle), "setSteerAngle", HookSteer);
    InstallMethodHook(getAxle, reinterpret_cast<void*>(&HookGetAxleSpeed),
        reinterpret_cast<void**>(&g_originalGetAxleSpeed), "getAxleSpeed", HookAxle);
    InstallMethodHook(getContact, reinterpret_cast<void*>(&HookGetContact),
        reinterpret_cast<void**>(&g_originalGetContact), "getContact", HookContact);

    // Use the undetoured trampoline for the audit's explicit read-only contact
    // sample so those reads remain distinguishable from native game calls.
    g_getSteerAngle = reinterpret_cast<GetWheelFloatFn>(getSteer);
    g_readContact = g_originalGetContact != nullptr
        ? g_originalGetContact
        : reinterpret_cast<GetWheelContactFn>(getContact);

    constexpr uint32_t kCoreMask =
        HookLongTire | HookLatTire | HookMotor | HookBrake | HookSteer | HookAxle;
    if ((g_methodHookMask & kCoreMask) == 0)
    {
        AppendLog("[VehiclePhysXBoundary] ERROR: no core NxWheelShape method hooks could be enabled.\n");
        g_physxHookInstallFailed = true;
        return false;
    }

    g_physxHooksInstalled = true;

    char text[768] = {};
    int used = sprintf_s(
        text,
        "[VehiclePhysXBoundary] NxWheelShape hooks armed: wheels=%u actor=%p hookMask=0x%02X methods long=%p lat=%p motor=%p brake=%p steer=%p getSteer=%p axle=%p contact=%p; shapeIndices=",
        static_cast<unsigned int>(g_wheelCount), actor, static_cast<unsigned int>(g_methodHookMask),
        setLong, setLat, setMotor, setBrake, setSteer, getSteer, getAxle, getContact);
    for (size_t i = 0; i < g_wheelCount && used > 0 && static_cast<size_t>(used) < sizeof(text) - 32; ++i)
    {
        used += sprintf_s(text + used, sizeof(text) - static_cast<size_t>(used),
            "%s%u", i == 0 ? "" : ",", g_wheelShapeIndices[i]);
    }
    if (used > 0 && static_cast<size_t>(used) < sizeof(text) - 3)
    {
        text[used++] = '.';
        text[used++] = '\n';
        text[used] = '\0';
        AppendLog(text);
    }

    sprintf_s(
        text,
        "[VehicleTurn] Read-only turning audit armed: actor getOrientation=%p getLinearVelocity=%p getAngularVelocity=%p "
        "wheel getSteerAngle=%p getContactRead=%p; slice=250ms.\n",
        reinterpret_cast<void*>(g_actorGetGlobalOrientation),
        reinterpret_cast<void*>(g_actorGetLinearVelocity),
        reinterpret_cast<void*>(g_actorGetAngularVelocity),
        reinterpret_cast<void*>(g_getSteerAngle),
        reinterpret_cast<void*>(g_readContact));
    AppendLog(text);
    ResetTurningWindowStats();
    return true;
}

void LogWheelWindow(size_t index, double seconds)
{
    if (index >= g_wheelCount)
        return;

    const WheelStats& w = g_wheelStats[index];
    const double invSeconds = seconds > 0.0 ? 1.0 / seconds : 0.0;
    char text[1200] = {};

    sprintf_s(
        text,
        "[VehiclePhysXWheel] w=%u shape=%u ptr=%p callsHz motor/brake/steer/long/lat/axle/contact=%.1f/%.1f/%.1f/%.1f/%.1f/%.1f/%.1f "
        "motor raw avg/min/max=%+.5g/%+.5g/%+.5g passed=%+.5g/%+.5g/%+.5g "
        "brake raw avg/min/max=%.5g/%.5g/%.5g passed=%.5g/%.5g/%.5g "
        "steer=%+.6f/%+.6f/%+.6f axle=%+.5g/%+.5g/%+.5g.\n",
        static_cast<unsigned int>(index), g_wheelShapeIndices[index], g_wheelPtrs[index],
        static_cast<double>(w.motorTorque.count) * invSeconds,
        static_cast<double>(w.brakeTorque.count) * invSeconds,
        static_cast<double>(w.steerAngle.count) * invSeconds,
        static_cast<double>(w.longitudalTire.calls) * invSeconds,
        static_cast<double>(w.lateralTire.calls) * invSeconds,
        static_cast<double>(w.axleSpeed.count) * invSeconds,
        static_cast<double>(w.contact.calls) * invSeconds,
        w.motorTorque.Average(), static_cast<double>(w.motorTorque.min), static_cast<double>(w.motorTorque.max),
        w.motorTorquePassed.Average(), static_cast<double>(w.motorTorquePassed.min), static_cast<double>(w.motorTorquePassed.max),
        w.brakeTorque.Average(), static_cast<double>(w.brakeTorque.min), static_cast<double>(w.brakeTorque.max),
        w.brakeTorquePassed.Average(), static_cast<double>(w.brakeTorquePassed.min), static_cast<double>(w.brakeTorquePassed.max),
        w.steerAngle.Average(), static_cast<double>(w.steerAngle.min), static_cast<double>(w.steerAngle.max),
        w.axleSpeed.Average(), static_cast<double>(w.axleSpeed.min), static_cast<double>(w.axleSpeed.max));
    AppendLog(text);

    if (w.longitudalTire.havePrevious || w.lateralTire.havePrevious)
    {
        const TireFunctionDescCompat& lng = w.longitudalTire.last;
        const TireFunctionDescCompat& lat = w.lateralTire.last;
        sprintf_s(
            text,
            "[VehiclePhysXTire] w=%u long changeHz=%.2f desc=%.6g/%.6g/%.6g/%.6g/%.6g "
            "lat changeHz=%.2f desc=%.6g/%.6g/%.6g/%.6g/%.6g.\n",
            static_cast<unsigned int>(index),
            static_cast<double>(w.longitudalTire.changed) * invSeconds,
            static_cast<double>(lng.extremumSlip), static_cast<double>(lng.extremumValue),
            static_cast<double>(lng.asymptoteSlip), static_cast<double>(lng.asymptoteValue),
            static_cast<double>(lng.stiffnessFactor),
            static_cast<double>(w.lateralTire.changed) * invSeconds,
            static_cast<double>(lat.extremumSlip), static_cast<double>(lat.extremumValue),
            static_cast<double>(lat.asymptoteSlip), static_cast<double>(lat.asymptoteValue),
            static_cast<double>(lat.stiffnessFactor));
        AppendLog(text);
    }

    if (w.contact.calls != 0)
    {
        sprintf_s(
            text,
            "[VehiclePhysXContact] w=%u hits=%lu/%lu force avg/max=%.5g/%.5g slip long/lat avg=%+.6g/%+.6g "
            "impulse long/lat avg=%+.6g/%+.6g pos avg=%.6g.\n",
            static_cast<unsigned int>(index), w.contact.hits, w.contact.calls,
            w.contact.force.Average(), static_cast<double>(w.contact.force.max),
            w.contact.longitudalSlip.Average(), w.contact.lateralSlip.Average(),
            w.contact.longitudalImpulse.Average(), w.contact.lateralImpulse.Average(),
            w.contact.position.Average());
        AppendLog(text);
    }
}

void LogTurningWindow(double seconds)
{
    if (g_turningStats.samples == 0)
        return;

    const double invSeconds = seconds > 0.0 ? 1.0 / seconds : 0.0;
    const double timebaseRatio =
        g_turningStats.absActorYawRate.Average() >= 0.05 &&
        g_turningStats.absPoseYawRate.count != 0
            ? g_turningStats.absPoseYawRate.Average() /
                g_turningStats.absActorYawRate.Average()
            : 0.0;
    char text[1400] = {};
    sprintf_s(
        text,
        "[VehicleTurn] slice=%u elapsed=%.0fms sampleHz=%.1f "
        "delta60=%.4f speed=%.3f steerState=%+.5f wheelSteer=%+.6f/%+.6f/%+.6f "
        "linSpeed=%.4f localV lat/up/fwd=%+.4f/%+.4f/%+.4f "
        "slipDeg avg/absMax=%+.4f/%.4f "
        "angVel x/y/z=%+.5f/%+.5f/%+.5f yawAbs=%.5f "
        "yawRatePose avg/min/max=%+.5f/%+.5f/%+.5f abs=%.5f timebaseR=%.4f "
        "yawAccel=%+.5f latAccel=%+.5f yawGain=%+.5f curvature=%+.7f radius=%.4f.\n",
        g_loggedTurnWindows + 1,
        seconds * 1000.0,
        static_cast<double>(g_turningStats.samples) * invSeconds,
        g_turningStats.delta60.Average(),
        g_turningStats.carSpeed.Average(),
        g_turningStats.steerState.Average(),
        g_turningStats.actualSteer.Average(),
        static_cast<double>(g_turningStats.actualSteer.min),
        static_cast<double>(g_turningStats.actualSteer.max),
        g_turningStats.actorLinearSpeed.Average(),
        g_turningStats.localLateralVelocity.Average(),
        g_turningStats.localVerticalVelocity.Average(),
        g_turningStats.localForwardVelocity.Average(),
        g_turningStats.slipAngle.Average() * (180.0 / 3.14159265358979323846),
        static_cast<double>(g_turningStats.absSlipAngle.max) * (180.0 / 3.14159265358979323846),
        g_turningStats.actorAngularX.Average(),
        g_turningStats.actorYawRate.Average(),
        g_turningStats.actorAngularZ.Average(),
        g_turningStats.absActorYawRate.Average(),
        g_turningStats.poseYawRate.Average(),
        static_cast<double>(g_turningStats.poseYawRate.min),
        static_cast<double>(g_turningStats.poseYawRate.max),
        g_turningStats.absPoseYawRate.Average(),
        timebaseRatio,
        g_turningStats.yawAcceleration.Average(),
        g_turningStats.lateralAcceleration.Average(),
        g_turningStats.yawGain.Average(),
        g_turningStats.curvature.Average(),
        g_turningStats.turnRadius.Average());
    AppendLog(text);

    for (size_t i = 0; i < g_wheelCount; ++i)
    {
        const ContactStats& c = g_turnContactStats[i];
        if (c.calls == 0)
            continue;
        sprintf_s(
            text,
            "[VehicleTurnContact] slice=%u w=%u hits=%lu/%lu force avg/max=%.5g/%.5g "
            "slip long/lat avg=%+.6g/%+.6g impulse long/lat avg=%+.6g/%+.6g pos=%.6g.\n",
            g_loggedTurnWindows + 1,
            static_cast<unsigned int>(i),
            c.hits,
            c.calls,
            c.force.Average(),
            static_cast<double>(c.force.max),
            c.longitudalSlip.Average(),
            c.lateralSlip.Average(),
            c.longitudalImpulse.Average(),
            c.lateralImpulse.Average(),
            c.position.Average());
        AppendLog(text);
    }
}

void SampleTurningState(void* car)
{
    if (car == nullptr || !g_physxHooksInstalled || g_vehicleActor == nullptr ||
        g_actorGetGlobalOrientation == nullptr || g_actorGetLinearVelocity == nullptr ||
        g_actorGetAngularVelocity == nullptr || g_getSteerAngle == nullptr ||
        g_loggedTurnWindows >= kMaxTurnWindows)
        return;

    const DpBuildProfile* build = GetDpBuildProfile();
    if (build == nullptr || g_mainExeBase == 0)
        return;

    LARGE_INTEGER nowQpc{};
    QueryPerformanceCounter(&nowQpc);

    const Mat33Compat orientation = g_actorGetGlobalOrientation(g_vehicleActor);
    const Vec3Compat linearVelocity = g_actorGetLinearVelocity(g_vehicleActor);
    const Vec3Compat angularVelocity = g_actorGetAngularVelocity(g_vehicleActor);

    const float localLat = Dot(linearVelocity, orientation.col0);
    const float localUp = Dot(linearVelocity, orientation.col1);
    const float localFwd = Dot(linearVelocity, orientation.col2);
    const float linearSpeed = std::sqrt(
        linearVelocity.x * linearVelocity.x +
        linearVelocity.y * linearVelocity.y +
        linearVelocity.z * linearVelocity.z);
    const float slipAngle = std::atan2(localLat, std::max(std::fabs(localFwd), 0.001f));

    float actualSteerSum = 0.0f;
    unsigned int actualSteerCount = 0;
    for (size_t i = 0; i < g_wheelCount; ++i)
    {
        const float steer = g_getSteerAngle(g_wheelPtrs[i]);
        if (std::isfinite(steer) && std::fabs(steer) > 0.00001f)
        {
            actualSteerSum += steer;
            ++actualSteerCount;
        }

        if (g_readContact != nullptr)
        {
            WheelContactDataCompat contact{};
            void* shape = g_readContact(g_wheelPtrs[i], &contact);
            RecordContact(g_turnContactStats[i], shape, contact);
        }
    }
    const float actualSteer = actualSteerCount != 0
        ? actualSteerSum / static_cast<float>(actualSteerCount)
        : 0.0f;

    const float delta60 = *reinterpret_cast<const float*>(g_mainExeBase + build->frameDeltaRva);
    const float carSpeed = std::fabs(ReadFloat(car, 0x13E4));
    const float steerState = ReadFloat(car, 0x4E0);
    const float actorPoseYaw = std::atan2(orientation.col2.x, orientation.col2.z);

    g_turningStats.delta60.Add(delta60);
    g_turningStats.carSpeed.Add(carSpeed);
    g_turningStats.steerState.Add(steerState);
    g_turningStats.actualSteer.Add(actualSteer);
    g_turningStats.actorLinearSpeed.Add(linearSpeed);
    g_turningStats.localLateralVelocity.Add(localLat);
    g_turningStats.localVerticalVelocity.Add(localUp);
    g_turningStats.localForwardVelocity.Add(localFwd);
    g_turningStats.slipAngle.Add(slipAngle);
    g_turningStats.absSlipAngle.Add(std::fabs(slipAngle));
    g_turningStats.actorAngularX.Add(angularVelocity.x);
    g_turningStats.actorYawRate.Add(angularVelocity.y);
    g_turningStats.absActorYawRate.Add(std::fabs(angularVelocity.y));
    g_turningStats.actorAngularZ.Add(angularVelocity.z);

    if (std::fabs(actualSteer) >= 0.02f)
        g_turningStats.yawGain.Add(angularVelocity.y / actualSteer);
    if (std::fabs(localFwd) >= 0.5f)
        g_turningStats.curvature.Add(angularVelocity.y / localFwd);
    if (std::fabs(angularVelocity.y) >= 0.02f)
        g_turningStats.turnRadius.Add(
            std::sqrt(localLat * localLat + localFwd * localFwd) /
            std::fabs(angularVelocity.y));

    if (g_previousTurnCar != car)
    {
        g_previousTurnCar = car;
        g_havePreviousTurnSample = false;
    }

    if (g_havePreviousTurnSample && g_qpcFrequency.QuadPart > 0)
    {
        const double dt = static_cast<double>(nowQpc.QuadPart - g_previousTurnQpc.QuadPart) /
            static_cast<double>(g_qpcFrequency.QuadPart);
        if (dt > 0.0001 && dt < 0.25)
        {
            float deltaYaw = actorPoseYaw - g_previousRawYaw;
            constexpr float kPi = 3.14159265358979323846f;
            constexpr float kTwoPi = 2.0f * kPi;
            while (deltaYaw > kPi)
                deltaYaw -= kTwoPi;
            while (deltaYaw < -kPi)
                deltaYaw += kTwoPi;

            const float poseYawRate = deltaYaw / static_cast<float>(dt);
            g_turningStats.poseYawRate.Add(poseYawRate);
            g_turningStats.absPoseYawRate.Add(std::fabs(poseYawRate));
            g_turningStats.lateralAcceleration.Add(
                (localLat - g_previousLocalLateralVelocity) / static_cast<float>(dt));
            g_turningStats.yawAcceleration.Add(
                (angularVelocity.y - g_previousActorYawRate) / static_cast<float>(dt));
        }
    }

    g_previousTurnQpc = nowQpc;
    g_previousRawYaw = actorPoseYaw;
    g_previousLocalLateralVelocity = localLat;
    g_previousActorYawRate = angularVelocity.y;
    g_havePreviousTurnSample = true;
    ++g_turningStats.samples;

    const ULONGLONG nowMs = GetTickCount64();
    if (g_turnWindowStartMs == 0)
    {
        g_turnWindowStartMs = nowMs;
        ResetTurningWindowStats();
        return;
    }

    const ULONGLONG elapsedMs = nowMs - g_turnWindowStartMs;
    if (elapsedMs < kTurnWindowMs)
        return;

    LogTurningWindow(static_cast<double>(elapsedMs) / 1000.0);
    ++g_loggedTurnWindows;
    g_turnWindowStartMs = nowMs;
    ResetTurningWindowStats();
}

void MaybeLogWindow(void* car)
{
    if (car == nullptr || g_loggedWindows >= kMaxLoggedWindows)
        return;

    const DpBuildProfile* build = GetDpBuildProfile();
    if (build == nullptr || g_mainExeBase == 0)
        return;

    const float delta60 = *reinterpret_cast<const float*>(g_mainExeBase + build->frameDeltaRva);
    g_delta60Stats.Add(delta60);
    g_speedStats.Add(std::fabs(ReadFloat(car, 0x13E4)));
    g_steerStateStats.Add(ReadFloat(car, 0x4E0));
    g_yawStats.Add(ReadFloat(car, 0x7C));
    ++g_boundaryCalls;

    const ULONGLONG now = GetTickCount64();
    if (g_windowStartMs == 0)
    {
        g_windowStartMs = now;
        ResetWindowStats();
        return;
    }

    const ULONGLONG elapsed = now - g_windowStartMs;
    if (elapsed < 1000)
        return;

    const double seconds = static_cast<double>(elapsed) / 1000.0;
    char text[800] = {};
    sprintf_s(
        text,
        "[VehiclePhysXBoundary] window=%u elapsed=%llums player=%08lX boundary=%.1fHz wheels=%u hooks=%s mask=0x%02X "
        "delta60 avg/min/max=%.4f/%.4f/%.4f speedAbs avg/max=%.2f/%.2f "
        "steerState avg/min/max=%+.5f/%+.5f/%+.5f yaw avg/min/max=%+.5f/%+.5f/%+.5f.\n",
        g_loggedWindows + 1,
        static_cast<unsigned long long>(elapsed),
        static_cast<unsigned long>(reinterpret_cast<uintptr_t>(car)),
        seconds > 0.0 ? static_cast<double>(g_boundaryCalls) / seconds : 0.0,
        static_cast<unsigned int>(g_wheelCount),
        g_physxHooksInstalled ? "armed" : (g_physxHookInstallFailed ? "failed" : "pending"),
        static_cast<unsigned int>(g_methodHookMask),
        g_delta60Stats.Average(), static_cast<double>(g_delta60Stats.min), static_cast<double>(g_delta60Stats.max),
        g_speedStats.Average(), static_cast<double>(g_speedStats.max),
        g_steerStateStats.Average(), static_cast<double>(g_steerStateStats.min), static_cast<double>(g_steerStateStats.max),
        g_yawStats.Average(), static_cast<double>(g_yawStats.min), static_cast<double>(g_yawStats.max));
    AppendLog(text);

    for (size_t i = 0; i < g_wheelCount; ++i)
        LogWheelWindow(i, seconds);

    ++g_loggedWindows;
    g_windowStartMs = now;
    ResetWindowStats();
}

void __fastcall HookPlayerWheelBoundary(void* self, void*)
{
    ++g_playerBoundaryDepth;
    void* oldCar = g_boundaryCar;
    g_boundaryCar = self;

    g_originalPlayerWheelBoundary(self);

    // The first invocation discovers the actual runtime NxActor/NxWheelShape
    // methods.  The turning sample is taken after the native wheel-facing pass
    // so it sees the exact steer/tire state that PhysX will consume.
    if (!g_physxHooksInstalled && !g_physxHookInstallFailed)
        DiscoverWheelsAndInstallPhysXHooks(self);
    if (g_physxHooksInstalled)
        SampleTurningState(self);

    g_boundaryCar = oldCar;
    --g_playerBoundaryDepth;

    MaybeLogWindow(self);
}
} // namespace

bool InstallVehicleTimingDiag()
{
    const DpBuildProfile* build = GetDpBuildProfile();
    if (build == nullptr || g_mainExeBase == 0)
    {
        AppendLog("[VehiclePhysXBoundary] Unsupported DP.exe build; probe not installed.\n");
        return false;
    }

    QueryPerformanceFrequency(&g_qpcFrequency);
    g_motorBrakeNormalizeABEnabled = false;
    g_frameDeltaAddress = g_mainExeBase + build->frameDeltaRva;

    // The older tire-timing experiment remains disabled in this audit build.
    // The only optional wheel-state mutation is the separately gated motor/brake
    // A/B below; steering, tire descriptors and all read-only turning samples stay native.
    if (g_config.fixVehicleTireTiming)
    {
        AppendLog(
            "[VehiclePhysXBoundary] NOTE: Physics.VehicleTireTimingFix=true is ignored in this "
            "audit build; native PC wheel/tire values are observed unchanged.\n");
    }

    {
        char modeText[768] = {};
        sprintf_s(
            modeText,
            "[VehicleDriveAB] Motor/brake setter mutation disabled in v2.2: NATIVE values are always passed. "
            "Xbox sub_82354198 and PC FUN_00555B50 both multiply drive setters by their central gameplay-time scalar; "
            "the old VehicleMotorBrakeNormalizeAB=%s setting is retained only for backward-compatible research INI parsing.\n",
            g_config.vehicleMotorBrakeNormalizeAB ? "true (ignored)" : "false");
        AppendLog(modeText);
    }

    const uintptr_t boundary = g_mainExeBase + build->vehiclePhysxWheelBoundaryRva;
    static constexpr unsigned char kBoundarySigA[] = {
        0x6A, 0xFF, 0x68
    };
    static constexpr unsigned char kBoundarySigB[] = {
        0x64, 0xA1, 0x00, 0x00, 0x00, 0x00, 0x50,
        0x81, 0xEC, 0xF8, 0x00, 0x00, 0x00,
        0x53, 0x55, 0x56, 0x57
    };

    if (!MatchesBytes(boundary, kBoundarySigA, sizeof(kBoundarySigA)) ||
        !MatchesBytes(boundary + 7, kBoundarySigB, sizeof(kBoundarySigB)))
    {
        AppendLog("[VehiclePhysXBoundary] ERROR: player NxWheelShape boundary signature mismatch; probe not installed.\n");
        return false;
    }

    const MH_STATUS createStatus = MH_CreateHook(
        reinterpret_cast<void*>(boundary),
        reinterpret_cast<void*>(&HookPlayerWheelBoundary),
        reinterpret_cast<void**>(&g_originalPlayerWheelBoundary));
    if (createStatus != MH_OK)
    {
        char text[256] = {};
        sprintf_s(text, "[VehiclePhysXBoundary] ERROR: MH_CreateHook(boundary) failed: %d.\n",
            static_cast<int>(createStatus));
        AppendLog(text);
        return false;
    }

    if (MH_EnableHook(reinterpret_cast<void*>(boundary)) != MH_OK)
    {
        MH_RemoveHook(reinterpret_cast<void*>(boundary));
        AppendLog("[VehiclePhysXBoundary] ERROR: could not enable player boundary hook.\n");
        return false;
    }

    char text[640] = {};
    sprintf_s(
        text,
        "[VehiclePhysXBoundary] Read-only normal/player CObjectCar -> PhysX 2.8.1 NxWheelShape probe enabled on %s at DP.exe+0x%08lX. "
        "Runtime vtables will arm the existing wheel-boundary hooks plus a read-only turning audit: NxActor linear/angular velocity, "
        "actor-pose yaw rate, local lateral/forward velocity, actual getSteerAngle and explicit getContact samples; %u one-second windows "
        "with 250ms VehicleTurn slices. Turning/contact sampling and motor/brake setter hooks are read-only in v2.2; "
        "scene-time mutation is owned by Physics.PhysXRealTimeAB and player-car cadence mutation by Physics.VehicleXboxTickCadence.\n",
        build->name,
        static_cast<unsigned long>(build->vehiclePhysxWheelBoundaryRva),
        kMaxLoggedWindows);
    AppendLog(text);
    return true;
}
