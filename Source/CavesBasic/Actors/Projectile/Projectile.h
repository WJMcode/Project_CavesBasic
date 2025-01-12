// Fill out your copyright notice in the Description page of Project Settings.

//JM@TODO : Ctrl(Crouch 키)를 입력한 상태로 생성하자마자 overlap되면 이상하게 동작함

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Projectile.generated.h"

USTRUCT()
struct CAVESBASIC_API FProjectileTableRow : public FTableRowBase
{
	GENERATED_BODY()
	FProjectileTableRow();

public:
	UPROPERTY(EditAnywhere, Category = "Projectile")
	UStaticMesh* StaticMesh = nullptr;

	UPROPERTY(EditAnywhere, Category = "Projectile")
	FTransform Transform = FTransform::Identity;

	UPROPERTY(EditAnywhere, Category = "Projectile")
	float InitialSpeed = 5000.f;
	UPROPERTY(EditAnywhere, Category = "Projectile")
	float MaxSpeed = 5000.f;
	UPROPERTY(EditAnywhere, Category = "Projectile")
	float ProjectileGravityScale = 0.0f;
	UPROPERTY(EditAnywhere, Category = "Projectile")
	float ProjectileLifeSpan = 5.0f;
	UPROPERTY(EditAnywhere, Category = "Projectile")
	bool bIsVisible = true;

	UPROPERTY(EditAnywhere, Category = "Projectile")
	TSubclassOf<AProjectile> ProjectileClass;

public:
	UPROPERTY(EditAnywhere, Category = "Projectile|MeshEffect")
	TObjectPtr<UParticleSystem> ProjectileMeshEffect;

	UPROPERTY(EditAnywhere, Category = "Projectile|MeshEffect")
	FTransform MeshEffectTransform = FTransform::Identity;

	UPROPERTY(EditAnywhere, Category = "Projectile|Effect", meta = (RowType = "/Script/CavesBasic.EffectDecalTableRow"))
	FDataTableRowHandle EffectTableRowHandle;
};


//DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE(FOnProjectileDestroyed, AProjectile, OnProjectileDestroyed);
UCLASS()
class CAVESBASIC_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AProjectile();
	virtual void SetData(const FDataTableRowHandle& InDataTableRowHandle);
	virtual void SetAdjustLocation(FTransform& OutTransform) {};

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual AActor* DetectDamageTarget() { return nullptr; };
	virtual AActor* DetectDamageTarget(float DeltaTime) { return nullptr; };
	virtual void FollowDamageTarget(AActor* TargetActor) { };

	UFUNCTION()
	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	// Destroyed가 호출되면 Actor가 파괴되기 직전에 
	// OnDestroyed가 호출된다. 그러면 OnProjectileDestroyed 함수가 실행됨
	// OnProjectileDestroyed 함수에서는 Actor가 파괴되기 전에 해주고 싶은 동작을 해 주면 됨
	UFUNCTION()
	void OnProjectileDestroyed(AActor* DestroyedActor);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY()
	UStaticMeshComponent* StaticMeshComponent;
	UPROPERTY()
	TObjectPtr<UParticleSystemComponent> ProjectileMeshEffectComponent;
	UPROPERTY()
	UProjectileMovementComponent* ProjectileMovementComponent;

protected:
	UPROPERTY(EditAnywhere, meta = (RowType = "/Script/CavesBasic.ProjectileTableRow"))
	FDataTableRowHandle DataTableRowHandle;

	FProjectileTableRow* ProjectileData;

//protected:
//	FOnProjectileDestroyed OnProjectileDestroyed;
};
