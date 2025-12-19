#include "Components/EnemySpawnerComponent.h"
#include "Characters/EnemyCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"

UEnemySpawnerComponent::UEnemySpawnerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	CurrentSpawnInterval = BaseSpawnInterval;
}

void UEnemySpawnerComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentSpawnInterval = BaseSpawnInterval;

	if (bStartSpawningOnBeginPlay)
	{
		StartSpawning();
	}
}

void UEnemySpawnerComponent::StartSpawning()
{
	if (!ensure(EnemyClass))
	{
		UE_LOG(LogTemp, Error, TEXT("EnemySpawnerComponent: No EnemyClass set!"));
		return;
	}

	GetWorld()->GetTimerManager().SetTimer(
		SpawnTimerHandle,
		this,
		&UEnemySpawnerComponent::SpawnEnemy,
		CurrentSpawnInterval,
		false
	);

	UE_LOG(LogTemp, Log, TEXT("EnemySpawnerComponent: Started spawning"));
}

void UEnemySpawnerComponent::StopSpawning()
{
	GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);
	UE_LOG(LogTemp, Log, TEXT("EnemySpawnerComponent: Stopped spawning"));
}

void UEnemySpawnerComponent::SpawnEnemy()
{
	FVector SpawnLocation = GetSpawnLocation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AEnemyCharacter* SpawnedEnemy = GetWorld()->SpawnActor<AEnemyCharacter>(
		EnemyClass,
		SpawnLocation,
		FRotator::ZeroRotator,
		SpawnParams
	);

	if (SpawnedEnemy)
	{
		UE_LOG(LogTemp, Verbose, TEXT("EnemySpawnerComponent: Spawned enemy at %s"), *SpawnLocation.ToString());
	}

	UpdateSpawnInterval();

	GetWorld()->GetTimerManager().SetTimer(
		SpawnTimerHandle,
		this,
		&UEnemySpawnerComponent::SpawnEnemy,
		CurrentSpawnInterval,
		false
	);
}

void UEnemySpawnerComponent::UpdateSpawnInterval()
{
	ElapsedTime += CurrentSpawnInterval;
	CurrentSpawnInterval = FMath::Max(MinSpawnInterval, BaseSpawnInterval - (ElapsedTime * SpawnIntervalDecreaseRate));
}

FVector UEnemySpawnerComponent::GetSpawnLocation() const
{
	APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!Player)
	{
		return GetOwner()->GetActorLocation();
	}

	FVector PlayerLocation = Player->GetActorLocation();

	float RandomAngle = FMath::RandRange(0.f, 360.f);
	float RandomRadius = GetWeightedRandomRadius();

	FVector Offset = FVector(
		FMath::Cos(FMath::DegreesToRadians(RandomAngle)) * RandomRadius,
		FMath::Sin(FMath::DegreesToRadians(RandomAngle)) * RandomRadius,
		0.f
	);

	FVector SpawnLocation = PlayerLocation + Offset;

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (NavSystem)
	{
		FNavLocation NavLocation;
		if (NavSystem->ProjectPointToNavigation(SpawnLocation, NavLocation))
		{
			SpawnLocation = NavLocation.Location;
			SpawnLocation.Z += 100.f;
		}
	}

	return SpawnLocation;
}

float UEnemySpawnerComponent::GetWeightedRandomRadius() const
{
	float RandomValue = FMath::FRand();
	float WeightedValue = FMath::Pow(RandomValue, SpawnDistanceBias);
	return FMath::Lerp(MinSpawnRadius, MaxSpawnRadius, WeightedValue);
}