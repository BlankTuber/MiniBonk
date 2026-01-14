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
├── Binds all input actions (Move, Jump, Look, Dash)
├── Handles Move/Jump/Look/Dash callbacks
├── Creates and manages HUD widget
└── Controls the pawn

Character (APlayerCharacter)
├── Camera setup (boom + follow camera)
├── Movement component configuration
├── Components (Health, Movement Stats, Abilities, Dash)
└── Physical body only - NO input logic
```

### Component-Based Systems
The game uses reusable components that can be attached to any actor:

| Component | Purpose |
|-----------|---------|
| `HealthComponent` | Health, damage, death events |
| `MovementStatsComponent` | Speed/jump with caps and upgrades |
| `AbilityManagerComponent` | Tracks unlocks, limits, applies cards |
| `LevelComponent` | XP tracking, level ups, pauses for card selection |
| `CoinComponent` | Coin collection, magnet radius |
| `AutoAttackComponent` | Base class for automatic attacks |
| `StoneThrowComponent` | Projectile auto-attack |
| `DashComponent` | Player-activated dash attack |

### UI System
```
MinibonkPlayerController
    ↓ creates
MinibonkHUD (WBP_MinibonkHUD)
├── Manages all UI elements
├── Holds reference to CardDataTable
├── Listens to LevelComponent::OnLevelUp
└── Contains:
    └── CardSelectionWidget (WBP_CardSelection)
        ├── 3 card buttons with name/description/value
        ├── Broadcasts OnCardSelected when clicked
        └── C++ handles logic, Blueprint handles layout
```

**UI Pattern:**
- C++ base classes (`UUserWidget` subclasses) define logic and `BindWidget` references
- Blueprint widgets derive from C++ and handle visual layout/styling
- `meta = (BindWidget)` connects Blueprint UI elements to C++ variables by name

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

**Ability Types:**
- **Passive** - Applied immediately when collected (broadcasts `OnPassiveCardApplied`)
- **Auto** - Same as Passive
- **Active** - Stored for runtime calculation via `CalculateActiveAbilityValue()`

**Modifier Types:**
- **Flat** - Adds fixed value (+50 health)
- **Percentage** - Multiplies current value (+10% speed)

**Rare System:**
- Progressive chance: starts at 0.5%, increases by 0.1% per card until rare, then drops
- Rare cards are either boosted buffs (2-3x value) or curse cards
- Acts as catch-up mechanic

**Scaling Systems:**
- **XP per coin:** `CoinValue * XPPerCoin * (1.005 ^ ElapsedSeconds)`
- **XP to level:** `5 * (1.4 ^ (Level - 1))` - exponential growth
- **Coin value:** `1 + (ElapsedMinutes * 1.0)` - linear growth
- **Enemy HP:** `BaseHP * (1.003 ^ ElapsedSeconds)` - exponential growth
- **Enemy Damage:** `BaseDamage * (1 + ElapsedMinutes * 0.2)` - linear growth
- **Spawn Rate:** Interval decreases over time (see EnemySpawnerComponent)

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
- [x] Auto-attack (projectile component)
- [x] Coin drops on enemy death
- [x] Enemy spawner with time-based scaling
- [x] XP/level-up system
- [x] Enemy HP/damage scaling over time
- [x] Card selection UI
- [x] HUD (health bar, coin counter, XP bar)
- [x] Dash attack (activated ability with damage)

### Next Up
- [ ] Audio / sfx
- [ ] Simple animations
- [ ] Simple vfx

### Future
- [ ] Melee attack (slash component)
- [ ] More enemy types
- [ ] Boss enemies
- [ ] Interactables (chests, power-ups)
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
Ability.Unpause     - Unpause game (if stuck)
```

---

## Code Reference

### Naming Conventions

**C++ Classes**
| Type | Prefix | Example |
|------|--------|---------|
| Actor | A | `AEnemyCharacter` |
| UObject / Component | U | `UHealthComponent` |
| Widget | U | `UCardSelectionWidget` |
| Struct | F | `FGeneratedCard` |
| Enum | E | `EModifierType` |
| Interface | I | `IDamageable` |
| Bool variable | b | `bIsDead` |

