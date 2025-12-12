#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Systems/AbilityTypes.h"
#include "AbilityManagerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPassiveCardApplied, FName, AbilityID, float, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnActiveCardCollected, FName, AbilityID, EModifierType, ModifierType, float, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUnlockAcquired, FName, UnlockID);

// Tracks unlocks, limits, and cards
UCLASS(ClassGroup = (Abilities), meta = (BlueprintSpawnableComponent))
class MINIBONK_API UAbilityManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAbilityManagerComponent();

	UPROPERTY(BlueprintAssignable, Category = "Abilities")
	FOnPassiveCardApplied OnPassiveCardApplied;

	UPROPERTY(BlueprintAssignable, Category = "Abilities")
	FOnActiveCardCollected OnActiveCardCollected;

	UPROPERTY(BlueprintAssignable, Category = "Abilities")
	FOnUnlockAcquired OnUnlockAcquired;

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void RegisterUnlock(FName UnlockID);

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void RegisterLimit(FName AbilityID);

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void NotifyLimitReached(FName AbilityID);

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void CollectCard(const FGeneratedCard& Card);

	UFUNCTION(BlueprintPure, Category = "Abilities")
	float CalculateActiveAbilityValue(FName AbilityID, float BaseValue) const;

	UFUNCTION(BlueprintPure, Category = "Abilities")
	bool IsUnlockAvailable(FName UnlockID) const;

	UFUNCTION(BlueprintPure, Category = "Abilities")
	bool IsUnlockAcquired(FName UnlockID) const;

	UFUNCTION(BlueprintPure, Category = "Abilities")
	bool IsAbilityLimited(FName AbilityID) const;

	UFUNCTION(BlueprintPure, Category = "Abilities")
	bool IsUnlockLocked(FName UnlockID) const;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TMap<FName, bool> Unlocks;

	UPROPERTY()
	TSet<FName> LockedUnlocks;

	UPROPERTY()
	TSet<FName> LimitedAbilities;

	UPROPERTY()
	TMap<FName, FAbilityCardCollection> ActiveAbilityCards;

	void ApplyPassiveCard(const FGeneratedCard& Card);
	void StoreActiveCard(const FGeneratedCard& Card);
	void ProcessCurseEffect(const FGeneratedCard& Card);
	void ProcessUnlockLock(FName LockedUnlockID);
};