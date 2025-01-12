// Fill out your copyright notice in the Description page of Project Settings.

// JM
// 전방에 바닥이 있으면 생성되는 장판형 스킬
// 만약 공중 몬스터가 있다면 overlap될 수 있으므로 바닥이 없는 경우 생성되면 안됨.
// GroundProjectile은 무기 소켓으로부터 스폰되는 게 아닌, 캐릭터 기준으로 스폰되어야 한다.

// JM@TODO : Monster에게 데미지 주기, BeginPlay 함수에 있는 FHitResult UpHitResult 부분 활용하기.
// 부모 클래스에 구현해서 StraightProjectile이 몬스터를 감지할 때 사용?

#pragma once

#include "CoreMinimal.h"
#include "Actors/Projectile/Projectile.h"
#include "GroundProjectile.generated.h"

/**
 * 
 */
UCLASS()
class CAVESBASIC_API AGroundProjectile : public AProjectile
{
	GENERATED_BODY()

public:
	AGroundProjectile();
	virtual void SetAdjustLocation(FTransform& OutTransform);
	
protected:
	virtual void BeginPlay() override;

protected:
	virtual AActor* DetectDamageTarget() override;
	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
};
