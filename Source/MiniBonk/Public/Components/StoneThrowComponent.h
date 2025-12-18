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
	virtual void PerformAttack(const TArray<AActor*>& Targets) override;

	UPROPERTY(EditDefaultsOnly, Category = "Attack|Projectile")
	TSubclassOf<AStoneProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "Attack|Projectile")
	FVector SpawnOffset = FVector(0.f, 0.f, 50.f);
};
