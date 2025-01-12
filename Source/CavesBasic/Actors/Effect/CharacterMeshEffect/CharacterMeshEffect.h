// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Components/SkeletalMeshComponent.h"
#include "CharacterMeshEffect.generated.h"

/**
 * 
 */
UCLASS()
class CAVESBASIC_API UCharacterMeshEffect : public UObject
{
	GENERATED_BODY()

public:
	virtual void SetOwner(class ABasicPlayer* InOwningPlayer) { OwningPlayer = InOwningPlayer; }
	virtual void SetMesh(USkeletalMeshComponent* InMeshComponent) { TargetMeshComponent = InMeshComponent; }

public:
	// 해당 클래스를 멤버로 보유하고 있는 캐릭터가 피격당하면 호출되는 함수
	virtual void ApplyHitMaterial(const float Duration);
	
protected:
	// 캐릭터의 메쉬를 깜빡거리게 하는 함수
	virtual void BlinkMaterial(UMaterialInstanceDynamic* OutDynOverlayMaterial);
	// 메쉬를 기본값으로 되돌리는 함수
	virtual void RestoreOriginalMaterial(UMaterialInstanceDynamic* OutDynOverlayMaterial);
	// 캐릭터 메쉬에 나타나는 효과가 모두 끝나면 해당 함수를 호출하여 멤버들을 초기화
	virtual void InitializeMembers();

public:
	UPROPERTY()
	// 해당 클래스를 멤버롭 보유하고 있는 캐릭터
	class ABasicPlayer* OwningPlayer;
	// 해당 클래스를 멤버로 보유하고 있는 캐릭터의 메쉬 컴포넌트
	UPROPERTY()
	USkeletalMeshComponent* TargetMeshComponent;

protected:
	// 해당 클래스를 멤버로 보유하고 있는 캐릭터의 원본 머티리얼
	UPROPERTY()
	UMaterialInterface* OriginalOverlayMaterial;

	int32 BlinkCount = 0;
	FTimerHandle BlinkTimerHandle; 
	FTimerHandle RestoreTimerHandle;
};
