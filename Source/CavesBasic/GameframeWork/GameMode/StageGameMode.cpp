// Fill out your copyright notice in the Description page of Project Settings.


#include "GameframeWork/GameMode/StageGameMode.h"
#include "StageGameMode.h"
#include "Actors/Player/Input/BasicPlayerController.h"
#include "Actors/Player/BasicPlayer.h"
#include "UI/HUD/BasicHUD.h"

AStageGameMode::AStageGameMode()
{
	PlayerControllerClass = ABasicPlayerController::StaticClass();
	DefaultPawnClass = ABasicPlayer::StaticClass();
}

void AStageGameMode::RespawnPlayer(APlayerController* PlayerController)
{
    if (!PlayerController) return;

    // 사망한 플레이어 Actor 제거
    APawn* OldCharacter = PlayerController->GetPawn();
    OldCharacter->Destroy();

    // PlayerStart를 찾아 리스폰 위치 결정
    AActor* PlayerStart = FindPlayerStart(PlayerController);
    FVector SpawnLocation = PlayerStart->GetActorLocation();
    FRotator SpawnRotation = PlayerStart->GetActorRotation();

    // 새로운 플레이어 캐릭터 생성
    ABasicPlayer* NewCharacter = GetWorld()->SpawnActor<ABasicPlayer>(DefaultPawnClass, SpawnLocation, SpawnRotation);
    if (NewCharacter)
    {
        PlayerController->Possess(NewCharacter);  // 새로운 캐릭터를 플레이어의 Pawn으로 설정

        // 캐릭터 상태 초기화
        NewCharacter->ResetCharacterStatus();

        // HUD 갱신 (새로 생성하는 것이 아니라 기존 HUD를 재활용)
        ABasicHUD* BasicHUD = Cast<ABasicHUD>(PlayerController->GetHUD());
        if (BasicHUD)
        {
            APawn* OwningPawn = BasicHUD->GetOwningPawn();

            if(OwningPawn)
            {
                if (!BasicHUD) { ensure(false); }
                
                // 델리게이트 구독 해제
                BasicHUD->RemoveHUDDelegate();  // 이미 구독한 델리게이트를 해제하는 함수
                // 델리게이트 다시 구독
                BasicHUD->AddRespawnHUDDelegate();  // 델리게이트 재구독

                BasicHUD->ResetUI(OwningPawn);  // UI 갱신 (필요한 부분만 리셋)
                
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("AStageGameMode::RespawnPlayer(APlayerController* PlayerController) : OwningPawn is nullptr"))
            }
        }
    }
}
