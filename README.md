# CavesBasic ( 개발 : 원재민 )
## 프로젝트 소개

![alt text](README_content/main2.png "Title Text")
![alt text](README_content/main.png "Title Text")

- 개발 목록
  * Player
    - 키 입력을 받아 앞, 뒤, 아래, 위와 점프, 웅크리기 가능
     + 움직임에 맞는 애니메이션 적용
     + Weapon 습득 후 Skill 사용 가능
     + 피격 시 무적 시간 동안 캐릭터 깜빡임 적용
     + 사망 처리 적용
     + 부활 기능 구현
     <br></br>
     
   * Monster
        - 앞, 뒤, 아래, 위 움직임 수행
        - 레벨에 NavMeshBoundsVolume을 배치하여 Monster AI 움직임 적용
        - 사망 시 점점 투명해지며 사라지도록 구현
        <br></br>
	
   * UI
        - 플레이어 체력바 구현
        - 몬스터 체력바 구현
        - 플레이어 사망 시 리스폰창 출력
        <br></br><br>

## 세부 사항
- ### ***Player***
  - ***Player의 데이터***는 언리얼 엔진 에디터상에서 데이터 테이블로 세팅 가능
          <br><br>
  ![alt text](README_content/pawndata.png "Title Text")<br>  　　　　　　  　　　　　　    　　　　　　  　　　　　　  **`Player 데이터 테이블`**<br><br><br>
	  
  * ***Weapon의 데이터***는 언리얼 엔진 에디터상에서 데이터 테이블로 세팅 가능
    <br><br>
***Weapon 데이터 테이블***에서는 해당 Weapon이 보유할 Skill을 세팅해 줄 수 있음.<br><br>
![alt text](README_content/WeaponTable.png "Title Text")<br>  　　　　　　  　　　　　　  **`Weapon 데이터 테이블`**<br><br><br>
***Skill 데이터 테이블***에서는 해당 Skill 사용 시 발사되는 Projectile을 세팅해 줄 수 있음.<br><br> ![alt text](README_content/SkillTable.png "Title Text")<br>  　　　　　　  　　　　　　  **`Skill 데이터 테이블`**<br><br><br>
***Projectile 데이터 테이블***에서는 해당 Projectile의 크기와 스폰 위치를 세팅해 줄 수 있음.<br><br> ![alt text](README_content/ProjectileTable.png "Title Text")<br>  　　　　　　  　　　　　　  **`Projectile 데이터 테이블`**<br><br><br>
***Effect 데이터 테이블***에서는 Projectile의 효과음과 Effect를 세팅해 줄 수 있음.<br><br> ![alt text](README_content/EffectTable.png "Title Text")<br>  　　　　　　  　　　　　　    　　　**`Effect 데이터 테이블`**<br><br><br>


  - Weapon을 습득한 Player는 Skill 사용 가능
    
      <details>
        <summary> OnSkill 함수 코드 ( Skill 실행 코드 )</summary>
    
     

    
       ```cpp
       /* Player가 키 입력을 통해 Skill을 사용하면 OnSkill 함수가 호출됩니다.
        * 기본적으로 Player가 웅크리지 않은 상태에서만 OnSkill 함수가 실행됩니다.
        * Weapon은 에디터 내에 존재하는 Weapon 전용 InputMappingContext를 통해
        * Skill InputAction들을 바인딩하고 있습니다.
        * OnSkill 함수가 호출되는 시점에 어떤 Skill InputAction이 들어왔는지 체크합니다.
        * ( 이때 InputAction 파일의 이름 규칙은 "IA_Skill*"이며 *은 1부터 시작하는 Skill 번호입니다. )
        * 체크한 Skill InputAction의 번호를 통해 Weapon 데이터 테이블이 저장하고 있는
        * Skill 데이터 테이블의 행에 접근합니다.
        * Skill 데이터 테이블에는 *번 스킬이 실행될 때 재생되는 몽타주가 저장되어 있어 해당 몽타주를 재생합니다.
        */
	void AWeaponBase::OnSkill(const FInputActionInstance& Instance)
	{
	    ACharacter* OwningCharacter = Cast<ACharacter>(OwningPawn);
       	    // 캐릭터가 웅크리지 않은 상태라면
	    if (!OwningCharacter->bIsCrouched)
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
	
		    // 현재 몽타주가 재생 중이지 않을 때
		    if (nullptr == CurrentMontage)
		    {
			if (ABasicPlayer* WeaponOwner = Cast<ABasicPlayer>(OwningCharacter))
			{
			    UStatusComponent* StatusComponent = WeaponOwner->GetComponentByClass<UStatusComponent>();
			    if (StatusComponent->IsPlayer())
			    {
				UAnimMontage* PlayingMontage = WeaponOwner->GetPlayingMontage();
				if (PlayingMontage)
				{
				    WeaponOwner->SetPlayingMontage(nullptr);
				}
	
				// 스킬 데이터 테이블에 있는 몽타주를 재생
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
		    }
		}
	    }
	}
       ```
      </details><br>


  - Projectile이 ***Ground Projectile***로 설정된 Skill 사용 시
