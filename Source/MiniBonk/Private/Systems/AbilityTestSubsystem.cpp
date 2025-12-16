#include "Systems/AbilityTestSubsystem.h"
#include "Systems/AbilityCardLibrary.h"
#include "Systems/AbilityManagerComponent.h"
#include "Systems/AbilityTypes.h"
#include "Components/HealthComponent.h"
#include "Components/MovementStatsComponent.h"
#include "Characters/PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"
#include "HAL/IConsoleManager.h"

void UAbilityTestSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	CardDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Data/DT_AbilityCards.DT_AbilityCards"));

	if (!CardDataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("AbilityTestSubsystem: Could not load DT_AbilityCards. Create it at /Game/Data/DT_AbilityCards"));
	}

	CardLibrary = NewObject<UAbilityCardLibrary>(this);

	RegisterConsoleCommands();

	UE_LOG(LogTemp, Log, TEXT("AbilityTestSubsystem: Initialized. Use 'Ability.Generate' to start."));
}

void UAbilityTestSubsystem::Deinitialize()
{
	UnregisterConsoleCommands();
	Super::Deinitialize();
}

void UAbilityTestSubsystem::RegisterConsoleCommands()
{
	GenerateCardsCommand = IConsoleManager::Get().RegisterConsoleCommand(TEXT("Ability.Generate"), TEXT("Generate 3 random ability cards"), FConsoleCommandDelegate::CreateUObject(this, &UAbilityTestSubsystem::GenerateCards), ECVF_Default);

    SelectCardCommand = IConsoleManager::Get().RegisterConsoleCommand(TEXT("Ability.Select"), TEXT("Select a card by index (0, 1, or 2). Usage: Ability.Select 0"),
        FConsoleCommandWithArgsDelegate::CreateLambda([this](const TArray<FString>& Args)
            {
                if (Args.Num() < 1)
                {
                    UE_LOG(LogTemp, Warning, TEXT("Usage: Ability.Select <index>"));
                    return;
                }
                int32 Index = FCString::Atoi(*Args[0]);
                SelectCard(Index);
            }),
        ECVF_Default
    );

    ShowStatsCommand = IConsoleManager::Get().RegisterConsoleCommand(TEXT("Ability.Stats"), TEXT("Show current player stats"), FConsoleCommandDelegate::CreateUObject(this, &UAbilityTestSubsystem::ShowStats), ECVF_Default);

    ShowCardsCommand = IConsoleManager::Get().RegisterConsoleCommand(TEXT("Ability.Cards"), TEXT("Show currently available cards"), FConsoleCommandDelegate::CreateUObject(this, &UAbilityTestSubsystem::ShowCards), ECVF_Default);
}

void UAbilityTestSubsystem::UnregisterConsoleCommands()
{
    if (GenerateCardsCommand)
    {
        IConsoleManager::Get().UnregisterConsoleObject(GenerateCardsCommand);
    }
    if (SelectCardCommand)
    {
        IConsoleManager::Get().UnregisterConsoleObject(SelectCardCommand);
    }
    if (ShowStatsCommand)
    {
        IConsoleManager::Get().UnregisterConsoleObject(ShowStatsCommand);
    }
    if (ShowCardsCommand)
    {
        IConsoleManager::Get().UnregisterConsoleObject(ShowCardsCommand);
    }
}

