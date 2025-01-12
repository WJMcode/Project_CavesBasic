// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUDWidget/BasicHUDWidget.h"

void UBasicHUDWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UBasicHUDWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void UBasicHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

    // 위젯이 제대로 바인딩되었는지 확인
    if (UI_PlayerHPBar)
    {
        UE_LOG(LogTemp, Warning, TEXT("UI_PlayerHPBar has been successfully bound!"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("UI_PlayerHPBar is NULL!"));
    }

    // 위젯이 제대로 바인딩되었는지 확인
    if (UI_RespawnPanel)
    {
        UE_LOG(LogTemp, Warning, TEXT("UI_RespawnPanel has been successfully bound!"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("UI_RespawnPanel is NULL!"));
    }
}

void UBasicHUDWidget::InitializeWidgets(APawn* InOwningPawn)
{
    if (!InOwningPawn) { ensure(false); return; }

    {
        UI_PlayerHPBar->SetOwningPawn(InOwningPawn);
    }

    {
        UI_RespawnPanel->SetOwningPawn(InOwningPawn);
        UI_RespawnPanel->ShowRespawnPanel(false);
    }
}

void UBasicHUDWidget::RefreshWidgetsForRespawn()
{
    // PlayerHPBar 관련
    {
        if (UI_PlayerHPBar)
        {
            // 델리게이트 구독 해제
            UI_PlayerHPBar->RemoveHPBarDelegate();  // 이미 구독한 델리게이트를 해제하는 함수
            // 델리게이트 다시 구독
            UI_PlayerHPBar->AddHPBarDelegate();  // 델리게이트 재구독

            UI_PlayerHPBar->RefreshHPBar();
        }
    }
}

void UBasicHUDWidget::HandlePlayerDeath()
{
    UI_RespawnPanel->ShowRespawnPanel(true);
}

void UBasicHUDWidget::AddDroppedItem(ADroppedItem* NewItem)
{
	ReceiveAddDroppedItem();
}

void UBasicHUDWidget::RemoveDroppedItem(ADroppedItem* NewItem)
{
	ReceiveRemoveDroppedItem();
}