<br></br>
***Ground Projectile***은 Player 앞에 땅이 있어야 생성되는 발사체임.<br>
Player를 중심( 캐릭터의 배꼽 위치 )을 기준으로 바닥이 존재한다면 바닥에 생성되고,<br>
바닥이 없다면 위에 있는 땅을 감지하여 생성됨. 아무것도 감지되지 않으면 생성되지 않음.
<br></br>
![alt text](README_content/groundproject.gif "Title Text")
      <details>
        <summary> AGroundProjectile 클래스의 BeginPlay 함수 코드 ( GroundProjectile의 생성 위치를 조정 )</summary>
    
     

    
       ```cpp
       /* Skill 데이터 테이블에서 Projectile 설정이 GroundProjectile로 설정된 Skill을 사용하면 GroundProjectile 객체가 생성됩니다.
        * GroundProjectile은 Player의 중심을 기준으로, Skill 데이터 테이블에서 설정한 Transform값을 포함한 위치에 생성됩니다.
        * GroundProjectile이 생성되면 GroundProjectile 위치 기준, 아래 방향으로 LineTrace를 발사하여 오브젝트를 감지합니다.
        * Collision이 Floor로 설정된 오브젝트가 감지되었다면 감지된 오브젝트 위로 GroundProjectile을 옮깁니다.
        * 만약 Collision이 Floor로 설정된 오브젝트가 아닌, 다른 오브젝트가 감지되거나 아무것도 감지되지 않으면
        * 위쪽 방향으로 LineTrace를 발사하여 오브젝트를 감지합니다.
        * 마찬가지로 Collision이 Floor로 설정된 오브젝트가 감지되었다면 감지된 오브젝트 위로 GroundProjectile을 옮깁니다.
        * 이 경우에도 아무것도 감지되지 않으면 GroundProjectile을 Destroy합니다.                     
        */
	void AGroundProjectile::BeginPlay()
	{
		Super::BeginPlay();
	
		// GroundProjectile의 위치 얻어오기
		FVector GroundProjectileLocation = GetActorLocation();
		// 아래 방향으로 LineTrace를 발사
		FHitResult DownHitResult;
		{
			TArray<AActor*> IgnoreActors; IgnoreActors.Add(GetOwner());
	
			// 해당 Trace는 FloorDetectTraceChannel로 발사되는 Trace이다. 
			// 발사된 해당 Trace는 Collision이 Floor로 설정된 오브젝트를 감지한다.
			// Floor로 설정된 오브젝트에만 GroundProjectile 스킬을 스폰시키는 것이 목적.
			const ETraceTypeQuery TraceTypeQuery = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel5);
			const bool bHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(),
				GetActorLocation(), GetActorLocation() + FVector(0, 0, -350), TraceTypeQuery,
				false, IgnoreActors, EDrawDebugTrace::ForDuration, DownHitResult, true);
			// 만약 Hit가 발생했다면 그 위치로 GroundProjectile을 옮김
			if (bHit)
			{
				GroundProjectileLocation.Z = DownHitResult.ImpactPoint.Z;
				SetActorLocation(GroundProjectileLocation);
	
				return;
			}
		}
		// 위쪽 방향으로 LineTrace를 발사
		FHitResult UpHitResult;
		{
			TArray<AActor*> IgnoreActors; IgnoreActors.Add(GetOwner());
	
			const ETraceTypeQuery TraceTypeQuery = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel5);
			const bool bHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(),
				GetActorLocation(), GetActorLocation() + FVector(0, 0, 200), TraceTypeQuery,
				false, IgnoreActors, EDrawDebugTrace::ForDuration, UpHitResult, true);
	
			if (bHit)
			{
				GroundProjectileLocation.Z = UpHitResult.ImpactPoint.Z;
				SetActorLocation(GroundProjectileLocation);
				
				return;
			}
		}
		
		// Floor가 감지되지 않으면 GroundProjectile을 그냥 제거한다.
		Destroy();
	}
 	```
      </details>

 
      <details>
        <summary> AGroundProjectile 클래스의 OnBeginOverlap 함수 코드 ( GroundProjectile이 다른 오브젝트와 접촉할 시 호출 )</summary>
    
     

    
       ```cpp
       /* GroundProjectile과 Collision이 Floor로 설정된 오브젝트가 접촉하면 Decal Effect를 생성합니다.
        * 접촉했다면 DetectDamageTarget 함수를 호출합니다. BoxTrace를 발사하여 데미지를 적용할 수 있는 오브젝트가 있는지 체크하는 함수입니다.
        * 이후 GroundProjectile을 파괴합니다.
        * 데미지를 적용할 수 있는 오브젝트가 존재한다면, UGameplayStatics의 ApplyDamage 함수를 호출하여 데미지를 적용합니다.
        */
	void AGroundProjectile::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
	{
		if (!IsValid(this)) { return; }
		
		FTransform NewTransform = GetActorTransform();
		
		// DecalEffect 출력
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
	  	// 스킬을 관리하는 배열에 접근하여 스킬 시전 애니메이션과
	  	// 플레이어가 재생 중인 스킬 시전 애니메이션과 같은 것을 찾아냄.
		// Notify를 발생시킨 애니메이션과 일치하는 애니메이션이 있는 스킬 배열의 인덱스를 저장하고 그 스킬 배열에 있는 데이터 테이블에 접근하여 반환
		const FSkillTableRow* SkillTableRow = OwningCharacter->GetSkillTableRow();
		ensure(SkillTableRow);
	
		// GroundProjectile이 Floor와 Overlap되면 
		// BoxTrace를 이용해 데미지를 줄 타겟을 감지한다.
		AActor* DetectActor = DetectDamageTarget();
	
		Destroy();
	
		// 타겟이 감지되었다면 데미지 주기
		if(DetectActor)
		{
			UGameplayStatics::ApplyDamage(DetectActor, SkillTableRow->SkillDamage, GetInstigator()->GetController(), this, nullptr);
		}
	}
	```
	</details><br><br>
      
  - Projectile이 ***Straight Projectile***로 설정된 Skill 사용 시
