# CavesBasic 개인 프로젝트

## 📑 목차
- [🎮 프로젝트 개요](#-프로젝트-개요)
- [🛠️ 기술 스택](#️-기술-스택)
- [⚡ 빌드 및 실행 방법](#-빌드-및-실행-방법)
- [✨ 주요 기능 요약](#-주요-기능-요약)
- [💡 설계 포인트](#-설계-포인트)
- [🧠 프로젝트 구조](#-프로젝트-구조)
- [⚠️ 트러블슈팅](#️-트러블슈팅)
- [🔎 세부 구현 (주요 코드/로직)](#-세부-구현-주요-코드로직)
  - [Player](#1-player)
  - [Projectile](#2-projectile)
  - [Monster](#3-monster)
- [🖥️ UI](#️-ui)
- [📊 데이터 관리](#-데이터-관리)
- [📝 참고 사항](#-참고-사항)
- [🧩 회고](#-회고)
- [✉️ Contact](#️-contact)

---

## 🎮 프로젝트 개요
| 항목        | 내용                              |
|-------------|-----------------------------------|
| **장르**        | 탑다운 판타지 RPG                 |
| **컨셉**        | 어두운 동굴 속 전투와 생존         |
| **목표**        | C++ 및 Unreal Engine 5.4 기반 핵심 RPG 시스템 구현 |
| **기간**        | 2024.10.15 ~ 2024.11.15           |
| **개발자**      | 원재민                            |

![alt text](README_content/main2.png "Title Text")
![alt text](README_content/main.png "Title Text")

---

## 🛠️ 기술 스택
- **언어** : C++
- **게임 엔진** : Unreal Engine 5.4
- **버전 관리** : Sourcetree, GitHub
- **IDE** : Visual Studio 2022

---

## ⚡ 빌드 및 실행 방법

1. **필수 환경**
   - Unreal Engine 5.4
   - Visual Studio 2022
   - Windows 10/11 64bit

2. **프로젝트 클론**
    ```bash
    git clone https://github.com/WJMcode/Project_CavesBasic.git
    ```

3. **프로젝트 열기**
   - `CavesBasic.uproject` 파일을 더블 클릭하거나, 언리얼 에디터에서 직접 열기

4. **Visual Studio 프로젝트 생성 및 빌드**
   - `CavesBasic.uproject` 파일 우클릭 → `Generate Visual Studio project files`
   - 생성된 `.sln` 파일을 Visual Studio에서 열고 빌드 실행 (`Ctrl + Shift + B`)

5. **게임 실행**
   - 언리얼 에디터에서 `Play` 클릭

---

## ✨ 주요 기능 요약

- **Player**  
  - 4방향 이동, 점프, 웅크리기가 가능합니다.  
  - 무기를 습득하면 고유 스킬을 사용할 수 있습니다.  
  - 피격 시 **무적 상태와 깜빡임 효과**가 적용되며, 사망 시 **리스폰**이 가능합니다.
- **Projectile**
  - **Ground Projectile** : 플레이어 전방의 지형을 **자동 감지**하여, **감지한 지면 위에 생성**됩니다.
  - **Straight Projectile** : 적을 향해 일직선으로 날아가는 **추적형 발사체**입니다.
- **Monster**  
  - 랜덤 AI를 기반으로 움직이며, 사망 시 점진적으로 투명화되어 자연스럽게 제거됩니다.
- **UI**  
  - 플레이어 및 몬스터의 체력바를 표시하며, 사망 시 리스폰 창을 통한 상태 전환을 지원합니다.

---

## 💡 설계 포인트

- **데이터 중심 구조**  
  캐릭터, 몬스터, 무기, 스킬 등의 핵심 데이터를 DataTable 기반으로 관리하여, **확장성과 유지보수에 용이한 구조**로 구현했습니다.

- **Blueprint + C++ 혼용 설계**  
**UI 및 단순 상호작용은 Blueprint**, **핵심 시스템 로직은 C++** 로 분리하여 **개발 효율과 성능**을 모두 고려한 구조입니다.
  
- **에디터 친화적 & 하드코딩 최소화**  
  대부분의 설정을 에디터에서 조정 가능하도록 설계하여, **디자이너 및 기획자와 협업**하기 용이하며 **유연한 밸런싱 조정**이 가능합니다.

---

## 🧠 프로젝트 구조

```text
Project_CavesBasic/
├── Source/CavesBasic/
│   ├── Actors/         # 플레이어, 몬스터 등 주요 액터
│   ├── Components/     # 플레이어 상태 등 기능별 컴포넌트
│   ├── UI/             # 위젯, HUD
│   └── GameFrameWork/  # GameMode 등
├── Config/
├── Content/
├── CavesBasic.uproject
└── README.md
```
---

## ⚠️ 트러블슈팅
- **Straight Projectile 추적 문제**
  - **문제 배경** : Straight Projectile이 몬스터 가까이에서 생성될 때, 몬스터를 제대로 추적하지 못했습니다.
  - **원인** : 몬스터의 방향이 아니라 위치만 추적하여, 발사체가 올바르게 따라가지 못한 것이 원인이었습니다.
  - **해결 과정** : Tick마다 몬스터의 방향을 체크하고, 발사체를 해당 방향으로 회전시키도록 수정하였습니다.
  - **결과** : 발사체가 몬스터를 지속적으로 정확히 추적해 자연스러운 타격이 가능해졌습니다.

- **몬스터 사망 시 투명화 효과 구현 실패**
  - **문제 배경** : 몬스터 사망 시 점점 투명해지는 효과를 구현하려 했으나, 머티리얼의 Opacity 조정이 불가능했습니다.
  - **원인** : 기존 머티리얼의 블렌드 모드가 Opaque로 설정되어 있었습니다.
  - **시도** : Opacity를 수정할 수 있도록 블렌드 모드를 바꾸어 적용하였으나, 몬스터 외형의 퀄리티가 떨어졌습니다.
  - **해결 과정** : 블렌드 모드가 Translucent인 머티리얼을 별도로 만들어 사망 시에만 교체하고, Opacity 값을 점차 낮추는 방식으로 수정했습니다.
  - **결과** : 몬스터가 사망할 때 자연스러운 시각 효과를 연출할 수 있었습니다.

---
 
## 🔎 세부 구현 (주요 코드/로직)

### 1. Player

- **피격 시 시각 효과** : 플레이어가 피격되면 Overlay 머티리얼의 Opacity 값을 조정해 **깜빡이는 효과**를 연출합니다.

- **✍️ 핵심 로직**
  - 피격 시 Overlay 머티리얼을 **동적 인스턴스로 생성**하여 `HitOverlayOpacity` 값을 조절합니다.
  - 타이머를 통해 **깜빡임 효과를 반복**하고, 일정 시간이 지나면 **머티리얼을 원래대로 복원**합니다.
  - 사망 상태일 경우, **효과 지속 시간이 더 짧게 설정**됩니다.

<br>

> 💡 플레이어 피격 시 깜빡임 효과 (실제 게임 내 모습)
> <br>
> <br>
> ![blinkCha](https://github.com/user-attachments/assets/394c9701-0187-46b3-941f-3b93eed8dc8f)

<br>

> 📄 플레이어 피격 시 깜빡임 효과 (핵심 구현 코드)
```cpp
void UCharacterMeshEffect::ApplyHitMaterial(const float Duration)
{
	// 1. Overlay Material을 가져오기
	OriginalOverlayMaterial = TargetMeshComponent->GetOverlayMaterial();
	    
	// 2. Overlay Material을 동적 머티리얼 인스턴스로 변환하여 Opacity 조정
	UMaterialInstanceDynamic* DynOverlayMaterial = UMaterialInstanceDynamic::Create(OriginalOverlayMaterial, this);
	DynOverlayMaterial->SetScalarParameterValue("HitOverlayOpacity", 0.6f);
	TargetMeshComponent->SetOverlayMaterial(DynOverlayMaterial);
	
	// 일정 주기로 깜빡임 효과 타이머 실행
	GetWorld()->GetTimerManager().SetTimer(BlinkTimerHandle, [this, DynOverlayMaterial]()
	{
		BlinkMaterial(DynOverlayMaterial);
	}, Duration / 30.f, true);	       
	
	// 일정 시간 후 머티리얼 원상 복구
	GetWorld()->GetTimerManager().SetTimer(RestoreTimerHandle, [this, DynOverlayMaterial]()
	{
		RestoreOriginalMaterial(DynOverlayMaterial);
		// ... (타이머 정지/멤버 초기화 등 생략)
	}, Duration / 3.f , false);
}
```

>  🔗 전체 소스는 [CharacterMeshEffect.cpp](https://github.com/WJMcode/Project_CavesBasic/blob/main/Source/CavesBasic/Actors/Effect/CharacterMeshEffect/CharacterMeshEffect.cpp)에서 확인하실 수 있습니다.

### 2. Projectile

- **GroundProjectile** : 플레이어 전방의 지형을 자동으로 감지해, **감지한 지형 위에 생성되는 발사체**입니다.  
                         Skill 데이터 테이블에서 `GroundProjectile`로 지정된 Skill을 사용한 경우에 생성되며 `Floor` 충돌 채널이 적용된 지형만 감지합니다.
- **✍️ 핵심 로직** : 지면을 감지하기 위해 위/아래 방향으로 LineTrace를 시도합니다.
  바닥이 감지되면 해당 위치로 이동해 생성되며, 위/아래 모두에서 감지에 실패한 경우 자동으로 제거됩니다.  

이 설계를 통해 지형 위에 생성되는 **장판형 스킬**, **함정 설치** 등의 기능을 손쉽게 구현할 수 있습니다.  
또한, 비정상 위치에 스킬이 사용되지 않도록 필터링하여 **스킬의 신뢰성**과 **사용자 경험 (UX)** 을 크게 향상시킬 수 있습니다.
<br></br>
![groundproject](https://github.com/user-attachments/assets/36e000cf-694d-49c4-94af-ed1080a55919)

	```cpp
	void AGroundProjectile::BeginPlay()
	{
 		const ETraceTypeQuery TraceTypeQuery = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel5);

		// 아래 아래 방향으로 바닥을 감지
		const bool bDownHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(),
			GetActorLocation(), GetActorLocation() + FVector(0, 0, -350), TraceTypeQuery,
			false, IgnoreActors, EDrawDebugTrace::ForDuration, DownHitResult, true);
		if (bDownHit)
		{
		GroundProjectileLocation.Z = DownHitResult.ImpactPoint.Z;
		SetActorLocation(GroundProjectileLocation);
		return;
		}

		// 위 방향으로 바닥을 감지
		const bool bUpHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(),
			GetActorLocation(), GetActorLocation() + FVector(0, 0, 200), TraceTypeQuery,
			false, IgnoreActors, EDrawDebugTrace::ForDuration, UpHitResult, true);
		if (bUpHit)
		{
		GroundProjectileLocation.Z = UpHitResult.ImpactPoint.Z;
		SetActorLocation(GroundProjectileLocation);
		return;
		}
  
		// 둘 다 실패하면 제거
		Destroy();
	}
	```

>  GroundProjectile의 전체 코드는 [GitHub에서 확인하실 수 있습니다.](https://github.com/WJMcode/Project_CavesBasic/blob/main/Source/CavesBasic/Actors/Projectile/GroundProjectile.cpp)

  - Projectile이 **Straight Projectile**로 설정된 Skill 사용 시
<br></br>
**Straight Projectile**은 Player를 중심으로 일직선으로 발사되는 Projectile.<br>
Straight Projectile이 날아가는 동안 Projectile 주변에 몬스터가 있는지 감지.<br>
몬스터가 감지되었다면 해당 몬스터쪽으로 날아갑니다.
<br></br>
![stra](https://github.com/user-attachments/assets/fa9d8cb8-2b37-4320-853e-5277b883c955)

      <details>
        <summary> AStraightProjectile 클래스의 DetectDamageTarget 함수 코드 ( Straight Projectile 생성 시, 범위 내 몬스터를 감지 ) </summary>
    
     

    
       ```cpp
       /* Straight Projectile이 생성되면 AStraightProjectile 클래스의 BeginPlay 함수가 호출됩니다.
        * BeginPlay 함수는 DetectDamageTarget 함수를 호출하여 반환값을 DetectActor에 저장합니다.
        * DetectDamageTarget 함수는 감지된 몬스터를 반환하는 함수입니다.
        * Straight Projectile의 이동 경로 근처에 Collision이 몬스터로 설정된 오브젝트가 있는지 Box Trace를 통해 감지합니다.
        * 감지되었다면 해당 오브젝트를 가리키는 포인터를 반환하고 DetectDamageTarget 함수를 종료합니다.
        */
	
	AActor* AStraightProjectile::DetectDamageTarget()
	{
		FHitResult DetectResult;
		{
		TArray<AActor*> IgnoreActors; IgnoreActors.Add(GetOwner());
	
			FVector TraceStartLocation = GetActorLocation();  // Trace 시작 위치
			FVector TraceDirection = GetActorForwardVector();  // 예: 전방 벡터 (정확한 방향은 상황에 따라 다를 수 있음)
	
			// 새로운 위치 계산
			FVector TraceEndLocation = TraceStartLocation + (TraceDirection * Distance);
	
			// StraightProjectile의 크기를 얻어와서 
			FVector Origin;
			FVector BoxExtent;
			GetActorBounds(false, Origin, BoxExtent);
			
			// Projectile의 자식으로 붙어있는 파티클 시스템의 크기는 빼준다.
			if (ProjectileMeshEffectComponent)
			{
				// 파티클 시스템의 크기 계산
				FBox ParticleBounds = ProjectileMeshEffectComponent->Bounds.GetBox();
				FVector ParticleExtent = ParticleBounds.GetExtent();
	
				// 파티클 시스템 크기를 반영하지 않으려면 BoxExtent에서 빼기
				BoxExtent -= ParticleExtent;
			}
	
			// 감지 범위 조절
			BoxExtent.Y += 50;
			BoxExtent.Z += 150;
			FVector DetectRange = BoxExtent;
	
		     //해당 Trace는 MonsterDetectTraceChannel로 발사되는 Trace이다. 
			// 발사된 해당 Trace는 Collision이 Monster로 설정된 오브젝트를 감지한다.
			const ETraceTypeQuery TraceTypeQuery = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel8);
			const bool bHit = UKismetSystemLibrary::BoxTraceSingle(GetWorld(),
				TraceStartLocation, TraceEndLocation, DetectRange, GetOwner()->GetActorRotation(), TraceTypeQuery,
				false, IgnoreActors, EDrawDebugTrace::ForDuration, DetectResult, true);	
	
			if (bHit)
			{
				AActor* TraceDetectActor = DetectResult.GetActor();
	
				if (TraceDetectActor)
				{
					return TraceDetectActor;
				}
			}
		}
		return nullptr;
	}
	```
	</details>

 
      <details>
        <summary> AStraightProjectile 클래스의 Tick 함수와 FollowDamageTarget 함수 코드 ( Straight Projectile이, 감지한 몬스터 쪽으로 이동 ) </summary>
    
     

    
       ```cpp
       /* Straight Projectile이 존재하는 동안 AStraightProjectile 클래스의 Tick 함수가 호출됩니다.
        * Tick 함수에서는 멤버 포인터인 DetectActor가 가리키는 오브젝트가 존재한다면, FollowDamageTarget 함수를 호출합니다.
        * FollowDamageTarget 함수는 인자로 받은 오브젝트(몬스터)를 Straight Projectile이 따라갈 수 있도록 하는 함수입니다.
        * FollowDamageTarget 함수가 호출될 때마다 따라가야 하는 오브젝트가 어떤 방향에 존재하는지 계속 체크합니다.
        * 그리고 Straight Projectile을 해당 방향으로 회전시킵니다.
        * Straight Projectile은 생성 시 일정한 방향으로 이동하고 속도(Velocity)도 이미 설정되어 있으므로, 
        * FollowDamageTarget 함수에서 방향만 설정해주어도 발사체가 타겟으로 이동하게 됩니다.
        */
	void AStraightProjectile::Tick(float DeltaTime)
	{
		Super::Tick(DeltaTime);
	
		if (DetectActor)
		{
			FollowDamageTarget(DetectActor);
		}
	}
	
	void AStraightProjectile::FollowDamageTarget(AActor* TargetActor)
	{
		// 감지된 Actor를 따라가는 함수
		// 타겟 방향을 계산.
		FVector DirectionToTarget = (TargetActor->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	
		// 발사체를 타겟 방향으로 회전시킴.
		FRotator NewRotation = DirectionToTarget.Rotation();
		SetActorRotation(NewRotation);
	
		ProjectileMovementComponent->Velocity = DirectionToTarget * ProjectileData->InitialSpeed;
	}
	```
	</details><br>

      

                    



### 3. Monster
  - Monster 사망 시, Material을 교체하고 Opacity 값을 수정
                                <br><br>
![monsteropa](https://github.com/user-attachments/assets/3a84ac96-a652-4355-9202-95e3382456a1)

      <details>
        <summary> ADefaultMonster 클래스의 BeginPlay와 OnDisappearMesh, OnDisappearMeshEnd 함수 코드 </summary>
    
     

    
       ```cpp
       /* 현재 Monster의 메시는 Opacity를 수정할 수 없는 메시로 설정되어 있습니다.
        * ADefaultMonster 클래스의 BeginPlay 함수에서
        * 멤버 변수 MaterialInstanceDynamics에 Opacity를 수정할 수 있는 Material을 저장합니다.
        * Monster 사망 시, OnDisappearMesh 함수를 호출하여
        * Monster의 메시를 MaterialInstanceDynamics에 저장된 Material로 교체하고 
        * Opacity를 조정합니다. 이로써 Monster의 메시가 점점 투명해지도록 연출할 수 있게 됩니다.
        * OnDisappearMesh 함수가 종료되면 OnDisappearMeshEnd 함수를 호출하여 Monster를 Destroy합니다.
        */
	void ADefaultMonster::BeginPlay()
	{
		Super::BeginPlay();
		
		SetData(DataTableRowHandle);
	
		USkeletalMeshComponent* SkeletalMeshComponent = GetComponentByClass<USkeletalMeshComponent>();
	
		// 1번 인덱스에 있는 Material이 BlendMode가 Translucent로 설정되어, Opacity를 수정할 수 있는 Material이다.
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
	
		...
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
	```
	</details>

---

### 🖥️ UI
  - Player 사망 시 리스폰창 출력 및 리스폰 진행
                                <br><br>
![deathAndRespawn](https://github.com/user-attachments/assets/8f044cc1-90b6-4b21-a080-380afc884a2f)

---

## 📊 데이터 관리

본 프로젝트는 주요 게임 요소(플레이어, 무기, 스킬, 발사체, 이펙트 등)의 속성과 밸런스를  
**코드가 아닌 DataTable**로 관리하여 빠른 수정과 확장, 유지보수가 가능합니다.

- **Player 데이터 테이블** : 플레이어의 주요 능력치, 외형, 무기 장착 정보 등을 관리합니다.
  
   ![Player 데이터 테이블](README_content/pawndata2.png) 

- **Weapon 데이터 테이블** : 각 무기 외형, 보유 스킬 등 무기 관련 데이터를 관리합니다.

   ![Weapon 데이터 테이블](README_content/WeaponTable.png) 


- **Skill 데이터 테이블** : 스킬의 피해량, 애니메이션(몽타주), 연동 발사체 등 스킬 정보를 설정합니다.
    
   ![Skill 데이터 테이블](README_content/SkillTable.png) 


- **Projectile 데이터 테이블** : 발사체의 타입, 크기, 스폰 위치, 이동 속도 등 상세 속성을 관리합니다.

   ![Projectile 데이터 테이블](README_content/ProjectileTable.png) 


- **Effect 데이터 테이블** : 각종 이펙트 및 효과음, 데칼 정보 등을 관리합니다.

    ![Effect 데이터 테이블](README_content/EffectTable.png) 


> **구조적 특징 :**  
> 각 데이터 테이블은 서로 참조합니다.  
> 예를 들어, 무기는 스킬을 참조하고, 스킬은 발사체를, 발사체는 이펙트를 참조하는 구조로 설계되었습니다.  
> 이를 통해 신규 요소 추가, 밸런스 조정, 효과 변경 등을 코드 수정 없이 에디터에서 즉시 반영할 수 있습니다.

---

## 📝 참고 사항
- 빌드/실행 오류 발생 시 Content 경로 확인 필수
  
---

## 🧩 회고
- **데이터 기반 설계 경험** : 
발사체, 무기, 스킬, 이펙트 등 게임의 주요 요소를 DataTable로 관리하며 하드코딩을 최소화하고, 새로운 데이터 추가와 수정이 매우 편리한 구조를 직접 설계하고 구현해볼 수 있었습니다. 이를 통해 확장성과 유지보수성의 중요성을 체감했습니다.

- **트러블슈팅 역량 강화** : 
발사체 추적 로직, 머티리얼 블렌딩 등에서 다양한 문제를 직접 해결하면서 문제 분석력과 디버깅 실력이 크게 성장했다고 느꼈습니다.

- **개인 프로젝트의 주도적 완성** : 
설계, 구현, 테스트 및 디버깅, 문서화까지 모든 과정을 스스로 주도적으로 진행하며 실무와 유사한 경험을 쌓았고, 자신감도 높아졌습니다.

- **향후 목표** : 
앞으로는 멀티플레이, 더 다양한 콘텐츠 추가 등 한 단계 높은 난이도의 시스템에도 도전하고 싶습니다.

---

### ✉️ Contact

- 이메일 : uoipoip@gmail.com
- GitHub : [WJMcode/Project_CavesBasic](https://github.com/WJMcode/Project_CavesBasic)

---

