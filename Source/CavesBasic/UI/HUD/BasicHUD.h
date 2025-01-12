// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Actors/Item/DroppedItem.h"
#include "InputActionValue.h"
#include "UI/HUDWidget/BasicHUDWidget.h"
#include "Actors/Player/Input/BasicPlayerController.h"
#include "BasicHUD.generated.h"

/**
 * 
 */
UCLASS()
class CAVESBASIC_API ABasicHUD : public AHUD
{
	GENERATED_BODY()

	friend class AStageGameMode;
	
public:
	ABasicHUD();
	virtual void PostInitializeComponents() override;

protected:
	// 플레이어 사망 시의 델리게이트를 받아 호출되는 함수
	UFUNCTION()
	virtual void OnPlayerDeath();
	virtual void ResetUI(APawn* InOwningPawn);

	// Player와 접촉한 DroppedItem을 Add하는 함수
	void AddDroppedItem(ADroppedItem* NewItem);
	// Player와 접촉한 DroppedItem을 Remove하는 함수
	void RemoveDroppedItem(ADroppedItem* NewItem);

protected: // InputAction callback
	void OnPickupItem(const FInputActionValue& InputActionValue);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void RemoveHUDDelegate();
	virtual void AddRespawnHUDDelegate();

protected:
	UPROPERTY()
	ABasicPlayerController* PlayerController = nullptr;
	UPROPERTY()
	class UBasicHUDWidget* Widget = nullptr;
	UPROPERTY()
	class UInputMappingContext* IMC_Item = nullptr;
	UPROPERTY()
	TArray<AActor*> OverlappedDroppedItems;
};
