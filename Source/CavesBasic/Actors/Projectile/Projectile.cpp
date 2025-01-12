// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Projectile/Projectile.h"
#include "Misc/Utils.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Actors/Effect/EffectWithDecal.h"
#include "Actors/Weapon/WeaponBase.h"
#include "Actors/Player/BasicPlayer.h"
#include "Actors/Monster/DefaultMonster/DefaultMonster.h"


FProjectileTableRow::FProjectileTableRow()
	: ProjectileClass(AProjectile::StaticClass())
{
}

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
		
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	RootComponent = StaticMeshComponent;

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));

	StaticMeshComponent->SetCollisionProfileName(CollisionProfileName::DefaultProjectile);
	StaticMeshComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnBeginOverlap);

	ProjectileMeshEffectComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ProjectileMeshEffectComponent"));
	ProjectileMeshEffectComponent->SetupAttachment(StaticMeshComponent);
	ProjectileMeshEffectComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ProjectileMeshEffectComponent->bAutoActivate = false;

	OnDestroyed.AddDynamic(this, &ThisClass::OnProjectileDestroyed);
}

void AProjectile::SetData(const FDataTableRowHandle& InDataTableRowHandle)
{
	DataTableRowHandle = InDataTableRowHandle;
	if (DataTableRowHandle.IsNull()) { return; }
	FProjectileTableRow* Data = DataTableRowHandle.GetRow<FProjectileTableRow>(TEXT("Projectile"));
	if (!Data) { ensure(false); return; }

	ProjectileData = Data;

	StaticMeshComponent->MoveIgnoreActors.Empty();
	StaticMeshComponent->MoveIgnoreActors.Add(GetOwner());

	StaticMeshComponent->SetStaticMesh(Data->StaticMesh);
	StaticMeshComponent->SetRelativeTransform(Data->Transform);
	StaticMeshComponent->SetVisibility(Data->bIsVisible);

	ProjectileMovementComponent->InitialSpeed = Data->InitialSpeed;
	ProjectileMovementComponent->MaxSpeed = Data->MaxSpeed;
	ProjectileMovementComponent->ProjectileGravityScale = Data->ProjectileGravityScale;
	InitialLifeSpan = Data->ProjectileLifeSpan;

	ProjectileMeshEffectComponent->SetRelativeTransform(Data->MeshEffectTransform);
	ProjectileMeshEffectComponent->SetTemplate(Data->ProjectileMeshEffect);
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (ProjectileMeshEffectComponent)
	{
		ProjectileMeshEffectComponent->Activate();
	}
}

void AProjectile::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValid(this)) { return; }

	if (ProjectileMeshEffectComponent)
	{
		ProjectileMeshEffectComponent->Deactivate();
	}

	FTransform NewTransform = GetActorTransform();

	// JM : DecalEffect 출력
	{
		FProjectileTableRow* EffectTableRow = DataTableRowHandle.GetRow<FProjectileTableRow>(TEXT("Effect"));
		FEffectDecalTableRow* DecalEffectTableRow = EffectTableRow->EffectTableRowHandle.GetRow<FEffectDecalTableRow>(TEXT("DecalEffect"));

		AEffectWithDecal* DecalEffect = GetWorld()->SpawnActorDeferred<AEffectWithDecal>(DecalEffectTableRow->EffectWithDecalClass,
			FTransform::Identity);

		FDataTableRowHandle DecalTableRowHandle = EffectTableRow->EffectTableRowHandle;

		DecalEffect->SetData(DecalTableRowHandle);
		NewTransform.SetScale3D(DecalEffectTableRow->OverlapParticleTransform.GetScale3D());
		DecalEffect->FinishSpawning(NewTransform);
		// DecalEffect의 위치를 FinishSpawning 함수를 통해 조정하는 것일 뿐,
		// Play 함수를 호출하면 원점에 무조건 DecalEffect가 출력된다.
		DecalEffect->Play();
	}
	ABasicPlayer* OwningCharacter = Cast<ABasicPlayer>(GetOwner());
	check(OwningCharacter);

	// Skill Data Table 얻어오기
	const FSkillTableRow* SkillTableRow = OwningCharacter->GetSkillTableRow();
	ensure(SkillTableRow);

	// Skill Data Table 얻은 후 Projectile은 제거
	Destroy();

	UGameplayStatics::ApplyDamage(OtherActor, SkillTableRow->SkillDamage, GetInstigator()->GetController(), this, nullptr);
}

void AProjectile::OnProjectileDestroyed(AActor* DestroyedActor)
{
	
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

