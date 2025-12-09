#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

// Simple AI that chases the player
UCLASS()
class MINIBONK_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	// How often to recalculate path to player
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float PathUpdateInterval = 0.3f;

	// How close to get before stopping
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float ContactDistance = 35.f;

	float TimeSinceLastPathUpdate = 0.f;

	void UpdatePathToPlayer();
};