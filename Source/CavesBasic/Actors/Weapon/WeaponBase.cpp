// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Weapon/WeaponBase.h"
#include "Engine/StaticMeshSocket.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/Character.h"
#include "Actors/Player/BasicPlayer.h"
#include "Actors/Monster/DefaultMonster/DefaultMonster.h"


// Weapon DataTable
FWeaponTableRow::FWeaponTableRow()
	: WeaponClass(AWeaponBase::StaticClass())
	//, SkillClass(UGameplayAbility::StaticClass())
{
	WeaponAttackMontage = nullptr;
}

// Sets default values
AWeaponBase::AWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	RootComponent = DefaultSceneRoot;

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	SkeletalMeshComponent->SetupAttachment(RootComponent);
	SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(RootComponent);
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Input
	{
		static ConstructorHelpers::FObjectFinder<UInputMappingContext> Asset
		{ TEXT("/Script/EnhancedInput.InputMappingContext'/Game/Actors/Player/Input/WeaponInput/IMC_Weapon.IMC_Weapon'") };
		check(Asset.Object);

		IMC_Weapon = Asset.Object;
	}
}

void AWeaponBase::SetData(const FDataTableRowHandle& InDataTableRowHandle)
{
	DataTableRowHandle = InDataTableRowHandle;
	if (DataTableRowHandle.IsNull()) { return; }
	FWeaponTableRow* Data = DataTableRowHandle.GetRow<FWeaponTableRow>(TEXT("Weapon"));
	if (!Data) { ensure(false); return; }
	WeaponTableRow = Data;

	if (Data->SkeletalMesh && Data->StaticMesh)
	{
		ensureMsgf(false, TEXT("둘 중에 하나만 지정 하십시오"));
		return;
	}


	if(Data->SkeletalMesh)
	{
		// 무기에 존재하는 손잡이(Grip) 소켓의 Location을 얻어와
		// 값을 반전시킨 후 적용시켜 무기의 Mesh Location 값을 조정해 준다.
		// 값을 반전시키는 이유는 Grip의 위치가 올라갈수록(+) Mesh의 위치는 내려가기(-) 때문
		SkeletalMeshComponent->SetSkeletalMesh(Data->SkeletalMesh);
		//SkeletalMeshComponent->SetRelativeTransform(Data->Transform);
		const USkeletalMeshSocket* SkeletalMeshGripSocket = SkeletalMeshComponent->GetSocketByName(SocketName::Grip);
		ensure(SkeletalMeshGripSocket);
		FVector SkeletalMeshFinalLocation = SkeletalMeshGripSocket->RelativeLocation;
		SkeletalMeshComponent->SetRelativeLocation(-SkeletalMeshFinalLocation);
	}

	if (Data->StaticMesh)
	{
		StaticMeshComponent->SetStaticMesh(Data->StaticMesh);
		//StaticMeshComponent->SetRelativeTransform(Data->Transform);
		const UStaticMeshSocket* StaticMeshGripSocket = StaticMeshComponent->GetSocketByName(SocketName::Grip);
		ensure(StaticMeshGripSocket);
		FVector StaticMeshFinalLocation = StaticMeshGripSocket->RelativeLocation;
		StaticMeshComponent->SetRelativeLocation(-StaticMeshFinalLocation);
	}

	check(Data->AnimClass);
	USkeletalMeshComponent* MeshComponent = GetOwner()->GetComponentByClass<USkeletalMeshComponent>();
	check(MeshComponent);
	MeshComponent->SetAnimClass(Data->AnimClass);

	// Owner 정보 세팅
	{
		OwningPawn = Cast<APawn>(GetOwner());
		check(OwningPawn);
		OwningPawnBodyMesh = OwningPawn->GetComponentByClass<USkeletalMeshComponent>();
		check(OwningPawnBodyMesh);
		BasicAnimInstance = Cast<UBasicPlayerAnimInstance>(MeshComponent->GetAnimInstance());
		check(BasicAnimInstance);

		//OwnerStatusComponent = OwningPawn->GetComponentByClass<UStatusComponent>();
		//check(OwnerStatusComponent);
		//{
		//	BasicAnimInstance->OnMontageEnded.AddDynamic(this, &ThisClass::OnMontageEnd);
		//}
	}

	// Input
	SkillRowHandleNum = WeaponTableRow->SkillRowHandle.Num();
	{
		if (APlayerController* PC = Cast<APlayerController>(OwningPawn->GetController()))
		{
			UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
			if (!Subsystem->HasMappingContext(IMC_Weapon))
			{
				Subsystem->AddMappingContext(IMC_Weapon, 0);
			}

			if (!InputComponent)
			{
				EnableInput(PC);
				UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
				ensure(EnhancedInputComponent);
				if (const UInputAction* InputAction = FUtils::GetInputActionFromName(IMC_Weapon, TEXT("IA_Attack")))
				{
					EnhancedInputComponent->BindAction(InputAction, ETriggerEvent::Triggered, this, &ThisClass::OnAttack);
				}
				// 스킬의 InputAction 파일명은 " IA_Skill + 스킬번호 " 규칙을 따른다.
				// SkillDataTable의 Row 값도 마찬가지.
				if (const UInputAction* InputAction = FUtils::GetInputActionFromName(IMC_Weapon, TEXT("IA_Skill1")))
				{
					EnhancedInputComponent->BindAction(InputAction, ETriggerEvent::Triggered, this, &ThisClass::OnSkill);
				}
				if (const UInputAction* InputAction = FUtils::GetInputActionFromName(IMC_Weapon, TEXT("IA_Skill2")))
				{
					EnhancedInputComponent->BindAction(InputAction, ETriggerEvent::Triggered, this, &ThisClass::OnSkill);
				}
			}
		}
	}
}

