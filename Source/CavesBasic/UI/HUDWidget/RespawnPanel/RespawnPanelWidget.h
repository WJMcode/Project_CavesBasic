// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/HUDWidget/UserWidgetBase.h"
#include "Components/Button.h"
#include "RespawnPanelWidget.generated.h"

/**
 * 
 */
UCLASS()
class CAVESBASIC_API URespawnPanelWidget : public UUserWidgetBase
{
	GENERATED_BODY()
	 
	friend class UBasicHUDWidget;

protected:
	virtual void NativeConstruct() override;

protected:
	// 플레이어 사망 시, UI_RespawnPanel을 화면에 나타냄
	virtual void ShowRespawnPanel(bool bShow);
	
private:
	// 버튼 클릭 시 호출될 함수
	UFUNCTION()
	void OnAcceptButtonClicked();
	
protected:
	// 해당 버튼 클릭 시 캐릭터 리스폰
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	UButton* AcceptButton;
};
