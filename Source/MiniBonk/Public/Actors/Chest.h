#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Chest.generated.h"

class USphereComponent;
class UPointLightComponent;
class APlayerCharacter;

UCLASS()
class MINIBONK_API AChest : public AActor
{
	GENERATED_BODY()

public:
	AChest();

	UFUNCTION(BlueprintPure, Category = "Chest")
	int32 GetCost() const { return CurrentCost; }

	UFUNCTION(BlueprintPure, Category = "Chest")
	bool CanPlayerAfford() const;

	UFUNCTION(BlueprintPure, Category = "Chest")
	bool IsPlayerInRange() const { return OverlappingPlayer.IsValid(); }

	UFUNCTION(BlueprintCallable, Category = "Chest")
	bool TryPurchase();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> InteractionSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UPointLightComponent> InteractionLight;

	UPROPERTY(EditDefaultsOnly, Category = "Chest|Cost")
	int32 BaseCost = 25;

	UPROPERTY(EditDefaultsOnly, Category = "Chest|Cost")
	float CostGrowthRate = 1.3f;

	UPROPERTY(EditDefaultsOnly, Category = "Chest|Interaction")
	float InteractionRadius = 200.f;

	UPROPERTY(EditDefaultsOnly, Category = "Chest|Visual")
	FLinearColor CanAffordColor = FLinearColor::Green;

	UPROPERTY(EditDefaultsOnly, Category = "Chest|Visual")
	FLinearColor CannotAffordColor = FLinearColor::Red;

	UPROPERTY(EditDefaultsOnly, Category = "Chest|Visual")
	float LightIntensity = 5000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Chest|Visual")
	float LightRadius = 300.f;

private:
	int32 CurrentCost = 0;
	TWeakObjectPtr<APlayerCharacter> OverlappingPlayer;

	void CalculateCost();
	void UpdateInteractionPrompt();
	void UpdateVisualFeedback();

	UFUNCTION()
	void OnInteractionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnInteractionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnPlayerCoinsChanged(int32 CurrentCoins, int32 Delta);
};