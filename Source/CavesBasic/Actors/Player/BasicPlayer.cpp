// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Player/BasicPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Misc/Utils.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ABasicPlayer::ABasicPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->SetCollisionProfileName(CollisionProfileName::Player);
	
	// SpringArm은 정상적인 Camera 작동을 위해 형식적으로 Camera의 부모로 설정해주었음.
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	{
		SpringArm->SetupAttachment(GetMesh());
		//SpringArm->ProbeSize = 5.0;
		SpringArm->TargetArmLength = 0.0;
		SpringArm->bUsePawnControlRotation = true;
		SpringArm->bInheritRoll = false;
	}

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	{
		Camera->SetupAttachment(SpringArm);

		Camera->SetRelativeLocation(FVector(-684.0, 500.0, 426.0));
		Camera->SetRelativeRotation(FRotator(-20.0, -40.0, 0.0));
		Camera->SetProjectionMode(ECameraProjectionMode::Orthographic);

		// Camera의  ProjectionMode가 Orthographic인 상황에서
		// 캐릭터 웅크리기 시 화면이 내려가는 것을 막아줌
		Camera->SetConstraintAspectRatio(true);

		bUseControllerRotationYaw = false;
	}

	WeaponChildActorComponent = CreateDefaultSubobject<UWeaponChildActorComponent>(TEXT("WeaponChildActorComponent"));
	{
		WeaponChildActorComponent->SetupAttachment(GetMesh(), SocketName::Weapon);
	}

	StatusComponent = CreateDefaultSubobject<UStatusComponent>(TEXT("StatusComponent"));

	CharacterMeshEffectMgr = CreateDefaultSubobject<UCharacterMeshEffect>(TEXT("CharacterMeshEffectMgr"));
	
	//UCharacterMovementComponent* Movement = GetCharacterMovement();
	//Movement->bCanWalkOffLedges = false;
}

void ABasicPlayer::OnDie()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	check(AnimInstance);
	AnimInstance->Montage_Pause(CurrentDieMontage);

	GetMovementComponent()->DestroyComponent();
	WeaponChildActorComponent->DestroyComponent();
}

void ABasicPlayer::ResetCharacterStatus()
{
	if(StatusComponent)
	{
		StatusComponent->RespawnStatus();
	}
	else
	{
		ensure(false);
		UE_LOG(LogTemp, Warning, TEXT("StatusComponent is nullptr."));
	}
}

UAnimMontage* ABasicPlayer::GetPlayingMontage() const
{
	return CurrentActiveMontage;
}

FSkillTableRow* ABasicPlayer::GetSkillTableRow() const
{

	USkeletalMeshComponent* MeshComponent = GetComponentByClass<USkeletalMeshComponent>();

	const UAnimMontage* CurrentActiveAnimMontage = GetPlayingMontage();

	AActor* WeaponActor = WeaponChildActorComponent->GetChildActor();
	AWeaponBase* Weapon = Cast<AWeaponBase>(WeaponActor);
	check(Weapon);

	const FWeaponTableRow* WeaponTableRow = Weapon->GetWeaponTableRow();
	ensure(WeaponTableRow);

	// JM
	// 해당 Notify는 이미 존재하는 몽타주에 적용됨.
	// 몽타주는 특정 스킬 사용 시 출력되는 애니메이션임.
	// 즉 몽타주는 스킬에 종속적이기 때문에 스폰되어야 하는 Projectile이 정해져 있음.
	// Projectile의 데이터 테이블을 찾아 그 데이터 테이블에 있는 데이터를 적용해야 함.
	// 이를 위해 스킬을 관리하는 배열을 하나씩 접근하여 그곳에 있는 스킬 데이터 테이블을
	// 다시 찾아 보면서 Notify를 발생시킨 애니메이션을 찾아냄.
	// Notify를 발생시킨 애니메이션과 일치하는 애니메이션이 있는 스킬 배열의 인덱스를 
	// 저장하고 그 스킬 배열에 있는 데이터 테이블에 접근하여 Projectile 데이터 테이블에 있는
	// 데이터를 적용하여 스폰함.
	TArray<FDataTableRowHandle> SkillTableHandle = WeaponTableRow->SkillRowHandle;
	int32 SkillRowMaxIndex = SkillTableHandle.Num();
	int32 AnimationFoundIndex = -1;
	for (int i = 0; i < SkillRowMaxIndex; ++i)
	{
		const FSkillTableRow* SkillTableRow = SkillTableHandle[i].GetRow<FSkillTableRow>(TEXT("Auto"));
		UAnimMontage* AnimMontage = SkillTableRow->SkillMotionMontage;
		if (AnimMontage == CurrentActiveAnimMontage)
		{
			AnimationFoundIndex = i;
			break;
		}
	}

	FSkillTableRow* SkillTableRow = WeaponTableRow->SkillRowHandle[AnimationFoundIndex].GetRow<FSkillTableRow>(TEXT("Auto"));
	ensure(SkillTableRow);

	return SkillTableRow;
}

// Called when the game starts or when spawned
void ABasicPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	// 생성자에서 초기화를 진행하면 
	// 독립형 모드로 게임 실행 시, this(플레이어 객체)와 
	// CharacterMeshEffectMgr의 Owner가 일치하지 않는 문제 발생.
	
	// 생성자에서 SetOwner(this)를 호출할 때, 
	// 플레이어 또는 CharacterMeshEffectMgr 객체가 아직 완전히 초기화되지 않은 상태일 수 있음.
	CharacterMeshEffectMgr->SetOwner(this);
	CharacterMeshEffectMgr->SetMesh(GetMesh());
}

