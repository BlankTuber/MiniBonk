#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Coin.generated.h"

class USphereComponent;

UCLASS()
class MINIBONK_API ACoin : public AActor
{
	GENERATED_BODY()

public:
	ACoin();

	void SetValue(int32 NewValue);

	UFUNCTION(BlueprintPure, Category = "Coin")
	int32 GetValue() const { return Value; }

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> CollectionSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> MagnetSphere;

	UPROPERTY(EditDefaultsOnly, Category = "Coin")
	float CollectionRadius = 50.f;

	UPROPERTY(EditDefaultsOnly, Category = "Coin")
	float MagnetRadius = 200.f;

	UPROPERTY(EditDefaultsOnly, Category = "Coin")
	float MagnetSpeed = 800.f;

	UPROPERTY(EditDefaultsOnly, Category = "Coin")
	int32 DefaultValue = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Coin")
	float ValueScalePerMinute = 0.5f;

private:
	int32 Value = 1;

	TWeakObjectPtr<AActor> MagnetTarget;

	UFUNCTION()
	void OnCollectionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnMagnetBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnMagnetEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};