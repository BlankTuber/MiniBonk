#include "Actors/StoneProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Characters/EnemyCharacter.h"
#include "Components/HealthComponent.h"

AStoneProjectile::AStoneProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComponent->InitSphereRadius(15.f);
	CollisionComponent->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	CollisionComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	RootComponent = CollisionComponent;

	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComponent"));
	MovementComponent->bRotationFollowsVelocity = true;
	MovementComponent->bShouldBounce = false;
	MovementComponent->ProjectileGravityScale = 0.f;
	MovementComponent->bAutoActivate = false;
}

void AStoneProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	SetLifeSpan(Lifespan);

	if (GetOwner())
	{
		CollisionComponent->IgnoreActorWhenMoving(GetOwner(), true);
	}


	CollisionComponent->OnComponentHit.AddDynamic(this, &AStoneProjectile::OnHit);
}

void AStoneProjectile::InitializeProjectile(float InDamage, const FVector& Direction, float InSpeed)
{
	Damage = InDamage;

	if (ensure(MovementComponent))
	{
		MovementComponent->InitialSpeed = InSpeed;
		MovementComponent->MaxSpeed = InSpeed;
		MovementComponent->Velocity = Direction.GetSafeNormal() * InSpeed;
		MovementComponent->Activate();
	}
}

void AStoneProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor || OtherActor == GetOwner())
	{
		return;
	}

	AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(OtherActor);
	
	UE_LOG(LogTemp, Warning, TEXT("StoneProjectile::OnHit - Cast to Enemy: %s"), Enemy ? TEXT("SUCCESS") : TEXT("FAILED"));

	if (Enemy)
	{
		UHealthComponent* HealthComp = Enemy->HealthComponent;
		if (HealthComp)
		{
			HealthComp->TakeDamage(Damage);
		}
	}

	Destroy();
}