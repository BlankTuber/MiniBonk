#include "Components/ChestSpawnerComponent.h"
#include "Actors/Chest.h"
#include "Systems/KillTrackingSubsystem.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"

UChestSpawnerComponent::UChestSpawnerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UChestSpawnerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (UGameInstance* GI = GetWorld()->GetGameInstance())
	{
		if (UKillTrackingSubsystem* KillTracker = GI->GetSubsystem<UKillTrackingSubsystem>())
		{
			KillTracker->OnEnemyKilled.AddDynamic(this, &UChestSpawnerComponent::OnEnemyKilled);
		}
	}

	if (!ensure(ChestClass))
	{
		UE_LOG(LogTemp, Error, TEXT("ChestSpawnerComponent: No ChestClass set!"));
	}
}

void UChestSpawnerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UGameInstance* GI = GetWorld()->GetGameInstance())
	{
		if (UKillTrackingSubsystem* KillTracker = GI->GetSubsystem<UKillTrackingSubsystem>())
		{
			KillTracker->OnEnemyKilled.RemoveDynamic(this, &UChestSpawnerComponent::OnEnemyKilled);
		}
	}

	Super::EndPlay(EndPlayReason);
}

void UChestSpawnerComponent::OnEnemyKilled()
{
	KillsSinceLastChest++;

	int32 KillsRequired = GetCurrentKillsRequired();

	if (KillsSinceLastChest >= KillsRequired)
	{
		SpawnChest();
		KillsSinceLastChest = 0;
		ChestsSpawned++;

		UE_LOG(LogTemp, Log, TEXT("ChestSpawnerComponent: Chest spawned! Next chest requires %d kills"), GetCurrentKillsRequired());
	}
}

void UChestSpawnerComponent::SpawnChest()
{
	if (!ChestClass)
	{
		return;
	}

	FVector SpawnLocation = GetSpawnLocation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AChest* SpawnedChest = GetWorld()->SpawnActor<AChest>(
		ChestClass,
		SpawnLocation,
		FRotator::ZeroRotator,
		SpawnParams
	);

	if (SpawnedChest)
	{
		UE_LOG(LogTemp, Log, TEXT("ChestSpawnerComponent: Spawned chest at %s"), *SpawnLocation.ToString());
	}
}

int32 UChestSpawnerComponent::GetCurrentKillsRequired() const
{
	float Scaled = BaseKillsRequired * FMath::Pow(KillScalingRate, static_cast<float>(ChestsSpawned));
	return FMath::RoundToInt(Scaled);
}

FVector UChestSpawnerComponent::GetSpawnLocation() const
{
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavSystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("ChestSpawnerComponent: No navigation system found"));
		return GetOwner()->GetActorLocation();
	}

	APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	FVector Origin = Player ? Player->GetActorLocation() : GetOwner()->GetActorLocation();

	// Try to find a valid NavMesh point within radius
	for (int32 Attempt = 0; Attempt < 10; ++Attempt)
	{
		float RandomAngle = FMath::RandRange(0.f, 360.f);
		float RandomRadius = FMath::RandRange(MinSpawnRadius, MaxSpawnRadius);

		FVector Offset = FVector(
			FMath::Cos(FMath::DegreesToRadians(RandomAngle)) * RandomRadius,
			FMath::Sin(FMath::DegreesToRadians(RandomAngle)) * RandomRadius,
			0.f
		);

		FVector TestLocation = Origin + Offset;

		FNavLocation NavLocation;
		if (NavSystem->ProjectPointToNavigation(TestLocation, NavLocation, FVector(500.f, 500.f, 500.f)))
		{
			return NavLocation.Location;
		}
	}

	// Fallback: random point anywhere on NavMesh
	FNavLocation RandomNavLocation;
	if (NavSystem->GetRandomPoint(RandomNavLocation))
	{
		return RandomNavLocation.Location;
	}

	return Origin;
}