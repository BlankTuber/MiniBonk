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
5. Find chests → Spend coins → Get additional cards
6. Difficulty scales over time (hybrid system)
7. Survive until death

**Scaling Philosophy:**
- Player power scales via card upgrades (level-based)
- Enemy difficulty scales over time (time-based)
- Chest costs scale with player level
- Rare cards provide catch-up mechanic for struggling players

---

## Architecture

### Input & Control
```
PlayerController (AMinibonkPlayerController)
├── Adds Input Mapping Context
├── Binds all input actions (Move, Jump, Look, Dash, Interact)
├── Handles Move/Jump/Look/Dash/Interact callbacks
├── Creates and manages HUD widget
└── Controls the pawn

Character (APlayerCharacter)
├── Camera setup (boom + follow camera)
├── Movement component configuration
├── Components (Health, Movement Stats, Abilities, Dash)
├── CurrentInteractable (tracks nearby interactable actor)
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
| `CoinComponent` | Coin collection, spending, magnet radius |
| `AutoAttackComponent` | Base class for automatic attacks |
| `StoneThrowComponent` | Projectile auto-attack |
| `DashComponent` | Player-activated dash attack |
| ChestSpawnerComponent | Kill-based chest spawning with scaling |

### Interactables
```
Chest (AChest)
├── InteractionSphere (detects player proximity)
├── InteractionLight (visual feedback: green/red based on affordability)
├── Cost scales with player level: BaseCost * (GrowthRate ^ (Level - 1))
├── On purchase: deducts coins, triggers card selection
└── Updates HUD interaction prompt when player nearby
```

**Interaction Flow:**
1. Player enters chest's InteractionSphere
2. Chest registers itself as player's CurrentInteractable
3. Chest shows HUD prompt and enables light
4. Chest binds to CoinComponent::OnCoinsChanged to update affordability in real-time
5. Player presses Interact (E) → PlayerController calls Chest::TryPurchase()
6. On success: spend coins, pause game, trigger card selection, destroy chest

### Subsystems
```
KillTrackingSubsystem (UGameInstanceSubsystem)
├── Tracks total enemy kills across the game
├── Broadcasts OnEnemyKilled delegate
└── Other systems (ChestSpawner, future stats/achievements) listen to this
```

### Animation System
```
Animation Blueprints (ABP_)
├── ABP_Player
│   ├── Event Graph: Updates Speed and bIsFalling variables
│   └── Anim Graph: State Machine (Locomotion)
│       ├── Idle/Move state → BS_Player_Locomotion
│       └── Jump state → Jump animation
│
└── ABP_Enemy
    ├── Event Graph: Updates Speed variable
    └── Anim Graph: BS_Enemy_Locomotion (direct, no state machine)

Blend Spaces (BS_)
├── BS_Player_Locomotion (1D: Speed 0-500)
│   ├── 0: Idle
│   ├── 150: Walk
│   └── 500: Run
│
└── BS_Enemy_Locomotion (1D: Speed 0-450)
    ├── 0: Idle
    ├── 150: Walk
    └── 450: Run
```

### UI System
```
MinibonkPlayerController
    ↓ creates
MinibonkHUD (WBP_MinibonkHUD)
├── Health bar, coin counter, XP bar, level text
├── Interaction prompt (shows chest cost, green/red color)
├── Holds reference to CardDataTable
├── Listens to LevelComponent::OnLevelUp
├── Exposes TriggerCardSelection(Title) for external triggers (chests)
└── Contains:
    └── CardSelectionWidget (WBP_CardSelection)
        ├── 3 card buttons with name/description/value
        ├── Title text (shows "Level Up!" or "Chest Reward")
        ├── Broadcasts OnCardSelected when clicked
        └── C++ handles logic, Blueprint handles layout
