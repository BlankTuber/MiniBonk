#include "Components/StoneThrowComponent.h"
#include "Actors/StoneProjectile.h"
#include "Systems/AbilityManagerComponent.h"
#include "Systems/AbilityMath.h"

UStoneThrowComponent::UStoneThrowComponent()
{
	DamageAbilityID = "StoneThrowDamage";
	BaseDamage = 15.f;
	BaseCooldown = 1.2f;
	BaseRange = 1200.f;

	CooldownAbilityID = "StoneThrowCooldown";
	SpeedAbilityID = "StoneThrowSpeed";
	BaseProjectileSpeed = 2000.f;
	CurrentProjectileSpeed = BaseProjectileSpeed;
}

void UStoneThrowComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentProjectileSpeed = BaseProjectileSpeed;

	if (AbilityManager)
	{
		AbilityManager->RegisterLimit(CooldownAbilityID);
		AbilityManager->RegisterLimit(SpeedAbilityID);
	}
}

void UStoneThrowComponent::OnPassiveCardApplied(FName CardAbilityID, EModifierType ModifierType, float Value)
{
	Super::OnPassiveCardApplied(CardAbilityID, ModifierType, Value);

	if (CardAbilityID == CooldownAbilityID)
	{
		if (CurrentCooldown <= MinCooldown)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s: Cooldown already at min (%.2fs)"), *GetName(), MinCooldown);
			return;
		}

		CurrentCooldown = AbilityMath::ApplyModifier(CurrentCooldown, ModifierType, Value);
		CurrentCooldown = FMath::Max(CurrentCooldown, MinCooldown);
		UE_LOG(LogTemp, Log, TEXT("%s: Cooldown changed to %.2fs [Min: %.2fs]"), *GetName(), CurrentCooldown, MinCooldown);

		if (CurrentCooldown <= MinCooldown && AbilityManager)
		{
			AbilityManager->NotifyLimitReached(CooldownAbilityID);
		}

		RestartAttackTimer();
	}
	else if (CardAbilityID == SpeedAbilityID)
	{
		if (CurrentProjectileSpeed >= MaxProjectileSpeed)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s: Projectile speed already at max (%.0f)"), *GetName(), MaxProjectileSpeed);
			return;
		}

		CurrentProjectileSpeed = AbilityMath::ApplyModifier(CurrentProjectileSpeed, ModifierType, Value, MaxProjectileSpeed);
		UE_LOG(LogTemp, Log, TEXT("%s: Projectile speed upgraded to %.0f [Max: %.0f]"), *GetName(), CurrentProjectileSpeed, MaxProjectileSpeed);

		if (CurrentProjectileSpeed >= MaxProjectileSpeed && AbilityManager)
		{
			AbilityManager->NotifyLimitReached(SpeedAbilityID);
		}
	}
}

void UStoneThrowComponent::PerformAttack(const TArray<AActor*>& Targets)
{
	if (!ensure(ProjectileClass))
	{
		UE_LOG(LogTemp, Warning, TEXT("StoneThrowComponent: No ProjectileClass set!"));
		return;
	}

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	FVector OwnerLocation = Owner->GetActorLocation() + SpawnOffset;

	for (AActor* Target : Targets)
	{
		if (!Target)
		{
			continue;
		}

		FVector Direction = (Target->GetActorLocation() - OwnerLocation).GetSafeNormal();

		FVector SpawnLocation = OwnerLocation + (Direction * 100.f);
		FRotator SpawnRotation = Direction.Rotation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = Owner;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AStoneProjectile* Projectile = GetWorld()->SpawnActor<AStoneProjectile>(
			ProjectileClass,
			SpawnLocation,
			SpawnRotation,
			SpawnParams
		);

		if (Projectile)
		{
			Projectile->InitializeProjectile(CurrentDamage, Direction, CurrentProjectileSpeed);
		}
	}
}