void AWeaponBase::OnAttack(const FInputActionInstance& Instance)
{
	if (WeaponTableRow->WeaponAttackMontage)

	{
		// 모든 몽타주가 재생 중이지 않을 때
		if (!BasicAnimInstance->Montage_IsPlaying(nullptr))
		{
			ACharacter* OwningCharacter = Cast<ACharacter>(OwningPawn);
			if(OwningCharacter->bIsCrouched){	/*캐릭터가 앉아있는 상태라면 앉은상태공격 모션 재생*/ }
			else
			{
				BasicAnimInstance->Montage_Play(WeaponTableRow->WeaponAttackMontage);
			}
		}
	}
}

void AWeaponBase::OnSkill(const FInputActionInstance& Instance)
{
	ACharacter* OwningCharacter = Cast<ACharacter>(OwningPawn);
	if(!OwningCharacter->bIsCrouched)
	{
		// 호출된 InputAction을 통해 어떤 키가 입력되었는지 확인
		const UInputAction* TriggeredAction = Instance.GetSourceAction();
		FString ActionName = TriggeredAction->GetName();
		// 스킬 번호만 남김
		ActionName.RemoveFromStart(TEXT("IA_Skill"));
		int32 ExecutedSkillNum = FCString::Atoi(*ActionName);

		// 어떤 키가 입력되었느냐에 따라 다른 스킬을 실행함
		const FString Skill_Number = TEXT("Skill") + FString::FromInt(ExecutedSkillNum);

		if (SkillRowHandleNum >= ExecutedSkillNum)
		{
			FSkillTableRow* SkillRow = WeaponTableRow->SkillRowHandle[ExecutedSkillNum - 1].GetRow<FSkillTableRow>(Skill_Number);

			if (!SkillRow) { ensure(false); return; }

			UAnimMontage* CurrentMontage = BasicAnimInstance->GetCurrentActiveMontage();

			// 모든 몽타주가 재생 중이지 않을 때
			if (nullptr == CurrentMontage)
			{
				// JM
				// 재생된 몽타주를 캐릭터(플레이어 또는 몬스터)가 멤버 변수로 갖고 있게 함.
				// 나중에 Projectile이 Overlap되면서 Skill Data Table에 있는 데미지값을 통해
				// 상대방에게 데미지를 주는데 이때 현재 재생 중인 애니메이션을 통해 Skill Data Table을
				// 찾고 있으므로 멤버 변수로 저장해 주어야 한다.
				// 그렇지 않으면 스킬 모션 몽타주의 재생 시간보다 Projectile의 살아있는 시간이 긴 경우,
				// Projectile이 Overlap 시점에 현재 재생 중인 애니메이션을 찾지 못해 
				// Skill Data Table에 접근할 수 없고, 데미지를 적용시킬 수 없다.

				if (ABasicPlayer* WeaponOwner = Cast<ABasicPlayer>(OwningCharacter))
				{
					UStatusComponent* StatusComponent = WeaponOwner->GetComponentByClass<UStatusComponent>();
					if (StatusComponent->IsPlayer())
					{
						UAnimMontage* PlayingMontage = WeaponOwner->GetPlayingMontage();
						if (PlayingMontage)
						{
							// 세팅된 Montage는 아래 코드를 만나기 전까지 이전에 세팅된 값을 계속 들고 있는 상태임
							// 따라서 WeaponOwner가 아무런 몽타주를 재생하고 있지 않는 상태임에도
							// 다른 곳에서 GetPlayingMontage 함수를 호출했을 때 특정 몽타주가 반환될 수 있으므로 주의
							WeaponOwner->SetPlayingMontage(nullptr);
						}
						
						WeaponOwner->SetPlayingMontage(SkillRow->SkillMotionMontage);
					}
					else
					{
						if (ADefaultMonster* WeaponOwnerIsMonster = Cast<ADefaultMonster>(OwningCharacter))
						{
							UAnimMontage* PlayingMontage = WeaponOwnerIsMonster->GetPlayingMontage();
							if (PlayingMontage)
							{
								WeaponOwnerIsMonster->SetPlayingMontage(nullptr);
							}
							WeaponOwnerIsMonster->SetPlayingMontage(SkillRow->SkillMotionMontage);
						}
					}
				}
				BasicAnimInstance->Montage_Play(SkillRow->SkillMotionMontage);

				// 몽타주가 종료될 때 호출될 델리게이트 바인딩
				//BasicAnimInstance->OnMontageEnded.AddDynamic(this, &AWeaponBase::OnSkillMotionMontageEnded);
			}
		}
	}
}

void AWeaponBase::OnSkillMotionMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	
}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