```

**Required Widget Bindings (WBP_MinibonkHUD):**
- `CardSelectionWidget` - UCardSelectionWidget
- `HealthBar` - UProgressBar
- `CoinText` - UTextBlock
- `XPBar` - UProgressBar
- `LevelText` - UTextBlock
- `InteractionPromptText` - UTextBlock

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
- **Chest cost:** `25 * (1.3 ^ (Level - 1))` - exponential growth
- **Enemy HP:** `BaseHP * (1.0125 ^ ElapsedSeconds)` - exponential growth
- **Enemy Damage:** `BaseDamage * (1 + ElapsedMinutes * 0.2)` - linear growth
- **Spawn Rate:** Interval decreases over time (see EnemySpawnerComponent)
- **Chest spawn kills:** `10 * (1.25 ^ ChestsSpawned)` - exponential growth

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
- [x] Player animations (locomotion + jump)
- [x] Enemy animations (locomotion)
- [x] Simple vfx
- [x] Chest interactable (purchase card rolls with coins)
- [x] Kill tracking subsystem
- [x] Chest spawner (kill-based with scaling)
- [X] Audio / sfx
- [X] Map
- [X] New attack: Orbital stones
- [X] Boss enemies

### Next Up
- [ ] Start Screen
- [ ] Game over screen

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
| Actor | A | `AEnemyCharacter`, `AChest` |
| UObject / Component | U | `UHealthComponent` |
| Widget | U | `UCardSelectionWidget` |
| Struct | F | `FGeneratedCard` |
| Enum | E | `EModifierType` |
| Interface | I | `IDamageable` |
| Bool variable | b | `bIsDead` |

**Project Naming**
- Module: `MINIBONK_API`
- Project-specific classes: prefix with `Minibonk` (e.g., `AMinibonkPlayerController`, `UMinibonkHUD`)
- Generic/reusable classes: no prefix (e.g., `APlayerCharacter`, `UHealthComponent`, `AChest`)

**Content Assets**
| Type | Prefix | Example |
|------|--------|---------|
| Blueprint | BP_ | `BP_Enemy`, `BP_Chest` |
| Widget Blueprint | WBP_ | `WBP_CardSelection` |
| Data Table | DT_ | `DT_AbilityCards` |
| Input Action | IA_ | `IA_Move`, `IA_Interact` |
| Input Mapping Context | IMC_ | `IMC_Default` |
| Material | M_ | `M_Ground` |
| Texture | T_ | `T_Rock_D` |
| Animation Blueprint | ABP_ | `ABP_Player` |
| Blend Space | BS_ | `BS_Player_Locomotion` |

### Input Actions Reference
| Action | Key | Purpose |
|--------|-----|---------|
| `IA_Move` | WASD | Movement |
| `IA_Look` | Mouse | Camera control |
| `IA_Jump` | Space | Jump |
| `IA_Dash` | Shift | Dash attack |
| `IA_Interact` | E | Interact with chests etc. |

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

**Triggering card selection from external source:**
```cpp
AMinibonkPlayerController* PC = Cast<AMinibonkPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
if (PC)
{
    UMinibonkHUD* HUD = PC->GetHUDWidget();
    if (HUD)
    {
        UGameplayStatics::SetGamePaused(GetWorld(), true);
        HUD->TriggerCardSelection(FText::FromString(TEXT("My Title")));
    }
}
```

**Interaction pattern (for new interactables):**
```cpp
// In your interactable actor:
void AMyInteractable::OnOverlapBegin(...)
{
    APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
    if (Player)
    {
        Player->CurrentInteractable = this;
        // Show UI prompt, visual feedback
    }
}

void AMyInteractable::OnOverlapEnd(...)
{
    APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
    if (Player && Player->CurrentInteractable == this)
    {
        Player->CurrentInteractable = nullptr;
        // Hide UI prompt, visual feedback
    }
}

// Implement a TryInteract() or similar that PlayerController calls
```

### Useful Includes
```cpp
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/PointLightComponent.h"
#include "TimerManager.h"
#include "Engine/DataTable.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
```