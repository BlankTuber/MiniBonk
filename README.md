# MiniBonk

A third-person roguelite prototype built in Unreal Engine 5.7 with C++ as a learning project.

**Engine:** UE 5.7 | **Language:** C++ | **IDE:** Visual Studio

---

## Game Overview

Third-person action roguelite where you fight waves of enemies, collect upgrades, and survive as long as possible. Unlike traditional top-down survivors, this uses standard third-person controls with camera-relative movement.

**Core Loop:**
1. Enemies spawn and chase the player
2. Player attacks enemies (auto-attack + unlockable abilities)
3. Enemies drop coins/XP on death
4. Collect enough → Level up → Choose upgrade card
5. Difficulty scales over time (hybrid system)
6. Survive until death

**Scaling Philosophy:**
- Player power scales via card upgrades (level-based)
- Enemy difficulty scales over time (time-based)
- Rare cards provide catch-up mechanic for struggling players

---

## Architecture

### Input & Control
```
PlayerController (AMinibonkPlayerController)
├── Adds Input Mapping Context
├── Binds all input actions
├── Handles Move/Jump/Look callbacks
└── Controls the pawn

Character (APlayerCharacter)
├── Camera setup (boom + follow camera)
├── Movement component configuration
├── Components (Health, Movement Stats, Abilities)
└── Physical body only - NO input logic
```

### Component-Based Systems
The game uses reusable components that can be attached to any actor:

| Component | Purpose |
|-----------|---------|
| `HealthComponent` | Health, damage, death events |
| `MovementStatsComponent` | Speed/jump with caps and upgrades |
| `AbilityManagerComponent` | Tracks unlocks, limits, applies cards |

### Ability Card System
```
DataTable (DT_AbilityCards)
    ↓
AbilityCardLibrary (generates scaled cards with variance)
    ↓
AbilityManagerComponent (applies cards, broadcasts to components)
    ↓
Components listen and modify their stats
```

**Card Types:**
- **Upgrade** - Stackable stat boosts (damage, speed, health)
- **Unlock** - One-time abilities (can be locked by curse cards)

**Modifier Types:**
- **Flat** - Adds fixed value (+50 health)
- **Percentage** - Multiplies current value (+10% speed)

**Rare System:**
- Progressive chance: starts at 0.5%, increases by 0.1% per card until rare, then drops
- Rare cards are either boosted buffs (2-3x value) or curse cards
- Acts as catch-up mechanic

---

## Project Structure

```
Source/MiniBonk/
├── Public/
│   ├── Characters/
│   │   ├── PlayerCharacter.h
│   │   ├── MinibonkPlayerController.h
│   │   ├── EnemyCharacter.h
│   │   └── EnemyAIController.h
│   ├── Components/
│   │   ├── HealthComponent.h
│   │   └── MovementStatsComponent.h
│   └── Systems/
│       ├── AbilityTypes.h
│       ├── AbilityMath.h
│       ├── AbilityManagerComponent.h
│       ├── AbilityCardLibrary.h
│       └── AbilityTestSubsystem.h
└── Private/
    └── (mirrors Public)

Content/
├── Data/
│   └── DT_AbilityCards (DataTable)
├── Blueprints/
│   ├── BP_Player
│   ├── BP_Enemy
│   └── BP_GameMode
└── Input/
    ├── IA_Move, IA_Jump, IA_Look
    └── IMC_Default
```

---

## Progress

### Completed
- [x] Project setup with Public/Private structure
- [x] Third-person movement (camera-relative)
- [x] Mouse camera control with pitch limits
- [x] Health component (damage, healing, death events)
- [x] Movement stats component (speed, jump with caps)
- [x] Basic enemy with chase AI
- [x] Enemy contact damage with cooldown
- [x] Death handling (player restarts, enemy destroys)
- [x] Ability card system (DataTable, generation, application)
- [x] Progressive rare card chance
- [x] Flat and percentage modifiers
- [x] Console commands for testing cards

### Next Up
- [ ] Auto-attack (projectile component)
- [ ] Melee attack (slash component)
- [ ] Coin drops on enemy death
- [ ] XP/level-up system
- [ ] Enemy spawner with time-based scaling
- [ ] Card selection UI

### Future
- [ ] More enemy types
- [ ] Boss enemies
- [ ] Interactables (chests, power-ups)
- [ ] HUD (health bar, coin counter, XP bar)
- [ ] Game over screen
- [ ] Audio and VFX

---

## Testing

### Console Commands
Open console with `~` (or F1 if remapped) or use Output Log command field:

```
Ability.Stats       - Show current player stats
Ability.Generate    - Generate 3 random upgrade cards
Ability.Cards       - Show current card choices
Ability.Select 0    - Pick card at index (0, 1, or 2)
```

---

## Code Reference

### Naming Conventions

**C++ Classes**
| Type | Prefix | Example |
|------|--------|---------|
| Actor | A | `AEnemyCharacter` |
| UObject / Component | U | `UHealthComponent` |
| Struct | F | `FGeneratedCard` |
| Enum | E | `EModifierType` |
| Interface | I | `IDamageable` |
| Bool variable | b | `bIsDead` |

**Project Naming**
- Module: `MINIBONK_API`
- Project-specific classes: prefix with `Minibonk` (e.g., `AMinibonkPlayerController`)
- Generic/reusable classes: no prefix (e.g., `APlayerCharacter`, `UHealthComponent`)

**Content Assets**
| Type | Prefix | Example |
|------|--------|---------|
| Blueprint | BP_ | `BP_Enemy` |
| Data Table | DT_ | `DT_AbilityCards` |
| Input Action | IA_ | `IA_Move` |
| Input Mapping Context | IMC_ | `IMC_Default` |
| Material | M_ | `M_Ground` |
| Texture | T_ | `T_Rock_D` |

### Code Standards

```cpp
// Pointers in UPROPERTY
UPROPERTY()
TObjectPtr<UHealthComponent> HealthComponent;

// Validation - logs error but continues
if (ensure(Pointer))
{
    // Use pointer
}

// Categories on everything
UPROPERTY(EditDefaultsOnly, Category = "Combat")
float Damage = 10.f;

// No empty overrides - delete if only calling Super
```

### Common Patterns

**Delegate binding (dynamic):**
```cpp
// In BeginPlay, not constructor
HealthComponent->OnDeath.AddDynamic(this, &AMyActor::HandleDeath);

// Function must be UFUNCTION()
UFUNCTION()
void HandleDeath();
```

**Applying modifiers:**
```cpp
#include "Systems/AbilityMath.h"

// Applies flat or percentage, rounds result, respects cap
CurrentSpeed = AbilityMath::ApplyModifier(CurrentSpeed, ModifierType, Value, MaxSpeed);
```

### Useful Includes

```cpp
#include "Kismet/GameplayStatics.h"      // GetPlayerCharacter, etc.
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SphereComponent.h"
#include "TimerManager.h"
#include "Engine/DataTable.h"
```