#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Systems/AbilityTypes.h"
#include "CoinComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCoinsChangedSignature, int32, CurrentCoins, int32, Delta);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MINIBONK_API UCoinComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCoinComponent();

	UPROPERTY(BlueprintAssignable, Category = "Coins")
	FOnCoinsChangedSignature OnCoinsChanged;

	UFUNCTION(BlueprintCallable, Category = "Coins")
	void AddCoins(int32 Amount);

	UFUNCTION(BlueprintPure, Category = "Coins")
	int32 GetCurrentCoins() const { return CurrentCoins; }

	UFUNCTION(BlueprintPure, Category = "Coins")
	float GetMagnetRadius() const { return MagnetRadius; }

	UFUNCTION(BlueprintCallable, Category = "Coins")
	void SetMagnetRadius(float NewRadius);

	UFUNCTION()
	void OnPassiveCardApplied(FName AbilityID, EModifierType ModifierType, float Value);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coins")
	int32 CurrentCoins = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Coins|Magnet")
	float BaseMagnetRadius = 200.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coins|Magnet")
	float MagnetRadius;

	UPROPERTY(EditDefaultsOnly, Category = "Coins|Magnet|Limits")
	float MaxMagnetRadius = 800.f;
};