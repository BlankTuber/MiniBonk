#include "Components/StoneThrowComponent.h"
#include "Actors/StoneProjectile.h"

UStoneThrowComponent::UStoneThrowComponent()
{
	AbilityID = "StoneThrow";

	BaseDamage = 15.f;
	BaseCooldown = 1.2f;
	BaseRange = 1200.f;
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
			Projectile->InitializeProjectile(CurrentDamage, Direction);
		}
	}
}