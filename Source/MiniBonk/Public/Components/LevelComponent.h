#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LevelComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLevelUpSignature, int32, NewLevel, int32, XPForNextLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnXPChangedSignature, int32, CurrentXP, int32, XPForNextLevel, int32, CurrentLevel);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MINIBONK_API ULevelComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULevelComponent();

	UPROPERTY(BlueprintAssignable, Category = "Level")
	FOnLevelUpSignature OnLevelUp;

	UPROPERTY(BlueprintAssignable, Category = "Level")
	FOnXPChangedSignature OnXPChanged;

	UFUNCTION(BlueprintCallable, Category = "Level")
	void AddXP(int32 Amount);

	UFUNCTION(BlueprintPure, Category = "Level")
	int32 GetCurrentLevel() const { return CurrentLevel; }

	UFUNCTION(BlueprintPure, Category = "Level")
	int32 GetCurrentXP() const { return CurrentXP; }

	UFUNCTION(BlueprintPure, Category = "Level")
	int32 GetXPForNextLevel() const;

	UFUNCTION(BlueprintPure, Category = "Level")
	float GetLevelProgressPercent() const;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 CurrentLevel = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 CurrentXP = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Level|Scaling")
	int32 BaseXPRequirement = 5;

	UPROPERTY(EditDefaultsOnly, Category = "Level|Scaling")
	float GrowthRate = 1.4f;

	UPROPERTY(EditDefaultsOnly, Category = "Level|Scaling")
	float XPPerCoin = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Level|Scaling", meta = (ClampMin = "0.001", ClampMax = "0.05"))
	float XPGrowthPerSecond = 0.005f;

private:
	UFUNCTION()
	void OnCoinsChanged(int32 TotalCoins, int32 Delta);

	int32 CalculateXPForLevel(int32 Level) const;
	void CheckLevelUp();
};