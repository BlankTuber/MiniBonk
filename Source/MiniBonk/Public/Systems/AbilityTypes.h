#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Engine/DataTable.h"
#include "AbilityTypes.generated.h"

class UTexture2D;

UENUM(BlueprintType)
enum class EAbilityType : uint8
{
	Passive,
	Auto,
	Active
};

UENUM(BlueprintType)
enum class EModifierType : uint8
{
	Flat,
	Percentage
};

UENUM(BlueprintType)
enum class ECardType : uint8
{
	Upgrade,
	Unlock
};

USTRUCT(BlueprintType)
struct FAbilityCard : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText CardName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ECardType CardType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName AffectsAbility;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EAbilityType AbilityType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EModifierType ModifierType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BaseValue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ScalingPerLevel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RareBoostMultiplier = 1.0f;

	// Curse card properties
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsCurse = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bIsCurse"))
	FName CurseAffectsAbility;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bIsCurse"))
	EAbilityType CurseAbilityType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bIsCurse"))
	EModifierType CurseModifierType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bIsCurse"))
	float CurseBaseValue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bIsCurse"))
	float CurseScalingPerLevel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bIsCurse"))
	bool bLocksUnlock = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bLocksUnlock"))
	FName LockedUnlockID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName RequiredUnlock;
};

USTRUCT(BlueprintType)
struct FGeneratedCard
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FName CardID;

	UPROPERTY(BlueprintReadOnly)
	FText CardName;

	UPROPERTY(BlueprintReadOnly)
	FText Description;

	UPROPERTY(BlueprintReadOnly)
	ECardType CardType;

	UPROPERTY(BlueprintReadOnly)
	FName AffectsAbility;

	UPROPERTY(BlueprintReadOnly)
	EAbilityType AbilityType;

	UPROPERTY(BlueprintReadOnly)
	EModifierType ModifierType;

	UPROPERTY(BlueprintReadOnly)
	float Value;

	UPROPERTY(BlueprintReadOnly)
	bool bIsCurse;

	UPROPERTY(BlueprintReadOnly)
	FName CurseAffectsAbility;

	UPROPERTY(BlueprintReadOnly)
	EAbilityType CurseAbilityType;

	UPROPERTY(BlueprintReadOnly)
	EModifierType CurseModifierType;

	UPROPERTY(BlueprintReadOnly)
	float CurseValue;

	UPROPERTY(BlueprintReadOnly)
	bool bLocksUnlock;

	UPROPERTY(BlueprintReadOnly)
	FName LockedUnlockID;
};

USTRUCT(BlueprintType)
struct FCollectedCard
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FName AbilityID;

	UPROPERTY(BlueprintReadOnly)
	EModifierType ModifierType;

	UPROPERTY(BlueprintReadOnly)
	float Value;
};

// Wrapper for nested container UPROPERTY support
USTRUCT()
struct FAbilityCardCollection
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FCollectedCard> Cards;
};

UCLASS()
class MINIBONK_API UAbilityTypes : public UObject
{
	GENERATED_BODY()
};