<br></br>
***Straight Projectile***은 Player를 중심으로 일직선으로 발사되는 Projectile임.<br>
Straight Projectile이 날아가는 동안 Projectile 주변에 몬스터가 있는지 감지함.<br>
몬스터가 감지되었다면 해당 몬스터쪽으로 날아감.
<br></br>
![alt text](README_content/stra.gif "Title Text")
      <details>
        <summary> AStraight Projectile 클래스의 BeginPlay 함수와 DetectDamageTarget 함수 코드 ( Straight Projectile 생성 시, 범위 내 몬스터를 감지 ) </summary>
    
     

    
       ```cpp
       /* Straight Projectile이 생성되면 AStraightProjectile 클래스의 BeginPlay 함수가 호출됩니다.
        * BeginPlay 함수는 DetectDamageTarget 함수를 호출하여 반환값을 DetectActor에 저장합니다.
        * DetectDamageTarget 함수는 감지된 몬스터를 반환하는 함수입니다.
        * Straight Projectile의 이동 경로 근처에 Collision이 몬스터로 설정된 오브젝트가 있는지 Box Trace를 통해 감지합니다.
        * 감지되었다면 해당 오브젝트를 가리키는 포인터를 반환하고 DetectDamageTarget 함수를 종료합니다.
        */
	void AStraightProjectile::BeginPlay()
	{
		Super::BeginPlay();
	
		DetectActor = DetectDamageTarget();
	}
	
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
        <summary> AStraight Projectile 클래스의 Tick 함수와 FollowDamageTarget 함수 코드 ( Straight Projectile이, 감지한 몬스터 쪽으로 이동 ) </summary>
    
     

    
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
	</details><br><br>

      
  - Player 피격 시, Overlay Material의 Opacity 값을 조정하여 깜빡이는 효과 부여 
<br></br>
![alt text](README_content/blinkCha.gif "Title Text")
      <details>
        <summary> UCharacterMeshEffect 클래스의 ApplyHitMaterial 함수 코드 ( Player의 메시에 효과를 주기 위한 클래스 ) </summary>
    
     

    
       ```cpp
       /* Player의 메시는 오버레이 머티리얼을 가지고 있습니다.
        * Player가 피격될 경우 오버레이 머티리얼의 HitOverlayOpacity라는 Parameter의 값을 변경합니다.
        * 기본적으로 HitOverlayOpacity의 값은 0이며, 피격될 경우 0.6으로 값이 설정되어 Player의 메시가 보라색이 됩니다.
        * 이후 HitOverlayOpacity의 값을 다시 0으로, 그리고 0.6으로 설정하는 것을 반복하여 보라색으로 깜빡이는 효과를 줍니다.
        * Player가 사망하거나, 일정 시간이 지나면 HitOverlayOpacity의 값을 0으로 설정함으로써 깜빡임을 멈춥니다.
        */
	void UCharacterMeshEffect::ApplyHitMaterial(const float Duration)
	{
	    if (!OwningPlayer || !TargetMeshComponent)
	    {
	        UE_LOG(LogTemp, Warning, TEXT("OwningPlayer or MeshComponent is nullptr."));
	        return;
	    }
	
	    // 1. Overlay Material을 가져오기
	    OriginalOverlayMaterial = TargetMeshComponent->GetOverlayMaterial();
	
	    if (!OriginalOverlayMaterial)
	    {
	        UE_LOG(LogTemp, Warning, TEXT("OriginalOverlayMaterial is nullptr, 플레이어 Mesh에는 오버레이 머티리얼이 존재해야 합니다."));
	        return;
	    }
	    
	    // 2. Overlay Material을 동적 머티리얼 인스턴스로 변환
	    UMaterialInstanceDynamic* DynOverlayMaterial = UMaterialInstanceDynamic::Create(OriginalOverlayMaterial, this);
	
	    if (DynOverlayMaterial)
	    {
	        DynOverlayMaterial->SetScalarParameterValue("HitOverlayOpacity", 0.6f);
	        TargetMeshComponent->SetOverlayMaterial(DynOverlayMaterial);
	
	        // BlinkTimerHandle가 작동하고 있지 않을 때에만 아래 코드 실행
	        if (!GetWorld()->GetTimerManager().IsTimerActive(BlinkTimerHandle))
	        {
	            // (Duration / x.f)초마다 BlinkMaterial 함수를 호출함
	            GetWorld()->GetTimerManager().SetTimer(BlinkTimerHandle, [this, DynOverlayMaterial]()
	                {
	                    BlinkMaterial(DynOverlayMaterial);
	                }, Duration / 30.f, true);
	        }
	
	        // RestoreTimerHandle가 작동하고 있지 않을 때에만 아래 코드 실행
	        if (!GetWorld()->GetTimerManager().IsTimerActive(RestoreTimerHandle))
	        {
	            // OwningPlayer가 사망 상태라면 덜 깜빡임
	            if (OwningPlayer && OwningPlayer->GetStatusComponent()->IsDie())
	            {
	                // Duration / x초 후에 타이머를 멈추도록 설정
	                GetWorld()->GetTimerManager().SetTimer(RestoreTimerHandle, [this, DynOverlayMaterial]()
	                    {
	                        RestoreOriginalMaterial(DynOverlayMaterial);
	
	                        // 타이머 정지
	                        GetWorld()->GetTimerManager().ClearTimer(BlinkTimerHandle);
	                        GetWorld()->GetTimerManager().ClearTimer(RestoreTimerHandle);
	
	                        InitializeMembers();
	
	                    }, Duration / 3.f , false);
	            }
	            else
	            {
	                // Duration초 후에 타이머를 멈추도록 설정
	                GetWorld()->GetTimerManager().SetTimer(RestoreTimerHandle, [this, DynOverlayMaterial]()
	                    {
	                        RestoreOriginalMaterial(DynOverlayMaterial);
	
	                        // 타이머 정지
	                        GetWorld()->GetTimerManager().ClearTimer(BlinkTimerHandle);
	                        GetWorld()->GetTimerManager().ClearTimer(RestoreTimerHandle);
	
	                        InitializeMembers();
	
	                    }, Duration, false);
	            }
	        }
	    }
	}
	
	void UCharacterMeshEffect::BlinkMaterial(UMaterialInstanceDynamic* OutDynOverlayMaterial)
	{
	    if (BlinkCount % 2 == 0)
	    {
	        // HitOverlayOpacity값을 0.0f로 설정, 기본 상태랑 같음
	        OutDynOverlayMaterial->SetScalarParameterValue("HitOverlayOpacity", 0.0f);
	    }
	    else
	    {
	        // HitOverlayOpacity값을 0.6f로 설정, OverlayMaterial이 캐릭터에게 반투명 형태로 나타남
	        OutDynOverlayMaterial->SetScalarParameterValue("HitOverlayOpacity", 0.6f);
	    }
	
	    // 파라미터 변경 후 Overlay Material 재설정
	    TargetMeshComponent->SetOverlayMaterial(OutDynOverlayMaterial);
	
	    // BlinkCount 증가
	    ++BlinkCount;
	}
	
	void UCharacterMeshEffect::RestoreOriginalMaterial(UMaterialInstanceDynamic* OutDynOverlayMaterial)
	{
	    TargetMeshComponent->SetOverlayMaterial(OriginalOverlayMaterial);
	}
	
	void UCharacterMeshEffect::InitializeMembers()
	{
	
	    OriginalOverlayMaterial = nullptr;
	
	    BlinkCount = 0;
	    BlinkTimerHandle.Invalidate();
	    RestoreTimerHandle.Invalidate(); 
	}
	```
	</details><br>
                    <br>
* ### ***Monster***
  - Monster 사망 시, Material을 교체하고 Opacity 값을 수정
                                <br><br>
![monsteropa](README_content/monsteropa.gif)
      <details>
        <summary> ADefaultMonster 클래스의 BeginPlay 함수 코드 </summary>
    
     

    
       ```cpp
       /* 현재 Monster의 Opcity를 수정할 수 없는 메시로 설정되어 있습니다.
        * ADefaultMonster 클래스의 BeginPlay 함수에서 멤버 변수 MaterialInstanceDynamics에 Opcity를 수정할 수 있는 Material을 저장하고,
        * Monster 사망 시, OnDisappearMesh 함수를 호출하여 Monster의 메시를 MaterialInstanceDynamics에 저장된 Material로 교체하고 
        * Opcity를 조정합니다. 이로써 Monster의 메시가 점점 투명해지도록 연출할 수 있게 됩니다.
        * OnDisappearMesh 함수가 종료되면 OnDisappearMeshEnd 함수를 호출하여 Monster를 Destroy합니다.
        */
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
                              <br></br>
                              <br></br>
                              <br></br>
                              
