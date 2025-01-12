// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Misc/Utils.h"
#include "BasicPlayerController.generated.h"

/**
 * 
 */

UCLASS()
class CAVESBASIC_API ABasicPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ABasicPlayerController();
	// 게임창의 크기 변화를 감지하면 호출되는 함수.
	// 마우스 커서를 화면 중앙으로 위치시킨다.
	void OnViewportResized(FViewport* InViewPort, uint32 Unknown);

protected:
	virtual void BeginPlay() override;

protected:
	virtual void SetupInputComponent() override;

protected:
	void OnMove(const FInputActionValue& InputActionValue);
	void OnJump(const FInputActionValue& InputActionValue);
	void OnCrouch(const FInputActionValue& InputActionValue);
	void OnUnCrouch(const FInputActionValue& InputActionValue);

protected:
	UInputMappingContext* IMC_Stage = nullptr;
};
