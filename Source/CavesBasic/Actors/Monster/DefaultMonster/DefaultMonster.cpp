// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Monster/DefaultMonster/DefaultMonster.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Misc/Utils.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "UI/HUDWidget/UserWidgetBase.h"
#include "Actors/Weapon/WeaponBase.h"
#include "Actors/Player/BasicPlayer.h"


// Sets default values
ADefaultMonster::ADefaultMonster()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GetCapsuleComponent()->SetCollisionProfileName(CollisionProfileName::DefaultMonster);	
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnBeginOverlap);

	// 기본적으로 AIControllerClass를 설정하여 ADefaultMonsterAIController를 사용할 수 있도록 한다
	AIControllerClass = ADefaultMonsterAIController::StaticClass();

	// HP Bar
	{
		ConstructorHelpers::FClassFinder<UUserWidget> WidgetClass(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/UI_DefaultMonsterHPBar.UI_DefaultMonsterHPBar_C'"));
		HPWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPWidgetComponent"));
		HPWidgetComponent->SetupAttachment(RootComponent);
		HPWidgetComponent->SetRelativeLocation(FVector(0., 0., 130.0));
		HPWidgetComponent->SetDrawSize(FVector2D(256.3, 17.0));
		HPWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
		HPWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		HPWidgetComponent->SetWidgetClass(WidgetClass.Class);
	}

	WeaponChildActorComponent = CreateDefaultSubobject<UWeaponChildActorComponent>(TEXT("WeaponChildActorComponent"));
	{
		WeaponChildActorComponent->SetupAttachment(GetMesh(), SocketName::Weapon);
	}

	StatusComponent = CreateDefaultSubobject<UStatusComponent>(TEXT("StatusComponent"));
	StatusComponent->SetIsPlayer(false);

	// 몬스터 사망 시 점점 사라지는 효과를 주기 위한 멤버 변수들
	DisappearTimelineComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("DisappearTimelineComponent"));
	DisappearCurve = CreateDefaultSubobject<UCurveFloat>(TEXT("DisappearCurve"));
}

FSkillTableRow* ADefaultMonster::GetSkillTableRow() const
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

void ADefaultMonster::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->IsA(ABasicPlayer::StaticClass()))
	{
		ABasicPlayer* Player = Cast<ABasicPlayer>(OtherActor);
		if (Player && !Player->GetStatusComponent()->IsInvincible())  // 상대방이 이미 무적 상태가 아니면
		{
			float MyBodyDmg = StatusComponent->GetBodyCollisionDamage();
			// 플레이어에게 데미지를 줌
			UGameplayStatics::ApplyDamage(OtherActor, MyBodyDmg, GetInstigator()->GetController(), this, nullptr);
		}
	}
}

void ADefaultMonster::OnDie()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->Montage_Pause(CurrentDieMontage);
}

void ADefaultMonster::OnDisappearMesh(float InDissolve)
{
	if (MaterialInstanceDynamics)
	{
		USkeletalMeshComponent* SkeletalMeshComponent = GetComponentByClass<USkeletalMeshComponent>();
		// MaterialInstanceDynamics가 가리키고 있는, BlendMode가 Translucent로 설정된
		// Material을 Monster의 0번 Material로 설정해 준다.
		SkeletalMeshComponent->SetMaterial(0, MaterialInstanceDynamics);

		// CurrentTransparency의 초깃값은 1이다.
		float CurrentTransparency;
		MaterialInstanceDynamics->GetScalarParameterValue(FName("Opacity"), CurrentTransparency);

		float SpeedMultiplier = 0.005f; // 낮을수록 투명도 감소 속도를 더 천천히 만듭니다.
		
		// CurrentTransparency의 값을 점점 감소시켜 몬스터가 점점 투명해지도록 한다.
		float NewTransparency = FMath::Max(CurrentTransparency - InDissolve * SpeedMultiplier, 0.0f); // Max 함수는 첫 번째 인자의 값이 음수가 나오면 0.0f을 반환해 준다.
		MaterialInstanceDynamics->SetScalarParameterValue(FName("Opacity"), NewTransparency);
	}
}

