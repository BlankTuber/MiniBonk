#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossGroundAttack.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UAudioComponent;

UCLASS()
class MINIBONK_API ABossGroundAttack : public AActor
{
	GENERATED_BODY()

public:
	ABossGroundAttack();

	void InitializeAttack(float InDamage);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> DamageArea;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> VisualMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	float WarningDuration = 1.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	float DamageRadius = 200.f;

	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	float StartScale = 0.2f;

	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	float EndScale = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	float PulseSpeed = 8.f;

	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	float PulseIntensity = 0.1f;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	TObjectPtr<USoundBase> WarningSound;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	TObjectPtr<USoundBase> ExplosionSound;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	float WarningPitchStart = 0.8f;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	float WarningPitchEnd = 1.5f;

private:
	float Damage = 0.f;
	float ElapsedTime = 0.f;
	FTimerHandle DetonationTimerHandle;

	UPROPERTY()
	TObjectPtr<UAudioComponent> WarningAudioComponent;

	void Detonate();
	void UpdateVisuals(float DeltaTime);
};