#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

class UHealthComponent;
class USphereComponent;
class APlayerCharacter;
class ACoin;

// Basic enemy that chases and damages player on contact
UCLASS()
class MINIBONK_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemyCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	TObjectPtr<UHealthComponent> HealthComponent;

	// Collision sphere for dealing damage to player
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<USphereComponent> DamageSphere;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float ContactDamage = 10.f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float DamageCooldown = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Loot")
	TSubclassOf<ACoin> CoinClass;

	UPROPERTY(EditDefaultsOnly, Category = "Loot")
	int32 MinCoinDrop = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Loot")
	int32 MaxCoinDrop = 3;

private:
	// Overlap callbacks
	UFUNCTION()
	void OnDamageSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnDamageSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	FTimerHandle DamageTimerHandle;
	TWeakObjectPtr<APlayerCharacter> OverlappingPlayer;

	void DealDamageToPlayer();

	UFUNCTION()
	void HandleDeath();
};