void ADefaultMonster::OnDisappearMeshEnd()
{
	Destroy();
}

// Called when the game starts or when spawned
void ADefaultMonster::BeginPlay()
{
	Super::BeginPlay();
	
	SetData(DataTableRowHandle);

	USkeletalMeshComponent* SkeletalMeshComponent = GetComponentByClass<USkeletalMeshComponent>();

	// 1번 인덱스에 있는 Material이 BlendMode가 Translucent로 설정되어, Opcity를 수정할 수 있는 Material이다.
	// MaterialInstanceDynamics는 1번 인덱스에 있는 Material을 가리키는 멤버 변수
	MaterialInstanceDynamics = SkeletalMeshComponent->CreateAndSetMaterialInstanceDynamic(1);
	ensureMsgf(MaterialInstanceDynamics->GetBlendMode() == EBlendMode::BLEND_Translucent && MaterialInstanceDynamics, TEXT("1번 인덱스에 존재하는 Material이 없거나, BlendMode가 Translucent가 아닙니다."));

	if (DisappearCurve)	// DisappearCurve 값 세팅
	{
		// 몬스터 사망 모션은 하나로 함
		float DieMontagePlayLength = MonsterData->DieMontage->GetPlayLength();
		
		// 키프레임 추가
		FKeyHandle KeyHandle1 = DisappearCurve->FloatCurve.AddKey(0.0f, 1.0f);  // 시간 0에서 값 1
		// 값이 점점 증가하는 Curve
		FKeyHandle KeyHandle3 = DisappearCurve->FloatCurve.AddKey(DieMontagePlayLength, DieMontagePlayLength);  // 시간 DieMontagePlayLength에서 값 DieMontagePlayLength

		DisappearCurve->FloatCurve.SetKeyInterpMode(KeyHandle1, RCIM_Cubic);  // 선형 보간
		DisappearCurve->FloatCurve.SetKeyInterpMode(KeyHandle3, RCIM_Cubic);
	}
	FOnTimelineFloat Delegate;
	Delegate.BindDynamic(this, &ThisClass::OnDisappearMesh);
	// Delegate와 연동된, 즉 OnDisappearMesh 함수가 호출될 때 DisappearCurve를 인자로 넘긴다.
	DisappearTimelineComponent->AddInterpFloat(DisappearCurve, Delegate);

	FOnTimelineEvent EndDelegate;
	EndDelegate.BindDynamic(this, &ThisClass::OnDisappearMeshEnd);
	DisappearTimelineComponent->SetTimelineFinishedFunc(EndDelegate);

	// Monster HP Bar Owner Setting	
	UUserWidgetBase* UserWidget = Cast<UUserWidgetBase>(HPWidgetComponent->GetWidget());
	check(UserWidget);
	UserWidget->SetOwningPawn(this);

	// AI
	if (AIControllerClass)
	{
		AAIController* AIController = Cast<AAIController>(GetController());
		if (!AIController)
		{
			// AIController가 없다면 새로 할당
			AIController = GetWorld()->SpawnActor<AAIController>(AIControllerClass);
			if (AIController)
			{
				PossessedBy(AIController);  // AIController가 몬스터를 제어하도록 설정
			}
		}
	}
}

void ADefaultMonster::OnConstruction(const FTransform& Transform)
{
	SetData(DataTableRowHandle);
}