* UI
  - 플레이어 사망 시 리스폰창 출력
                                <br></br>
                              <br></br>
![사진이름](resp.gif)
                                <br></br>
                              <br></br>

       <details>
    <summary> 코드 </summary>                      
 ```cpp
void ABasicHUD::BeginPlay()
{
	Super::BeginPlay();
	
  // 플레이어가 사망했을 때 Broadcast하는
  // OnDie 델리게이트를 구독하는 함수
	AddRespawnHUDDelegate();

	// 블루프린트에서 UBasicHUDWidget 클래스로 만든 위젯 안에는 
	// UHPBarWidgetBase와 URespawnPanelWidget가 있음
	
	// C++에서 UBasicHUDWidget 클래스 안에는
	// UHPBarWidgetBase 와 URespawnPanelWidget 객체를 가리키는 포인터를 멤버로 들고 있다.
	// 해당 멤버들은 블루프린트에 있는 위젯을 바인딩하고 있다.


	UClass* WidgetClass = LoadClass<UBasicHUDWidget>(nullptr,
		TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/UI_CavesBasic.UI_CavesBasic_C'"));
	
	if (!WidgetClass)
	{
		ensure(WidgetClass);
		UE_LOG(LogTemp, Error, TEXT("Failed to load WidgetClass!"));
		return;
	}
	
	Widget = CreateWidget<UBasicHUDWidget>(GetWorld(), WidgetClass);

	// UBasicHUDWidget가 멤버로 들고 있는 위젯들의 초기화 작업 진행
	Widget->InitializeWidgets(GetOwningPawn());
	
	...

	Widget->AddToViewport();

}

void ABasicHUD::OnPlayerDeath()
{
	Widget->HandlePlayerDeath();
}

```
</details>
                                <br></br>
                              <br></br>
          <details>
    <summary> 코드 </summary>

