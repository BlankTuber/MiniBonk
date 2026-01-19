#include "Systems/KillTrackingSubsystem.h"

void UKillTrackingSubsystem::NotifyEnemyKilled()
{
	TotalKills++;
	OnEnemyKilled.Broadcast();

	UE_LOG(LogTemp, Verbose, TEXT("KillTrackingSubsystem: Enemy killed. Total: %d"), TotalKills);
}