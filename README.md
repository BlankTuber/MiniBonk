# MiniBonk

A survivor-like roguelite prototype built in Unreal Engine 5.7 with C++ as a learning project.

**Engine:** UE 5.7 | **Language:** C++ | **IDE:** Visual Studio | **VCS:** GitHub

---

## Game Overview

Auto-attack roguelite where the player navigates a 3D arena while enemies spawn in waves. Collect coins, level up, choose upgrades, survive.

**Core Loop:**
1. Enemies spawn and chase player
2. Player auto-attacks nearby enemies
3. Enemies drop coins on death
4. Collect coins → level up → choose upgrade
5. Difficulty scales over time
6. Survive until death (or optional boss defeat)

---

## MVP Scope

### Player
- Third-person movement (speed/jump modifiable)
- Health system (damage, death, upgradeable)
- Auto-attack (projectile + melee slash)

### Enemies
- 2 enemy types (basic chaser + variant)
- 1 mini-boss (ranged attacks)
- Chase AI, health, death, coin drops
- Contact damage with cooldown

### Systems
- **Spawning:** Distance-based probability, time-scaling difficulty
- **Economy:** Coin pickups, upgradeable pickup radius
- **Progression:** Level-up thresholds, upgrade wheel (3 options), stackable upgrades
- **Interactables:** Chests (cost coins → random upgrade), power-ups (heal, speed, mega-coin)

### World
- Single arena map with boundary walls
- Nav mesh coverage

### UI
- Health bar, coin counter, XP bar
- Upgrade selection screen
- Game over screen

---

## Checklist

### Setup
- [x] Project created
- [x] Public/Private folder structure
- [ ] GitHub repo initialized

### Player System
- [ ] Basic movement
- [ ] Speed/jump modifiers
- [ ] Health component
- [ ] Take damage / death
- [ ] Auto-attack (timer, melee, projectile)
- [ ] Attack stat modifiers (speed, damage, range)

### Enemy System
- [ ] Base enemy class
- [ ] AI chase behavior
- [ ] Health / damage / death
- [ ] Coin drops on death
- [ ] Enemy variant (different stats)
- [ ] Mini-boss (ranged attack)
- [ ] Contact damage with cooldown

### Spawning
- [ ] Spawn manager
- [ ] Distance-based spawn logic
- [ ] Time-based difficulty scaling
- [ ] Weighted enemy selection
- [ ] Mini-boss trigger

### Economy & Progression
- [ ] Coin actor + collection
- [ ] Coin counter
- [ ] Level-up threshold (scaling)
- [ ] Upgrade selection UI
- [ ] Apply upgrades to player
- [ ] Pickup radius (base + upgradeable)

### Interactables
- [ ] Chest actor (interaction, cost, reward)
- [ ] Power-up drops (heal, speed, mega-coin)

### World & UI
- [ ] Arena with boundaries
- [ ] Nav mesh
- [ ] HUD (health, coins, XP)
- [ ] Upgrade wheel
- [ ] Game over screen

### Game State
- [ ] Playing / paused / game over states
- [ ] Death detection → game over
- [ ] Restart option

### Polish (Secondary)
- [ ] Audio (hits, death, coins, music)
- [ ] VFX (particles, screen shake, damage numbers)
- [ ] Start menu
- [ ] Persistent currency / permanent upgrades

---

## Quick Reference

### Naming Conventions

**C++ Classes**
| Type | Prefix | Example |
|------|--------|---------|
| Actor | A | `AEnemyBase` |
| UObject / Component | U | `UHealthComponent` |
| Struct | F | `FDamageInfo` |
| Enum | E | `EGameState` |
| Interface | I | `IDamageable` |
| Bool variable | b | `bIsAlive` |

**Content Assets**
| Type | Prefix | Example |
|------|--------|---------|
| Blueprint | BP_ | `BP_Enemy` |
| Static Mesh | SM_ | `SM_Coin` |
| Skeletal Mesh | SK_ | `SK_Player` |
| Material | M_ | `M_Ground` |
| Material Instance | MI_ | `MI_Ground_Wet` |
| Texture | T_ | `T_Rock_D` (diffuse), `T_Rock_N` (normal) |
| Animation | A_ | `A_Run` |
| Anim Blueprint | ABP_ | `ABP_Player` |
| Anim Montage | AM_ | `AM_Attack` |
| Particle System | PS_ | `PS_Explosion` |
| Sound | A_ | `A_Hit`, `A_Hit_Cue` |
| Widget | WBP_ | `WBP_HUD` |
| Data Table | DT_ | `DT_Enemies` |

---

### Folder Structure

**Source (C++)**
```
Source/MiniBonk/
├── Public/              # .h files
│   ├── Core/            # GameMode, GameState
│   ├── Characters/      # Player, enemies
│   ├── Components/      # Health, attack, etc.
│   ├── Systems/         # Spawning, progression
│   ├── Actors/          # Coins, chests, projectiles
│   ├── UI/              # HUD classes
│   └── Data/            # Structs, enums
└── Private/             # .cpp files (mirror Public)
```

**Content (Assets)**
```
Content/MiniBonk/
├── Core/                # GameMode BP, essential BPs
├── Characters/
│   ├── Player/
│   │   ├── Animations/
│   │   └── Meshes/
│   └── Enemies/
├── Pickups/             # Coins, power-ups
├── Effects/             # Particles
├── Audio/
├── UI/
├── Maps/
└── Data/                # DataTables
```

