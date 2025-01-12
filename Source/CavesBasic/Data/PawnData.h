// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PawnData.generated.h"

USTRUCT()
struct CAVESBASIC_API FPawnTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Pawn")
	USkeletalMesh* SkeletalMesh = nullptr;

	UPROPERTY(EditAnywhere, Category = "Pawn")
	FTransform MeshTransform = FTransform::Identity;

	UPROPERTY(EditAnywhere, Category = "Pawn")
	float MaxHP = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Pawn")
	float HP = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Pawn")
	float BodyCollisionDamage = 0.0f;

public:
	UPROPERTY(EditAnywhere, Category = "Pawn|Collision")
	float CollisionCapsuleRadius = 22.f;
	UPROPERTY(EditAnywhere, Category = "Pawn|Collision")
	float CollisionCapsuleHalfHeight = 44.f;

public: // Animation
	UPROPERTY(EditAnywhere, Category = "Pawn|Animation")
	TArray<UAnimMontage*> HitReactMontage;
	UPROPERTY(EditAnywhere, Category = "Pawn|Animation")
	UAnimMontage* DieMontage = nullptr;

public: // Movement
	UPROPERTY(EditAnywhere, Category = "Pawn|Movement")
	float MovementMaxSpeed = 400.f;

//public: // AI (Enemy만 적용)
//	UPROPERTY(EditAnywhere, Category = "Pawn|AI")
//	TSubclassOf<AAIController> AIControllerClass;

public: // Weapon
	UPROPERTY(EditAnywhere, Category = "Pawn|Weapon", meta = (RowType = "/Script/CavesBasic.WeaponTableRow"))
	FDataTableRowHandle Weapon;
};