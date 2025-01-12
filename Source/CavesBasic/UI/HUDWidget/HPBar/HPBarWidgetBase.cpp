// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUDWidget/HPBar/HPBarWidgetBase.h"
#include "Components/StatusComponent.h"

void UHPBarWidgetBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UHPBarWidgetBase::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void UHPBarWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();

	AddHPBarDelegate();

	RefreshHPBar();
}

void UHPBarWidgetBase::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	const float EffectPercent = HPBar_Effect->GetPercent();
	const float Percent = HPBar->GetPercent();
	if (!FMath::IsNearlyEqual(EffectPercent, Percent))
	{
		const float NewEffectPercent = FMath::Lerp(EffectPercent, Percent, InDeltaTime * 10.f);
		HPBar_Effect->SetPercent(NewEffectPercent);
	}
}

void UHPBarWidgetBase::OnHiddenUI()
{
	SetVisibility(ESlateVisibility::Hidden);
}

void UHPBarWidgetBase::OnHPChanged(float CurrentHP, float MaxHP)
{
	if (FMath::IsNearlyZero(MaxHP))
	{
		ensureMsgf(false, TEXT("MaxHP is nearly zero"));
		return;
	}
	SetVisibility(ESlateVisibility::Visible);
	const float Percent = CurrentHP / MaxHP;
	HPBar->SetPercent(Percent);
}

void UHPBarWidgetBase::RefreshHPBar()
{
	if (OwningPawn)
	{
		UStatusComponent* StatusComponent = OwningPawn->GetComponentByClass<UStatusComponent>();
		check(StatusComponent);

		// OwningPawn의 현재 체력 상태를 얻어와서 
		// 계산 후에 화면에 띄워준다.
		float PawnMaxHP = StatusComponent->GetMaxHP();
		float PawnCurrentHP = StatusComponent->GetHP();

		const float Percent = PawnCurrentHP / PawnMaxHP;
		HPBar->SetPercent(Percent);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UHPBarWidgetBase::RefreshHPBar() : Pawn is nullptr"));
	}
}

void UHPBarWidgetBase::RemoveHPBarDelegate()
{
	if(OwningPawn)
	{
		UStatusComponent* StatusComponent = OwningPawn->GetComponentByClass<UStatusComponent>();
		check(StatusComponent);
		StatusComponent->OnHPChanged.RemoveDynamic(this, &ThisClass::OnHPChanged);
		StatusComponent->OnDie.RemoveDynamic(this, &ThisClass::OnDie);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UHPBarWidgetBase::RemoveHPBarDelegate() : OwningPawn is nullptr"));
	}
}

void UHPBarWidgetBase::AddHPBarDelegate()
{
	if (OwningPawn)
	{
		UStatusComponent* StatusComponent = OwningPawn->GetComponentByClass<UStatusComponent>();
		check(StatusComponent);
		StatusComponent->OnHPChanged.AddDynamic(this, &ThisClass::OnHPChanged);
		StatusComponent->OnDie.AddDynamic(this, &ThisClass::OnDie);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UHPBarWidgetBase::AddHPBarDelegate() : OwningPawn is nullptr"));
	}
}


