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
- [X] GitHub repo initialized

### Player System
- [X] Basic movement
- [X] Basic camera movement
- [ ] Speed/jump modifiers
- [X] Health component
- [ ] Take damage / death
- [ ] Auto-attack (timer, melee, projectile)
- [ ] Attack stat modifiers (speed, damage, range)

### Enemy System
- [X] Base enemy class
- [X] AI chase behavior
- [X] Health component
- [ ] Take damage / death
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

**Include Order:**
1. `#pragma once`
2. `CoreMinimal.h`
3. Engine headers
4. Project headers
5. `.generated.h` (must be last)

**Forward Declarations:** Use in headers when you only need pointers/references. Include the actual header in the .cpp file.

---

### UPROPERTY

| Specifier | Use Case |
|-----------|----------|
| `EditDefaultsOnly` | Tweakable in BP defaults only |
| `EditAnywhere` | Tweakable everywhere |
| `VisibleAnywhere` | Read-only display |
| `BlueprintReadOnly` | BP can read |
| `BlueprintReadWrite` | BP can read/write |

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

---

### Common Includes

```cpp
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
```