#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "KillTrackingSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnemyKilledSignature);

UCLASS()
class MINIBONK_API UKillTrackingSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Kills")
	FOnEnemyKilledSignature OnEnemyKilled;

	UFUNCTION(BlueprintCallable, Category = "Kills")
	void NotifyEnemyKilled();

	UFUNCTION(BlueprintPure, Category = "Kills")
	int32 GetTotalKills() const { return TotalKills; }

private:
	int32 TotalKills = 0;
};