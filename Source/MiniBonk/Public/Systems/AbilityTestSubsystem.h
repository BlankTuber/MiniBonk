#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AbilityTestSubsystem.generated.h"

class UAbilityCardLibrary;
class UDataTable;
struct FGeneratedCard;

UCLASS()
class MINIBONK_API UAbilityTestSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void GenerateCards();
	void SelectCard(int32 Index);
	void ShowStats();
	void ShowCards();

protected:
	UPROPERTY()
	TObjectPtr<UAbilityCardLibrary> CardLibrary;

	UPROPERTY()
	TObjectPtr<UDataTable> CardDataTable;

	TArray<FGeneratedCard> CurrentCardChoices;

private:
	void RegisterConsoleCommands();
	void UnregisterConsoleCommands();

	IConsoleObject* GenerateCardsCommand = nullptr;
	IConsoleObject* SelectCardCommand = nullptr;
	IConsoleObject* ShowStatsCommand = nullptr;
	IConsoleObject* ShowCardsCommand = nullptr;
	IConsoleObject* UnpauseCommand = nullptr;
};
