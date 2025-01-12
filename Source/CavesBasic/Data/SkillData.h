#pragma once

#include "CoreMinimal.h"
#include "Actors/Projectile/Projectile.h"
#include "SkillData.generated.h"

USTRUCT()
struct CAVESBASIC_API FSkillTableRow : public FTableRowBase
{
	GENERATED_BODY()

public: // Animation
	UPROPERTY(EditAnywhere, Category = "Skill|Animation")
	TObjectPtr<UAnimMontage> SkillMotionMontage = nullptr;

public: // Damage
	UPROPERTY(EditAnywhere, Category = "Skill|Damage")
	float SkillDamage = 0.0f;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	TSubclassOf<AProjectile> ProjectileClass = AProjectile::StaticClass(); // 발사체 클래스
	UPROPERTY(EditAnywhere, meta = (RowType = "/Script/CavesBasic.ProjectileTableRow"))
	FDataTableRowHandle ProjectileRowHandle;
};