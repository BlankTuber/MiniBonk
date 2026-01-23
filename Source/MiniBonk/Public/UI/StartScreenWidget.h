#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StartScreenWidget.generated.h"

class UButton;

UCLASS()
class MINIBONK_API UStartScreenWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> PlayButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> QuitButton;

	UPROPERTY(EditDefaultsOnly, Category = "Start Screen")
	FName GameLevelName = "TestMap";

private:
	UFUNCTION()
	void OnPlayClicked();

	UFUNCTION()
	void OnQuitClicked();
};