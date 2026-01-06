#include "Actors/Coin.h"
#include "Components/SphereComponent.h"
#include "Characters/PlayerCharacter.h"
#include "Components/CoinComponent.h"

ACoin::ACoin()
{
	PrimaryActorTick.bCanEverTick = true;

	// Collection sphere - triggers pickup
	CollectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollectionSphere"));
	CollectionSphere->InitSphereRadius(CollectionRadius);
	CollectionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollectionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollectionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	RootComponent = CollectionSphere;

	// Magnet sphere - attracts coin to player
	MagnetSphere = CreateDefaultSubobject<USphereComponent>(TEXT("MagnetSphere"));
	MagnetSphere->InitSphereRadius(MagnetRadius);
	MagnetSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	MagnetSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	MagnetSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	MagnetSphere->SetupAttachment(RootComponent);
}

void ACoin::BeginPlay()
{
	Super::BeginPlay();

	// Scale value based on game time
	float ElapsedMinutes = GetWorld()->GetTimeSeconds() / 60.f;
	float ScaleMultiplier = 1.f + (ElapsedMinutes * ValueScalePerMinute);
	Value = FMath::RoundToInt(DefaultValue * ScaleMultiplier);

	// Bind overlap events
	if (ensure(CollectionSphere))
	{
		CollectionSphere->OnComponentBeginOverlap.AddDynamic(this, &ACoin::OnCollectionOverlap);
	}

	if (ensure(MagnetSphere))
	{
		MagnetSphere->OnComponentBeginOverlap.AddDynamic(this, &ACoin::OnMagnetBeginOverlap);
		MagnetSphere->OnComponentEndOverlap.AddDynamic(this, &ACoin::OnMagnetEndOverlap);
	}
}

void ACoin::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MagnetTarget.IsValid())
	{
		FVector CurrentLocation = GetActorLocation();
		FVector TargetLocation = MagnetTarget->GetActorLocation();
		FVector Direction = (TargetLocation - CurrentLocation).GetSafeNormal();

		FVector NewLocation = CurrentLocation + (Direction * MagnetSpeed * DeltaTime);
		SetActorLocation(NewLocation);
	}
}

void ACoin::SetValue(int32 NewValue)
{
	Value = FMath::Max(1, NewValue);
}

void ACoin::OnCollectionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
	if (!Player)
	{
		return;
	}

	if (Player->CoinComponent)
	{
		Player->CoinComponent->AddCoins(Value);
	}

	Destroy();
}

void ACoin::OnMagnetBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
	if (!Player)
	{
		return;
	}

	if (Player->CoinComponent)
	{
		float PlayerMagnetRadius = Player->CoinComponent->GetMagnetRadius();
		MagnetSphere->SetSphereRadius(PlayerMagnetRadius);
	}

	MagnetTarget = Player;
}

void ACoin::OnMagnetEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
	if (!Player)
	{
		return;
	}

	MagnetTarget = nullptr;
}