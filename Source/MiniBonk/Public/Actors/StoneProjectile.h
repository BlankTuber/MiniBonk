#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StoneProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class MINIBONK_API AStoneProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AStoneProjectile();

	void InitializeProjectile(float InDamage, const FVector& Direction);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UProjectileMovementComponent> MovementComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float Speed = 2000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float Lifespan = 3.f;

private:
	float Damage = 0.f;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
