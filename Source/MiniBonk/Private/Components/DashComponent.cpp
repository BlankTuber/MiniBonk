#include "Components/DashComponent.h"
#include "Systems/AbilityManagerComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Characters/EnemyCharacter.h"
#include "Components/HealthComponent.h"
#include "Kismet/GameplayStatics.h"

UDashComponent::UDashComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UDashComponent::BeginPlay()
{
	Super::BeginPlay();

	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (ensure(OwnerCharacter))
	{
		MovementComponent = OwnerCharacter->GetCharacterMovement();
		AbilityManager = OwnerCharacter->FindComponentByClass<UAbilityManagerComponent>();
	}
}

void UDashComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bIsDashing)
	{
		return;
	}

	// Check for enemies to damage
	CheckForEnemies();

	// Check if dash has ended
	if (GetWorld()->GetTimeSeconds() >= DashEndTime)
	{
		EndDash();
	}
}

float UDashComponent::GetCurrentCooldown() const
{
	float Cooldown = BaseCooldown;

	if (AbilityManager)
	{
		Cooldown = AbilityManager->CalculateActiveAbilityValue(CooldownAbilityID, BaseCooldown);
	}

	return FMath::Max(Cooldown, MinCooldown);
}

float UDashComponent::GetCurrentDistance() const
{
	float Distance = BaseDistance;

	if (AbilityManager)
	{
		Distance = AbilityManager->CalculateActiveAbilityValue(DistanceAbilityID, BaseDistance);
	}

	return FMath::Min(Distance, MaxDistance);
}

float UDashComponent::GetCurrentDamage() const
{
	if (AbilityManager)
	{
		return AbilityManager->CalculateActiveAbilityValue(DamageAbilityID, BaseDamage);
	}

	return BaseDamage;
}

bool UDashComponent::TryDash(const FVector& Direction)
{
	if (!CanDash())
	{
		return false;
	}

	PerformDash(Direction);
	return true;
}

bool UDashComponent::CanDash() const
{
	if (!MovementComponent)
	{
		return false;
	}

	if (bIsDashing)
	{
		return false;
	}

	float CurrentTime = GetWorld()->GetTimeSeconds();
	return (CurrentTime - LastDashTime) >= GetCurrentCooldown();
}

float UDashComponent::GetCooldownRemaining() const
{
	float CurrentTime = GetWorld()->GetTimeSeconds();
	float TimeSinceDash = CurrentTime - LastDashTime;
	return FMath::Max(0.f, GetCurrentCooldown() - TimeSinceDash);
}

float UDashComponent::GetCooldownPercent() const
{
	float Cooldown = GetCurrentCooldown();
	if (Cooldown <= 0.f)
	{
		return 1.f;
	}

	float Remaining = GetCooldownRemaining();
	return 1.f - (Remaining / Cooldown);
}

void UDashComponent::PerformDash(const FVector& Direction)
{
	if (!ensure(MovementComponent))
	{
		return;
	}

	LastDashTime = GetWorld()->GetTimeSeconds();

	if (DashSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DashSound, GetOwner()->GetActorLocation());
	}

	// Get current stats from ability manager
	float CurrentDistance = GetCurrentDistance();
	CurrentDashDamage = GetCurrentDamage();

	// Calculate velocity needed to travel CurrentDistance in DashDuration
	FVector DashDirection = Direction.GetSafeNormal();
	if (DashDirection.IsNearlyZero())
	{
		// Default to forward if no direction provided
		DashDirection = GetOwner()->GetActorForwardVector();
	}

	float DashSpeed = CurrentDistance / DashDuration;
	FVector DashVelocity = DashDirection * DashSpeed;

	// Apply the dash as a velocity change
	MovementComponent->Velocity = DashVelocity;

	// Start dash state
	bIsDashing = true;
	DashEndTime = GetWorld()->GetTimeSeconds() + DashDuration;
	HitEnemiesThisDash.Empty();

	// Enable tick to check for enemies during dash
	SetComponentTickEnabled(true);

	UE_LOG(LogTemp, Log, TEXT("DashComponent: Dashed %.0f units, %.0f damage"), CurrentDistance, CurrentDashDamage);
}

void UDashComponent::EndDash()
{
	bIsDashing = false;
	HitEnemiesThisDash.Empty();
	SetComponentTickEnabled(false);
}

void UDashComponent::CheckForEnemies()
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	FVector OwnerLocation = Owner->GetActorLocation();

	// Find all enemies in damage radius
	TArray<AActor*> AllEnemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyCharacter::StaticClass(), AllEnemies);

	for (AActor* Enemy : AllEnemies)
	{
		if (!Enemy)
		{
			continue;
		}

		// Skip if already hit this dash
		if (HitEnemiesThisDash.Contains(Enemy))
		{
			continue;
		}

		float Distance = FVector::Dist(OwnerLocation, Enemy->GetActorLocation());
		if (Distance <= DamageRadius)
		{
			// Deal damage
			AEnemyCharacter* EnemyChar = Cast<AEnemyCharacter>(Enemy);
			if (EnemyChar && EnemyChar->HealthComponent)
			{
				EnemyChar->HealthComponent->TakeDamage(CurrentDashDamage);
				HitEnemiesThisDash.Add(Enemy);

				UE_LOG(LogTemp, Log, TEXT("DashComponent: Hit %s for %.0f damage"), *Enemy->GetName(), CurrentDashDamage);
			}
		}
	}
}