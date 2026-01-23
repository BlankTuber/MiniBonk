#include "UI/CardSelectionWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UCardSelectionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Card0Button)
	{
		Card0Button->OnClicked.RemoveDynamic(this, &UCardSelectionWidget::OnCard0Clicked);
		Card0Button->OnClicked.AddDynamic(this, &UCardSelectionWidget::OnCard0Clicked);
	}

	if (Card1Button)
	{
		Card1Button->OnClicked.RemoveDynamic(this, &UCardSelectionWidget::OnCard1Clicked);
		Card1Button->OnClicked.AddDynamic(this, &UCardSelectionWidget::OnCard1Clicked);
	}

	if (Card2Button)
	{
		Card2Button->OnClicked.RemoveDynamic(this, &UCardSelectionWidget::OnCard2Clicked);
		Card2Button->OnClicked.AddDynamic(this, &UCardSelectionWidget::OnCard2Clicked);
	}
}

void UCardSelectionWidget::SetCardChoices(const TArray<FGeneratedCard>& Cards)
{
	CurrentCards = Cards;

	for (int32 i = 0; i < Cards.Num() && i < 3; ++i)
	{
		UpdateCardDisplay(i, Cards[i]);
	}
}

void UCardSelectionWidget::UpdateCardDisplay(int32 Index, const FGeneratedCard& Card)
{
	UTextBlock* NameBlock = nullptr;
	UTextBlock* DescBlock = nullptr;
	UTextBlock* ValueBlock = nullptr;

	switch (Index)
	{
	case 0:
		NameBlock = Card0Name;
		DescBlock = Card0Description;
		ValueBlock = Card0Value;
		break;
	case 1:
		NameBlock = Card1Name;
		DescBlock = Card1Description;
		ValueBlock = Card1Value;
		break;
	case 2:
		NameBlock = Card2Name;
		DescBlock = Card2Description;
		ValueBlock = Card2Value;
		break;
	}

	if (NameBlock)
	{
		NameBlock->SetText(Card.CardName);
	}

	if (DescBlock)
	{
		DescBlock->SetText(Card.Description);
	}

	if (ValueBlock)
	{
		ValueBlock->SetText(FText::FromString(FormatCardValue(Card)));
	}
}

FString UCardSelectionWidget::FormatCardValue(const FGeneratedCard& Card) const
{
	if (Card.ModifierType == EModifierType::Flat)
	{
		return FString::Printf(TEXT("+%.0f"), Card.Value);
	}
	else
	{
		return FString::Printf(TEXT("+%.0f%%"), Card.Value * 100.f);
	}
}

void UCardSelectionWidget::OnCard0Clicked()
{
	SelectCard(0);
}

void UCardSelectionWidget::OnCard1Clicked()
{
	SelectCard(1);
}

void UCardSelectionWidget::OnCard2Clicked()
{
	SelectCard(2);
}

void UCardSelectionWidget::SelectCard(int32 Index)
{
	if (!CurrentCards.IsValidIndex(Index))
	{
		UE_LOG(LogTemp, Warning, TEXT("CardSelectionWidget: Invalid card index %d"), Index);
		return;
	}

	const FGeneratedCard& SelectedCard = CurrentCards[Index];
	UE_LOG(LogTemp, Log, TEXT("CardSelectionWidget: Selected card '%s'"), *SelectedCard.CardName.ToString());

	OnCardSelected.Broadcast(SelectedCard);
}

void UCardSelectionWidget::SetTitle(const FText& Title)
{
	if (TitleText)
	{
		TitleText->SetText(Title);
	}
}