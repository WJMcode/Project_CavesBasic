// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "StageGameMode.generated.h"

/**
 * 
 */
UCLASS()
class CAVESBASIC_API AStageGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AStageGameMode();
	// 플레이어 리스폰 함수
	void RespawnPlayer(APlayerController* PlayerController);
};
