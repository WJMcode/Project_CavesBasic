// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/HUDWidget/UserWidgetBase.h"
#include "Components/ProgressBar.h"
#include "HPBarWidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class CAVESBASIC_API UHPBarWidgetBase : public UUserWidgetBase
{
	GENERATED_BODY()

	friend class UBasicHUDWidget;

protected:
	virtual void NativeOnInitialized();
	virtual void NativePreConstruct();
	virtual void NativeConstruct();
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

protected:
	UFUNCTION()
	void OnHiddenUI();
	UFUNCTION()
	virtual void OnHPChanged(float CurrentHP, float MaxHP);
	UFUNCTION()
	virtual void OnDie() {};

protected:
	virtual void RefreshHPBar();

	virtual void RemoveHPBarDelegate();
	virtual void AddHPBarDelegate();

protected:
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	UProgressBar* HPBar_Effect;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	UProgressBar* HPBar;
};