```cpp
void UBasicHUDWidget::HandlePlayerDeath()
{
    UI_RespawnPanel->ShowRespawnPanel(true);
}
```

</details>
             <br></br>
             <br></br>
                <details>
    <summary> 코드 </summary>

```cpp
void URespawnPanelWidget::ShowRespawnPanel(bool bShow)
{
	SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}
```

</details>
                              <br></br>
                                                            <br></br>

* UI
  - 플레이어 리스폰 진행
                              <br></br>
                                                            <br></br>
    <details>
    <summary> 코드 </summary>

```cpp
void AStageGameMode::RespawnPlayer(APlayerController* PlayerController)
{
    if (!PlayerController) return;

    // 사망한 플레이어 Actor 제거
    APawn* OldCharacter = PlayerController->GetPawn();
    OldCharacter->Destroy();

    // PlayerStart를 찾아 리스폰 위치 결정
    AActor* PlayerStart = FindPlayerStart(PlayerController);
    FVector SpawnLocation = PlayerStart->GetActorLocation();
    FRotator SpawnRotation = PlayerStart->GetActorRotation();

    // 새로운 플레이어 캐릭터 생성
    ABasicPlayer* NewCharacter = GetWorld()->SpawnActor<ABasicPlayer>(DefaultPawnClass, SpawnLocation, SpawnRotation);
    if (NewCharacter)
    {
        PlayerController->Possess(NewCharacter);  // 새로운 캐릭터를 빙의

        // 캐릭터 상태 초기화, 체력을 1로 세팅한 후 리스폰
        NewCharacter->ResetCharacterStatus();

        // HUD 갱신
        ABasicHUD* BasicHUD = Cast<ABasicHUD>(PlayerController->GetHUD());
        if (BasicHUD)
        {
            APawn* OwningPawn = BasicHUD->GetOwningPawn();

            if(OwningPawn)
            {
                if (!BasicHUD) { ensure(false); }
                
                // 델리게이트 구독 해제
                BasicHUD->RemoveHUDDelegate();  // 기존에 구독한 델리게이트를 해제하는 함수
                // 델리게이트 다시 구독
                BasicHUD->AddRespawnHUDDelegate();  // 델리게이트 재구독

                BasicHUD->ResetUI(OwningPawn);  // UI 갱신 
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("AStageGameMode::RespawnPlayer(APlayerController* PlayerController) : OwningPawn is nullptr"))
            }
        }
    }
}
```

