# MiniBonk

A survivor-like roguelite prototype inspired by MegaBonk (2025). Built in Unreal Engine 5 with C++ as a learning project.

## Tech Stack

- **Engine:** Unreal Engine 5 (5.7)
- **Language:** C++
- **IDE:** Visual Studio
- **Version Control:** GitHub

---

## Game Overview

Auto-attack roguelite where the player navigates a small 3D world while enemies spawn in waves. Collect coins from enemies and the environment, level up, choose upgrades, and survive as long as possible.

**Core Loop:**

1. Enemies spawn and chase player
2. Player auto-attacks nearby enemies
3. Enemies drop coins on death
4. Collect coins → level up → choose upgrade
5. Difficulty scales over time
6. Survive until death or defeat boss

---

## Game Systems

### Player

| Feature | Description |
|---------|-------------|
| Movement | Simple Third Person movement. Speed/jump height modifiable via upgrades |
| Health | Numeric HP system. Death when HP < 1. Upgradeable |
| Auto-Attack | Fires automatically. Types: long-range projectile, close-range slash |
| Special Pickups | Temporary power boosts (nuke/instakill style) |

### Enemies

| Feature | Description |
|---------|-------------|
| Types | 2 regular enemy types + 1 mini-boss for MVP |
| Behavior | Chasers. Mini-boss has ranged attacks |
| Scaling | Health increases over time |

### Spawning

| Feature | Description |
|---------|-------------|
| Location | Outside a minimum range from player. Closer = higher spawn probability, farther = lower but possible |
| Difficulty | Time-based ramp up |

### Progression

| Feature | Description |
|---------|-------------|
| Coin Drops | Varies by enemy type + randomness factor. Intentionally hard to keep up with difficulty |
| Pickup Radius | Upgradeable |
| Level-Up Cost | Scaling (more coins needed per level) |
| Upgrade Selection | Spin wheel with 3 random options. Upgrades stack (e.g., ATK +5% → ATK +10% with two). Chance for "critical" rolls (double upgrade or rare option) |

### Interactables

| Feature | Description |
|---------|-------------|
| Chests | Same rewards as level-ups but cost coins. Price ramps with difficulty. Existing chests keep original price |
| Power-Up Drops | Heal, speed boost, mega-coin (MVP) |

### World

| Feature | Description |
|---------|-------------|
| Maps | 1 map for MVP |
| Size | Large enough to navigate/kite enemies |
| Boundaries | Walls. Max jump height capped to prevent escaping |

### Game Loop

| State | Description |
|-------|-------------|
| Start | Menu screen. Spend excess coins on permanent upgrades |
| Win Condition | Kill boss (optional) or endless survival |
| Lose Condition | Player death (HP < 1) |
| Post-Game | Excess coins added to separate persistent counter for menu upgrades |

### UI

- Health bar
- Coin counter
- Level / XP progress bar
- Upgrade selection screen (spin wheel)
- Menu for permanent upgrades

---



## Setup / Infrastructure

- [ ] Project created
- [ ] File structure (Public/Private folders)
- [ ] GitHub repo initialized

---

## Player System

**Movement**

- [ ] Basic movement
- [ ] Speed modifier (for upgrades)
- [ ] Jump height modifier (for upgrades)

**Health**

- [ ] Health component/variable
- [ ] Take damage function
- [ ] Death state
- [ ] Health upgradeable via progression

**Auto-Attack**

- [ ] Auto-attack timer/interval
- [ ] Close-range slash attack
- [ ] Long-range projectile attack
- [ ] Damage enemies in range
- [ ] Attack speed upgradeable
- [ ] Attack damage upgradeable
- [ ] Attack range upgradeable

---

## Enemy System

**Enemy Type 1 (Basic Chaser)**

- [ ] Character class created
- [ ] AI Controller created
- [ ] Chase behavior (moves toward player)
- [ ] Health variable
- [ ] Take damage function
- [ ] Death (destroy actor)
- [ ] Drop coins on death

**Enemy Type 2 (Chaser Variant)**

- [ ] Character class
- [ ] Different stats (speed/health/damage)
- [ ] Visual distinction
- [ ] Drop coins on death

**Mini-Boss**

- [ ] Character class
- [ ] Higher health pool
- [ ] Chase behavior
- [ ] Ranged attack ability
- [ ] Drop more coins on death
- [ ] Spawn trigger (time-based or kill-based)

**Enemy Damage**

- [ ] Enemies damage player on contact
- [ ] Damage cooldown (no instant kill)

---

## Spawning System

- [ ] Spawn manager actor
- [ ] Spawn outside minimum range from player
- [ ] Spawn probability (closer = higher chance)
- [ ] Time-based spawn rate
- [ ] Difficulty scaling (more enemies over time)
- [ ] Enemy type selection (weighted random)
- [ ] Mini-boss spawn trigger

---

## Economy System

**Coins**

- [ ] Coin actor/pickup class
- [ ] Coins spawn on enemy death
- [ ] Coin value varies by enemy type
- [ ] Random variance on drop amount
- [ ] Player can collect coins (overlap)
- [ ] Coin counter variable

**Pickup Radius**

- [ ] Base pickup radius
- [ ] Upgradeable pickup radius

---

## Progression System

**Leveling**

- [ ] XP/coin threshold for level up
- [ ] Scaling threshold (more coins per level)
- [ ] Level up event/trigger

**Upgrade Selection**

