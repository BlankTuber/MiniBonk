#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemySpawnerComponent.generated.h"

class AEnemyCharacter;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MINIBONK_API UEnemySpawnerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEnemySpawnerComponent();

	UFUNCTION(BlueprintCallable, Category = "Spawner")
	void StartSpawning();

	UFUNCTION(BlueprintCallable, Category = "Spawner")
	void StopSpawning();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Spawner")
	TSubclassOf<AEnemyCharacter> EnemyClass;

	UPROPERTY(EditDefaultsOnly, Category = "Spawner|Timing")
	float BaseSpawnInterval = 2.f;

	UPROPERTY(EditDefaultsOnly, Category = "Spawner|Timing")
	float MinSpawnInterval = 0.3f;

	UPROPERTY(EditDefaultsOnly, Category = "Spawner|Timing")
	float SpawnIntervalDecreaseRate = 0.02f;

	UPROPERTY(EditDefaultsOnly, Category = "Spawner|Timing")
	bool bStartSpawningOnBeginPlay = true;

	UPROPERTY(EditDefaultsOnly, Category = "Spawner|Radius")
	float MinSpawnRadius = 600.f;

	UPROPERTY(EditDefaultsOnly, Category = "Spawner|Radius")
	float MaxSpawnRadius = 1500.f;

	UPROPERTY(EditDefaultsOnly, Category = "Spawner|Radius", meta = (ClampMin = "0.1", ClampMax = "3.0"))
	float SpawnDistanceBias = 0.5f;

private:
	FTimerHandle SpawnTimerHandle;
	float CurrentSpawnInterval;
	float ElapsedTime = 0.f;

	void SpawnEnemy();
	void UpdateSpawnInterval();
	FVector GetSpawnLocation() const;
	float GetWeightedRandomRadius() const;
};