</details>
                              <br></br>
                                                            <br></br>
    <details>
    <summary> ABasicHUD::ResetUI </summary>

```cpp
void ABasicHUD::ResetUI(APawn* InOwningPawn)
{
  // Widget은 체력바와 리스폰창을 멤버로 들고 있음
	Widget->InitializeWidgets(InOwningPawn);
	Widget->RefreshWidgetsForRespawn();
}
```

</details>
                              <br></br>
                                                            <br></br>
    <details>
    <summary> UBasicHUDWidget::RefreshWidgetsForRespawn </summary>

```cpp
void UBasicHUDWidget::RefreshWidgetsForRespawn()
{
    // PlayerHPBar 관련
    {
        if (UI_PlayerHPBar)
        {
            // 델리게이트 구독 해제
            UI_PlayerHPBar->RemoveHPBarDelegate();  // 이미 구독한 델리게이트를 해제하는 함수
            // 델리게이트 다시 구독
            UI_PlayerHPBar->AddHPBarDelegate();  // 델리게이트 재구독

            UI_PlayerHPBar->RefreshHPBar();
        }
    }
}
```

</details>
                              <br></br>
                                                            <br></br>
    <details>
    <summary> UHPBarWidgetBase::AddHPBarDelegate, RefreshHPBar </summary>

```cpp
void UHPBarWidgetBase::AddHPBarDelegate()
{
	if (OwningPawn)
	{
		UStatusComponent* StatusComponent = OwningPawn->GetComponentByClass<UStatusComponent>();
		check(StatusComponent);
		StatusComponent->OnHPChanged.AddDynamic(this, &ThisClass::OnHPChanged);
		StatusComponent->OnDie.AddDynamic(this, &ThisClass::OnDie);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UHPBarWidgetBase::AddHPBarDelegate() : OwningPawn is nullptr"));
	}
}

void UHPBarWidgetBase::RefreshHPBar()
{
	if (OwningPawn)
	{
		UStatusComponent* StatusComponent = OwningPawn->GetComponentByClass<UStatusComponent>();
		check(StatusComponent);

		// OwningPawn의 현재 체력 상태를 얻어와서 
		// 계산 후에 화면에 띄워준다.
		float PawnMaxHP = StatusComponent->GetMaxHP();
		float PawnCurrentHP = StatusComponent->GetHP();

		const float Percent = PawnCurrentHP / PawnMaxHP;
		HPBar->SetPercent(Percent);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UHPBarWidgetBase::RefreshHPBar() : Pawn is nullptr"));
	}
}

```
</details>