void ABasicPlayer::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	SetData(DataTableRowHandle);
}

void ABasicPlayer::SetData(FDataTableRowHandle InDataTableRowHandle)
{
	DataTableRowHandle = InDataTableRowHandle;
	if (DataTableRowHandle.IsNull()) { return; }
	FPawnTableRow* Data = DataTableRowHandle.GetRow<FPawnTableRow>(TEXT("Player"));
	if (!Data) { ensure(false); return; }

	CharacterData = Data;
	
	// CapsuleComponent 관련
	{
		UCharacterMovementComponent* Movement = GetCharacterMovement();
		//Movement->RotationRate = CharacterData->RotationRate;
		Movement->bOrientRotationToMovement = true;
		Movement->GetNavAgentPropertiesRef().bCanCrouch = true;

		const float NewCapsuleHalfHeight = CharacterData->CollisionCapsuleHalfHeight * 0.5f;
		Movement->SetCrouchedHalfHeight(NewCapsuleHalfHeight);
	}
	{
		UCapsuleComponent* Capsule = GetCapsuleComponent();
		if (!FMath::IsNearlyEqual(Capsule->GetUnscaledCapsuleHalfHeight(), CharacterData->CollisionCapsuleHalfHeight))
		{
			Capsule->SetCapsuleHalfHeight(CharacterData->CollisionCapsuleHalfHeight/* * 2.f*/, false);
		}
	}

	USkeletalMeshComponent* SkeletalMeshComponent = GetMesh();
	SkeletalMeshComponent->SetSkeletalMesh(CharacterData->SkeletalMesh);
	SkeletalMeshComponent->SetRelativeTransform(CharacterData->MeshTransform);

	UCharacterMovementComponent* Movement = GetCharacterMovement();
	Movement->bOrientRotationToMovement = true;
	Movement->MaxWalkSpeed = CharacterData->MovementMaxSpeed;

	StatusComponent->SetHP(CharacterData->MaxHP, CharacterData->HP);
	StatusComponent->SetBodyCollisionDamage(CharacterData->BodyCollisionDamage);
}

void ABasicPlayer::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{

	RecalculateBaseEyeHeight();
	FVector& MeshRelativeLocation = GetMesh()->GetRelativeLocation_DirectMutable();
	MeshRelativeLocation.Z = CharacterData->MeshTransform.GetLocation().Z + HalfHeightAdjust;
	BaseTranslationOffset.Z = MeshRelativeLocation.Z;

	K2_OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

void ABasicPlayer::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	RecalculateBaseEyeHeight();

	FVector& MeshRelativeLocation = GetMesh()->GetRelativeLocation_DirectMutable();
	MeshRelativeLocation.Z = CharacterData->MeshTransform.GetLocation().Z;
	BaseTranslationOffset.Z = MeshRelativeLocation.Z;

	K2_OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

// Called every frame
void ABasicPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABasicPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float ABasicPlayer::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (StatusComponent)
	{
		if (StatusComponent->IsDie()) { return 0.f; }

		float DamageResult = StatusComponent->TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
		if (FMath::IsNearlyZero(DamageResult)) { return 0.0; }

		if (Controller)
		{
			Controller->StopMovement();
		}

		StatusComponent->ActivateInvincibility(InvincibilityTime);  // x초간 무적 상태 부여

		// 무적 시간 동안 캐릭터의 메시가 깜빡거리도록 설정
		// JM@TODO : 여기도 간혹 문제 발생
		if (CharacterMeshEffectMgr)
		{
			CharacterMeshEffectMgr->ApplyHitMaterial(InvincibilityTime);
		}

		if (StatusComponent->IsDie())
		{
			ensure(CharacterData->DieMontage);

			// Die Montage가 배열로 선언되어 여러 개 존재하는 경우 아래 코드 사용
			///*const int64 Index = FMath::RandRange(0, CharacterData->DieMontage.Num() - 1);
			//CurrentDieMontage = CharacterData->DieMontage[Index];*/

			UCapsuleComponent* Capsule = GetCapsuleComponent();
			ensure(Capsule);
			Capsule->SetCollisionProfileName(CollisionProfileName::PlayerDeath);

			CurrentDieMontage = CharacterData->DieMontage;

			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			AnimInstance->Montage_Play(CurrentDieMontage);

			UKismetSystemLibrary::K2_SetTimer(this, TEXT("OnDie"),
				CharacterData->DieMontage->GetPlayLength() - 0.5f, false);
		}
		else if (!StatusComponent->IsDie() && !CharacterData->HitReactMontage.IsEmpty())
		{
			const int64 HitReactIndex = FMath::RandRange(0, CharacterData->HitReactMontage.Num() - 1);
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			AnimInstance->Montage_Play(CharacterData->HitReactMontage[HitReactIndex]);
		}
	}
	else
	{
		// StatusComponent가 nullptr인 경우 처리
		UE_LOG(LogTemp, Warning, TEXT("StatusComponent is nullptr in TakeDamage()"));
	}
	
	return 0.0f;
}