// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Monster/AIController/DefaultMonsterAIController.h"
#include "NavigationSystem.h"

ADefaultMonsterAIController::ADefaultMonsterAIController()
{
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));	
}

void ADefaultMonsterAIController::BeginPlay()
{
	Super::BeginPlay();

	BehaviorTree = LoadObject<UBehaviorTree>(nullptr, TEXT("/Script/AIModule.BehaviorTree'/Game/Actors/Monster/AIController/BT_DefaultMonster.BT_DefaultMonster'"));
	ensure(BehaviorTree);

	if(BlackboardComponent && BehaviorTree)
	{
		BlackboardComponent->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
		RunBehaviorTree(BehaviorTree);
	}
	else
	{
		ensure(false);
	}


    FTimerHandle RandomLocationTimerHandle;

    GetWorld()->GetTimerManager().SetTimer(
        RandomLocationTimerHandle,
        this,
        &ADefaultMonsterAIController::FindRandomLocation,
        2.0f,  // x초마다
        true    // 반복적으로 실행
    );

}

void ADefaultMonsterAIController::FindRandomLocation()
{
    // DefaultMonster의 Controller가 존재할 때만 
    // FindRandomLocation 함수 진행.
    // DefaultMonster는 사망하면 Controller를 UnPossess하기 때문에 
    // 체크해주어야 한다.
    if (!GetInstigatorController()) { return; }

    FVector CurrentLocation = GetPawn()->GetActorLocation();
    float SearchRadius = 300.0f;
    FNavLocation RandomLocation;

    UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());

    if (NavSystem)
    {
        // NavMesh에서 임의의 유효한 위치를 찾기
        bool bFound = NavSystem->GetRandomPointInNavigableRadius(CurrentLocation, SearchRadius, RandomLocation);

        if (bFound)
        {
            // Blackboard에 'TargetLocation'을 설정
            UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
            if (BlackboardComp)
            {
                BlackboardComp->SetValueAsVector("TargetLocation", RandomLocation);
                //UE_LOG(LogTemp, Warning, TEXT("Found random location: %s"), *RandomLocation.Location.ToString());
            }

            // Behavior Tree에서 TargetLocation을 사용할 수 있도록 이동 명령 실행
            //MoveToLocation(RandomLocation);  // AI가 이동하도록 지시, Behavior Tree에서 이동하도록 지시하고 있으므로 주석 처리
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to find random location."));
        }
    }
}
