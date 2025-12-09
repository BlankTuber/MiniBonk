#include "Characters/EnemyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	UpdatePathToPlayer();
}

void AEnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TimeSinceLastPathUpdate += DeltaTime;

	// Update path at regular intervals instead of every frame
	if (TimeSinceLastPathUpdate >= PathUpdateInterval)
	{
		UpdatePathToPlayer();
		TimeSinceLastPathUpdate = 0.f;
	}
}

void AEnemyAIController::UpdatePathToPlayer()
{
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	if (PlayerCharacter)
	{
		MoveToActor(PlayerCharacter, ContactDistance);
	}
}