**Project Naming**
- Module: `MINIBONK_API`
- Project-specific classes: prefix with `Minibonk` (e.g., `AMinibonkPlayerController`, `UMinibonkHUD`)
- Generic/reusable classes: no prefix (e.g., `APlayerCharacter`, `UHealthComponent`)

**Content Assets**
| Type | Prefix | Example |
|------|--------|---------|
| Blueprint | BP_ | `BP_Enemy` |
| Widget Blueprint | WBP_ | `WBP_CardSelection` |
| Data Table | DT_ | `DT_AbilityCards` |
| Input Action | IA_ | `IA_Move` |
| Input Mapping Context | IMC_ | `IMC_Default` |
| Material | M_ | `M_Ground` |
| Texture | T_ | `T_Rock_D` |

**DataTable Row Names (Ability Cards)**
| Card Type | Pattern | Example |
|-----------|---------|---------|
| Flat upgrade | `{AbilityID}_Flat` | `DashDamage_Flat` |
| Percentage upgrade | `{AbilityID}_Percent` | `DashDamage_Percent` |
| Curse (flat benefit) | `{AbilityID}_Curse_{CurseTarget}` | `DashDamage_Curse_MaxHealth` |
| Curse (percent benefit) | `{AbilityID}_CursePercent_{CurseTarget}` | `DashDamage_CursePercent_MaxHealth` |
| Unlock | `{UnlockID}_Unlock` | `DoubleJump_Unlock` |

### Ability IDs Reference

**Passive Abilities** (apply immediately via `OnPassiveCardApplied`):
| Ability ID | Component | Effect |
|------------|-----------|--------|
| `MaxHealth` | HealthComponent | Increases max health |
| `MoveSpeed` | MovementStatsComponent | Increases movement speed |
| `JumpHeight` | MovementStatsComponent | Increases jump velocity |
| `MagnetRadius` | CoinComponent | Increases coin pickup range |
| `StoneThrowDamage` | StoneThrowComponent | Increases projectile damage |
| `StoneThrowCooldown` | StoneThrowComponent | Reduces attack cooldown |
| `StoneThrowSpeed` | StoneThrowComponent | Increases projectile speed |

**Active Abilities** (calculated at use-time via `CalculateActiveAbilityValue`):
| Ability ID | Component | Effect |
|------------|-----------|--------|
| `DashDamage` | DashComponent | Increases dash attack damage |
| `DashCooldown` | DashComponent | Reduces dash cooldown |
| `DashDistance` | DashComponent | Increases dash distance |

### Code Standards
```cpp
// Pointers in UPROPERTY
UPROPERTY()
TObjectPtr<UHealthComponent> HealthComponent;

// Widget binding - Blueprint must have widget with matching name
UPROPERTY(meta = (BindWidget))
TObjectPtr<UButton> Card0Button;

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

**Applying modifiers (Passive abilities):**
```cpp
#include "Systems/AbilityMath.h"

// Applies flat or percentage, rounds result, respects cap
CurrentSpeed = AbilityMath::ApplyModifier(CurrentSpeed, ModifierType, Value, MaxSpeed);
```

**Calculating active ability values:**
```cpp
// Get value at use-time from AbilityManager
float Damage = AbilityManager->CalculateActiveAbilityValue(DamageAbilityID, BaseDamage);
```

**Creating UMG widgets in C++:**
```cpp
// In PlayerController or similar
HUDWidget = CreateWidget<UMinibonkHUD>(this, HUDWidgetClass);
HUDWidget->AddToViewport();
```

### Useful Includes
```cpp
#include "Kismet/GameplayStatics.h"      // GetPlayerCharacter, etc.
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SphereComponent.h"
#include "TimerManager.h"
#include "Engine/DataTable.h"
#include "Blueprint/UserWidget.h"        // For UMG widgets
#include "Components/Button.h"           // UMG Button
#include "Components/TextBlock.h"        // UMG TextBlock
```