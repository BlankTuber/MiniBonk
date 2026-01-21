#include "Actors/OrbitalStoneActor.h"
#include "Components/SphereComponent.h"

AOrbitalStoneActor::AOrbitalStoneActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->InitSphereRadius(25.f);
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = SphereComponent;
}