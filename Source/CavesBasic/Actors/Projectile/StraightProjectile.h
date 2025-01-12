// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Projectile/Projectile.h"
#include "StraightProjectile.generated.h"

/**
 * 
 */
UCLASS()
class CAVESBASIC_API AStraightProjectile : public AProjectile
{
	GENERATED_BODY()

public:
	virtual void SetData(const FDataTableRowHandle& InDataTableRowHandle) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// 캐릭터(플레이어 또는 몬스터)를 감지하는 함수
	virtual AActor* DetectDamageTarget() override;
	virtual void FollowDamageTarget(AActor* TargetActor) override;

	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

protected:
	float Speed = 0.f;
	float LifeSpan = 0.f;
	float Distance = 0.f;
	
protected:
	// Projectile이 생성된 후, 흐른 시간을 저장하는 변수
	float SpawnTime = 0.f;

	// 디버깅용
	FVector initlocation;


protected:
	AActor* DetectActor = nullptr;
	float DetectionRange = 0.f;  // 1000 유닛 이내의 몬스터를 추적합니다.

};
