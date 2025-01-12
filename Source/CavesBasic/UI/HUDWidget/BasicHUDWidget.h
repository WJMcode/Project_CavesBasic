// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/VerticalBox.h"
#include "UI/HUDWidget/HPBar/HPBarWidgetBase.h"
#include "UI/HUDWidget/RespawnPanel/RespawnPanelWidget.h"
#include "BasicHUDWidget.generated.h"

/**
 * 
 */

class ADroppedItem;

UCLASS()
class CAVESBASIC_API UBasicHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
	friend class ABasicHUD;

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

protected:
	virtual void InitializeWidgets(APawn* InOwningPawn);
	virtual void RefreshWidgetsForRespawn();

protected:
	virtual void HandlePlayerDeath();
	void AddDroppedItem(ADroppedItem* NewItem);
	void RemoveDroppedItem(ADroppedItem* NewItem);

protected:
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "AddDroppedItem"))
	void ReceiveAddDroppedItem();
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "RemoveDroppedItem"))
	void ReceiveRemoveDroppedItem();

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UVerticalBox* DroppedItemDescVerticalBox = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UHPBarWidgetBase* UI_PlayerHPBar = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	URespawnPanelWidget* UI_RespawnPanel = nullptr;
};
