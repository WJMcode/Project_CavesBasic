// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "EnhancedActionKeyMapping.h"
#include "Kismet/KismetSystemLibrary.h"

namespace CollisionProfileName
{
	static inline FName PawnTrigger = TEXT("PawnTrigger");		// 플레이어, 적, Pawn을 감지하는 트리거
	static inline FName PlayerTrigger = TEXT("PlayerTrigger");	// 플레이어를 감지하는 트리거
	static inline FName Invincible = TEXT("Invincible");	// 무적 상태 콜리전
	static inline FName Player = TEXT("Player");
	static inline FName PlayerDeath = TEXT("PlayerDeath");
	static inline FName DefaultMonster = TEXT("DefaultMonster");
	static inline FName DefaultProjectile = TEXT("DefaultProjectile");
	static inline FName GroundProjectile = TEXT("GroundProjectile");
}

namespace SocketName
{
	static inline FName Weapon = TEXT("Weapon"); // 캐릭터소켓 : 무기가 위치할 곳
	static inline FName CharacterCenter = TEXT("CharacterCenter"); // 캐릭터소켓 : 캐릭터의 가운데 부분( 스킬 발동 위치 )

	static inline FName Grip = TEXT("Grip"); // 무기소켓 : 무기를 잡는 부분
	static inline FName ProjectileStart = TEXT("ProjectileStart"); // 무기소켓 : 발사체가 나갈 부분
}

class FUtils
{
public:
	static const UInputAction* GetInputActionFromName(UInputMappingContext* IMC, const FName& InName)
	{
		const TArray<FEnhancedActionKeyMapping>& Mappings = IMC->GetMappings();
		for (auto& It : Mappings)
		{
			if (It.Action->GetFName() == InName)
			{
				return It.Action;
			}
		}

		return nullptr;
	}
};
