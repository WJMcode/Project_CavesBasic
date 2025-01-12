// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUDWidget/HPBar/HPBarWidgetDefaultMonster.h"
#include "Kismet/KismetSystemLibrary.h"

void UHPBarWidgetDefaultMonster::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::Hidden);
}

void UHPBarWidgetDefaultMonster::OnHPChanged(float CurrentHP, float MaxHP)
{
	Super::OnHPChanged(CurrentHP, MaxHP);

	// 체력의 변화가 생긴 후 3초 뒤 HPBar를 숨김
	UKismetSystemLibrary::K2_SetTimer(this, TEXT("OnHiddenUI"), 3.f, false);
}

void UHPBarWidgetDefaultMonster::OnDie()
{
	UKismetSystemLibrary::K2_SetTimer(this, TEXT("OnHiddenUI"), 2.f, false);
}