- [ ] Upgrade selection UI (3 options)
- [ ] Random upgrade pool
- [ ] Stackable upgrades (ATK +5% → +10%)
- [ ] Apply selected upgrade to player

**Upgrade Types (minimum 5)**

- [ ] Attack damage increase
- [ ] Attack speed increase
- [ ] Attack range increase
- [ ] Movement speed increase
- [ ] Max health increase
- [ ] Pickup radius increase
- [ ] (Optional) Critical roll chance for rare/double upgrades

---

## Interactables

**Chests**

- [ ] Chest actor class
- [ ] Interaction prompt
- [ ] Cost coins to open
- [ ] Give random upgrade on open
- [ ] Price scales with difficulty
- [ ] Existing chests keep original price

**Power-Up Drops**

- [ ] Heal drop (restore HP)
- [ ] Speed boost drop (temporary)
- [ ] Mega-coin drop (large coin value)
- [ ] (Polish) Nuke/instakill drop

---

## World / Level

- [ ] Playable area designed
- [ ] Boundary walls
- [ ] Floor with nav mesh coverage
- [ ] Chest spawn locations
- [ ] Visual landmarks (optional)

---

## UI

- [ ] Health bar display
- [ ] Coin counter display
- [ ] Level/XP progress bar
- [ ] Upgrade selection screen
- [ ] Game over screen
- [ ] (Secondary) Start menu
- [ ] (Secondary) Permanent upgrade menu

---

## Game Loop / State

**Game States**

- [ ] Playing state
- [ ] Paused state (upgrade selection)
- [ ] Game over state

**Start**

- [ ] (Secondary) Start menu
- [ ] (Secondary) Persistent currency display
- [ ] (Secondary) Permanent upgrades purchase

**End**

- [ ] Detect player death
- [ ] Show game over screen
- [ ] (Secondary) Add excess coins to persistent currency
- [ ] Restart option

**Win Condition (Optional)**

- [ ] Boss spawn trigger
- [ ] Boss defeat = win
- [ ] Win screen

---

## Polish / Juice

**Audio**

- [ ] Hit sounds
- [ ] Enemy death sounds
- [ ] Coin pickup sound
- [ ] Level-up jingle
- [ ] Background music

**Visual Effects**

- [ ] Screen shake on hit
- [ ] Damage numbers
- [ ] Enemy death particles
- [ ] Level-up effect
- [ ] Upgrade selection animation (spin wheel)

---

## UE5 C++ Quick Reference

---

### Getting References

```cpp
// Player
APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

// Component on an actor
UMyComponent* Comp = MyActor->FindComponentByClass<UMyComponent>();

// Game mode
AMyGameMode* GM = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
```

---

### Logging

```cpp
UE_LOG(LogTemp, Warning, TEXT("Message here"));
UE_LOG(LogTemp, Warning, TEXT("Value: %f"), SomeFloat);
UE_LOG(LogTemp, Warning, TEXT("Name: %s"), *SomeString);

// On-screen (good for runtime debugging)
GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Hello"));
```

---

### Timers

```cpp
// Header
FTimerHandle MyTimerHandle;

// Repeating timer
GetWorldTimerManager().SetTimer(MyTimerHandle, this, &AMyClass::MyFunction, 1.0f, true);

// One-shot timer
GetWorldTimerManager().SetTimer(MyTimerHandle, this, &AMyClass::MyFunction, 1.0f, false);

// Stop timer
GetWorldTimerManager().ClearTimer(MyTimerHandle);
```

---

### Random

```cpp
float RandomFloat = FMath::RandRange(1.f, 10.f);
int32 RandomInt = FMath::RandRange(1, 10);  // Inclusive

// Chance (30%)
if (FMath::FRand() < 0.3f) { }
```

---

### Distance

```cpp
float Dist = FVector::Dist(GetActorLocation(), Other->GetActorLocation());
```

---

### Casting

```cpp
AMyCharacter* Player = Cast<AMyCharacter>(OtherActor);
if (Player)
{
    // Safe to use
}
```

---

### Overlap Event Signature

```cpp
// Header
UFUNCTION()
void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
        bool bFromSweep, const FHitResult& SweepResult);

// Bind in BeginPlay
MyCollision->OnComponentBeginOverlap.AddDynamic(this, &AMyClass::OnOverlapBegin);
```

---

### UPROPERTY Cheat Sheet

| You want... | Use |
|-------------|-----|
| Editable everywhere | `EditAnywhere` |
| Editable only in Blueprint defaults | `EditDefaultsOnly` |
| Visible but not editable | `VisibleAnywhere` |
| Blueprint can read | `BlueprintReadOnly` |
| Blueprint can read/write | `BlueprintReadWrite` |
| Class reference (for spawning) | `TSubclassOf<AMyClass>` |

```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
float MaxHealth = 100.f;
```

---

### UFUNCTION Cheat Sheet

| You want... | Use |
|-------------|-----|
| Blueprint can call it | `BlueprintCallable` |
| Blueprint can override it | `BlueprintNativeEvent` |
| No side effects (getter) | `BlueprintPure` |

---

### Debug Drawing

```cpp
#include "DrawDebugHelpers.h"

DrawDebugSphere(GetWorld(), Location, Radius, 12, FColor::Red, false, 0.1f);
DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 0.1f);
```

---

### Common Includes

```cpp
#include "Kismet/GameplayStatics.h"     // Most utility functions
#include "Kismet/KismetMathLibrary.h"   // Extra math
#include "TimerManager.h"
#include "DrawDebugHelpers.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
```
