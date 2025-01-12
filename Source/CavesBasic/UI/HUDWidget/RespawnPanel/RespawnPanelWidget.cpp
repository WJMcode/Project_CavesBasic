// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUDWidget/RespawnPanel/RespawnPanelWidget.h"
#include "GameframeWork/GameMode/StageGameMode.h"

void URespawnPanelWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 버튼 클릭 이벤트 바인딩
	if (AcceptButton)
	{
		AcceptButton->OnClicked.AddDynamic(this, &ThisClass::OnAcceptButtonClicked);
	}

	SetVisibility(ESlateVisibility::Hidden);
}

void URespawnPanelWidget::ShowRespawnPanel(bool bShow)
{
	SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void URespawnPanelWidget::OnAcceptButtonClicked()
{
    // 현재 월드에서 GameMode 얻기
    AStageGameMode* StageGameMode = GetWorld()->GetAuthGameMode<AStageGameMode>();
    if (StageGameMode)
    {
        // 현재 플레이어 컨트롤러 얻기
        APlayerController* PlayerController = GetOwningPlayer();
        if (PlayerController)
        {
            // 게임모드에서 리스폰 호출
            StageGameMode->RespawnPlayer(PlayerController);
        }
    }

	SetVisibility(ESlateVisibility::Hidden);
}