---

### Header File Template

```cpp
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyActor.generated.h"  // Always last

class UMyComponent;  // Forward declare when possible

UCLASS()
class MINIBONK_API AMyActor : public AActor
{
    GENERATED_BODY()

public:
    AMyActor();

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UMyComponent> MyComponent;
};
```

**Include Order:**
1. `#pragma once`
2. `CoreMinimal.h`
3. Engine headers
4. Project headers
5. `.generated.h` (must be last)

**Forward Declarations:** Use in headers when you only need pointers/references. Include the actual header in the .cpp file.

---

### UPROPERTY

```cpp
// Designer-tweakable value
UPROPERTY(EditDefaultsOnly, Category = "Stats")
float MaxHealth = 100.f;

// Runtime state, BP readable
UPROPERTY(BlueprintReadOnly, Category = "Stats")
float CurrentHealth;

// Component reference
UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
TObjectPtr<UHealthComponent> HealthComp;

// Class reference for spawning
UPROPERTY(EditDefaultsOnly, Category = "Config")
TSubclassOf<AActor> ProjectileClass;
```

| Specifier | Use Case |
|-----------|----------|
| `EditDefaultsOnly` | Tweakable in BP defaults only |
| `EditAnywhere` | Tweakable everywhere |
| `VisibleAnywhere` | Read-only display |
| `BlueprintReadOnly` | BP can read |
| `BlueprintReadWrite` | BP can read/write |

---

### UFUNCTION

```cpp
// BP can call
UFUNCTION(BlueprintCallable, Category = "Combat")
void TakeDamage(float Amount);

// Getter (no exec pin in BP)
UFUNCTION(BlueprintPure, Category = "Stats")
float GetHealthPercent() const;

// BP can override
UFUNCTION(BlueprintNativeEvent, Category = "Events")
void OnDeath();
void OnDeath_Implementation();  // Required
```

---

### Object Creation

```cpp
// Constructor - components
HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComp"));

// Runtime - UObjects
UMyObject* Obj = NewObject<UMyObject>(this);

// Runtime - Actors
AActor* Actor = GetWorld()->SpawnActor<AMyActor>(Location, Rotation);
```

---

### Debugging

**Assertions**
```cpp
// Non-fatal: logs error, continues execution
ensure(Pointer != nullptr);
ensureMsgf(Value > 0, TEXT("Expected positive value, got %d"), Value);

// Fatal in dev builds: halts execution
check(Pointer != nullptr);
checkf(Index >= 0, TEXT("Invalid index: %d"), Index);
```

Use `ensure()` liberally during development to catch issues without crashing.

**Logging**
```cpp
UE_LOG(LogTemp, Warning, TEXT("Health: %f"), CurrentHealth);
UE_LOG(LogTemp, Error, TEXT("Actor is null!"));

// On-screen
if (GEngine)
{
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Debug"));
}
```

**Visual Debug**
```cpp
#include "DrawDebugHelpers.h"

DrawDebugSphere(GetWorld(), Location, Radius, 12, FColor::Red, false, 0.f);
DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 0.f);
```

---

### Common Patterns

**Timers**
```cpp
FTimerHandle TimerHandle;

// Looping
GetWorldTimerManager().SetTimer(TimerHandle, this, &AMyClass::OnTimer, Rate, true);

// One-shot
GetWorldTimerManager().SetTimer(TimerHandle, this, &AMyClass::OnTimer, Delay, false);

// Stop
GetWorldTimerManager().ClearTimer(TimerHandle);
```

**Overlap Events**
```cpp
// Header
UFUNCTION()
void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult);

// BeginPlay
CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AMyClass::OnOverlapBegin);
```

**Casting**
```cpp
if (AMiniBonkCharacter* Player = Cast<AMiniBonkCharacter>(OtherActor))
{
    Player->TakeDamage(Damage);
}
```

**Getting References**
```cpp
// Player
APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

// Game Mode
AMiniBonkGameMode* GM = Cast<AMiniBonkGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

// Component
UHealthComp* Health = Actor->FindComponentByClass<UHealthComp>();
```

**Random**
```cpp
float RandFloat = FMath::RandRange(1.f, 10.f);
int32 RandInt = FMath::RandRange(1, 10);

if (FMath::FRand() < 0.3f) { /* 30% chance */ }
```

**Distance**
```cpp
float Dist = FVector::Dist(GetActorLocation(), Other->GetActorLocation());
```

---

### Design Principles

**Separation of Concerns**
- One class = one responsibility
- Use components for reusable behavior (HealthComponent, AttackComponent)
- Systems manage global logic (SpawnManager, ProgressionManager)
- Actors are the "things" in the world

**Composition Over Inheritance**
- Prefer adding components to creating deep class hierarchies
- Base classes for shared interface, components for shared behavior

**Data-Driven**
- Use DataTables for tunable values (enemy stats, upgrade definitions)
- `EditDefaultsOnly` properties for per-class configuration
- Easier to tweak without recompiling

**Pointer Safety**
```cpp
// Always validate before use
if (IsValid(MyActor))
{
    MyActor->DoSomething();
}

// UPROPERTY prevents garbage collection issues
UPROPERTY()
TObjectPtr<AActor> MyActor;  // Safe

AActor* RawPointer;  // Dangerous - can become dangling
```

---

### Common Includes

```cpp
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
```