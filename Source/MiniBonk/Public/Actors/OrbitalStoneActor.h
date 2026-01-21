#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OrbitalStoneActor.generated.h"

class USphereComponent;

UCLASS()
class MINIBONK_API AOrbitalStoneActor : public AActor
{
	GENERATED_BODY()

public:
	AOrbitalStoneActor();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> SphereComponent;
};