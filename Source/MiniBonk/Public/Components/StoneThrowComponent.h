#pragma once

#include "CoreMinimal.h"
#include "Components/AutoAttackComponent.h"
#include "StoneThrowComponent.generated.h"

class AStoneProjectile;

UCLASS()
class MINIBONK_API UStoneThrowComponent : public UAutoAttackComponent
{
	GENERATED_BODY()

public:
	UStoneThrowComponent();

protected:
	virtual void BeginPlay() override;
	virtual void PerformAttack(const TArray<AActor*>& Targets) override;
	virtual void OnPassiveCardApplied(FName CardAbilityID, EModifierType ModifierType, float Value) override;

	UPROPERTY(EditDefaultsOnly, Category = "Attack|Ability IDs")
	FName CooldownAbilityID;

	UPROPERTY(EditDefaultsOnly, Category = "Attack|Ability IDs")
	FName SpeedAbilityID;

	UPROPERTY(EditDefaultsOnly, Category = "Attack|Projectile")
	float BaseProjectileSpeed = 2000.f;

	UPROPERTY(VisibleAnywhere, Category = "Attack|Current Stats")
	float CurrentProjectileSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "Attack|Limits")
	float MinCooldown = 0.2f;

	UPROPERTY(EditDefaultsOnly, Category = "Attack|Limits")
	float MaxProjectileSpeed = 5000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Attack|Projectile")
	TSubclassOf<AStoneProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "Attack|Projectile")
	FVector SpawnOffset = FVector(0.f, 0.f, 50.f);
};