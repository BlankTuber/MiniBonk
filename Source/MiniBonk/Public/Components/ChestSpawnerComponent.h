#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ChestSpawnerComponent.generated.h"

class AChest;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MINIBONK_API UChestSpawnerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UChestSpawnerComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditDefaultsOnly, Category = "Spawner")
	TSubclassOf<AChest> ChestClass;

	UPROPERTY(EditDefaultsOnly, Category = "Spawner|Kills")
	int32 BaseKillsRequired = 10;

	UPROPERTY(EditDefaultsOnly, Category = "Spawner|Kills")
	float KillScalingRate = 1.25f;

	UPROPERTY(EditDefaultsOnly, Category = "Spawner|Location")
	float MinSpawnRadius = 500.f;

	UPROPERTY(EditDefaultsOnly, Category = "Spawner|Location")
	float MaxSpawnRadius = 2000.f;

private:
	int32 KillsSinceLastChest = 0;
	int32 ChestsSpawned = 0;

	UFUNCTION()
	void OnEnemyKilled();

	void SpawnChest();
	int32 GetCurrentKillsRequired() const;
	FVector GetSpawnLocation() const;
};