void UAbilityTestSubsystem::GenerateCards()
{
    if (!CardDataTable)
    {
        UE_LOG(LogTemp, Error, TEXT("Ability.Generate: No DataTable loaded! Create /Game/Data/DT_AbilityCards"));
        return;
    }

    UWorld* World = GetGameInstance()->GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("Ability.Generate: No world found!"));
        return;
    }

    APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(World, 0));
    if (!Player)
    {
        UE_LOG(LogTemp, Error, TEXT("Ability.Generate: No player found!"));
        return;
    }

    UAbilityManagerComponent* AbilityManager = Player->AbilityManagerComponent;
    if (!AbilityManager)
    {
        UE_LOG(LogTemp, Error, TEXT("Ability.Generate: Player has no AbilityManagerComponent!"));
        return;
    }


    CardLibrary->Initialize(CardDataTable, AbilityManager);

    CurrentCardChoices = CardLibrary->GenerateCardChoices(1);

    if (CurrentCardChoices.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Ability.Generate: No cards generated! Check DataTable has rows."));
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("========== GENERATED CARDS =========="));
    for (int32 i = 0; i < CurrentCardChoices.Num(); ++i)
    {
        const FGeneratedCard& Card = CurrentCardChoices[i];
        FString ModType = Card.ModifierType == EModifierType::Flat ? TEXT("Flat") : TEXT("Percent");
        UE_LOG(LogTemp, Log, TEXT("[%d] %s - %s (+%.1f %s)"),
            i,
            *Card.CardName.ToString(),
            *Card.AffectsAbility.ToString(),
            Card.Value,
            *ModType);
    }
    UE_LOG(LogTemp, Log, TEXT("======================================"));
    UE_LOG(LogTemp, Log, TEXT("Use 'Ability.Select <0-2>' to pick a card"));
}

void UAbilityTestSubsystem::SelectCard(int32 Index)
{
    if (CurrentCardChoices.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Ability.Select: No cards available! Use 'Ability.Generate' first."));
        return;
    }

    if (Index < 0 || Index >= CurrentCardChoices.Num())
    {
        UE_LOG(LogTemp, Warning, TEXT("Ability.Select: Invalid index %d. Choose 0-%d"), Index, CurrentCardChoices.Num() - 1);
        return;
    }

    UWorld* World = GetGameInstance()->GetWorld();
    if (!World)
    {
        return;
    }

    APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(World, 0));
    if (!Player || !Player->AbilityManagerComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("Ability.Select: No player or AbilityManager!"));
        return;
    }

    const FGeneratedCard& SelectedCard = CurrentCardChoices[Index];

    UE_LOG(LogTemp, Log, TEXT("Selected: %s"), *SelectedCard.CardName.ToString());

    Player->AbilityManagerComponent->CollectCard(SelectedCard);

    CurrentCardChoices.Empty();

    UE_LOG(LogTemp, Log, TEXT("Card applied! Use 'Ability.Stats' to see changes."));
}

void UAbilityTestSubsystem::ShowStats()
{
    UWorld* World = GetGameInstance()->GetWorld();
    if (!World)
    {
        return;
    }

    APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(World, 0));
    if (!Player)
    {
        UE_LOG(LogTemp, Error, TEXT("Ability.Stats: No player found!"));
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("========== PLAYER STATS =========="));

    // Health
    if (Player->HealthComponent)
    {
        UE_LOG(LogTemp, Log, TEXT("Health: %.0f / %.0f"),
            Player->HealthComponent->GetHealthPrecent() * Player->HealthComponent->MaxHealth,
            Player->HealthComponent->MaxHealth);
    }

    // Movement
    if (UCharacterMovementComponent* MoveComp = Player->GetCharacterMovement())
    {
        UE_LOG(LogTemp, Log, TEXT("Move Speed: %.0f"), MoveComp->MaxWalkSpeed);
        UE_LOG(LogTemp, Log, TEXT("Jump Height: %.0f"), MoveComp->JumpZVelocity);
    }

    UE_LOG(LogTemp, Log, TEXT("=================================="));
}

void UAbilityTestSubsystem::ShowCards()
{
    if (CurrentCardChoices.Num() == 0)
    {
        UE_LOG(LogTemp, Log, TEXT("No cards currently available. Use 'Ability.Generate' to generate cards."));
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("========== AVAILABLE CARDS =========="));
    for (int32 i = 0; i < CurrentCardChoices.Num(); ++i)
    {
        const FGeneratedCard& Card = CurrentCardChoices[i];
        FString ModType = Card.ModifierType == EModifierType::Flat ? TEXT("Flat") : TEXT("Percent");
        UE_LOG(LogTemp, Log, TEXT("[%d] %s - %s (+%.1f %s)"),
            i,
            *Card.CardName.ToString(),
            *Card.AffectsAbility.ToString(),
            Card.Value,
            *ModType);
    }
    UE_LOG(LogTemp, Log, TEXT("======================================"));
}