void ADefaultMonster::SetData(FDataTableRowHandle InDataTableRowHandle)
{
	DataTableRowHandle = InDataTableRowHandle;
	if (DataTableRowHandle.IsNull()) { return; }
	FPawnTableRow* Data = DataTableRowHandle.GetRow<FPawnTableRow>(TEXT("Monster"));
	if (!Data) { ensure(false); return; }

	MonsterData = Data;

	// CapsuleComponent 관련
	{
		UCharacterMovementComponent* Movement = GetCharacterMovement();
		//Movement->RotationRate = CharacterData->RotationRate;
		Movement->bOrientRotationToMovement = true;
		Movement->GetNavAgentPropertiesRef().bCanCrouch = true;

		const float NewCapsuleHalfHeight = MonsterData->CollisionCapsuleHalfHeight * 0.5f;
		Movement->SetCrouchedHalfHeight(NewCapsuleHalfHeight);
	}
	{
		UCapsuleComponent* Capsule = GetCapsuleComponent();
		if (!FMath::IsNearlyEqual(Capsule->GetUnscaledCapsuleHalfHeight(), MonsterData->CollisionCapsuleHalfHeight))
		{
			Capsule->SetCapsuleHalfHeight(MonsterData->CollisionCapsuleHalfHeight, false);
		}
		Capsule->SetCapsuleRadius(MonsterData->CollisionCapsuleRadius);
	}

	USkeletalMeshComponent* SkeletalMeshComponent = GetMesh();
	SkeletalMeshComponent->SetSkeletalMesh(MonsterData->SkeletalMesh);
	SkeletalMeshComponent->SetRelativeTransform(MonsterData->MeshTransform);

	UCharacterMovementComponent* Movement = GetCharacterMovement();
	Movement->bOrientRotationToMovement = true;
	Movement->MaxWalkSpeed = MonsterData->MovementMaxSpeed;

	StatusComponent->SetHP(MonsterData->MaxHP, MonsterData->HP);
	StatusComponent->SetBodyCollisionDamage(MonsterData->BodyCollisionDamage);

}

// Called every frame
void ADefaultMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADefaultMonster::Falling()
{
}

// Called to bind functionality to input
void ADefaultMonster::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

float ADefaultMonster::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (StatusComponent->IsDie()) { return 0.f; }
	
	if (Controller)
	{
		Controller->StopMovement();
	}

	float DamageResult = StatusComponent->TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (FMath::IsNearlyZero(DamageResult)) { return 0.0; }

	if (StatusComponent->IsDie() && MonsterData->DieMontage)
	{
		// UnPossess()를 호출하여 제어를 멈추기
		if (Controller)
		{
			Controller->UnPossess();     // Controller 비활성화
		}

		WeaponChildActorComponent->DestroyComponent();
		HPWidgetComponent->DestroyComponent();

		SetActorEnableCollision(false);

		/*const int64 Index = FMath::RandRange(0, MonsterData->DieMontage.Num() - 1);
		CurrentDieMontage = MonsterData->DieMontage[Index];*/
		CurrentDieMontage = MonsterData->DieMontage; 
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		check(AnimInstance);
		AnimInstance->Montage_Play(CurrentDieMontage);
		
		// 몬스터 사망 모션 재생이 시작되면 Mesh가 점점 투명해짐
		// BeginPlay에서 DisappearTimelineComponent에 Delegate가 연동됨
		// DisappearTimelineComponent->Play을 통해 Delegate를 호출하면
		// 인자로 DisappearCurve를 넘기며 OnDisappearMesh 함수가 실행됨.
		// OnDisappearMesh 함수에서 DisappearCurve의 값을 통해 Mesh를 점점 투명하게 해 주고 있음.
		// OnDisappearMesh 함수가 끝나면 Delegate가 끝난 것이므로 OnDisappearMeshEnd 함수를 호출하여
		// Monster를 제거한다.
		DisappearTimelineComponent->Play();
		
		UKismetSystemLibrary::K2_SetTimer(this, TEXT("OnDie"),
			MonsterData->DieMontage->GetPlayLength() - 0.5f, false);
		// 위 코드가 끝난 후에 OnDie가 호출됨
	}
	else if (!StatusComponent->IsDie() && !MonsterData->HitReactMontage.IsEmpty())
	{
		const int64 HitReactIndex = FMath::RandRange(0, MonsterData->HitReactMontage.Num() - 1);
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		AnimInstance->Montage_Play(MonsterData->HitReactMontage[HitReactIndex]);
	}

	return 0.0f;
}

