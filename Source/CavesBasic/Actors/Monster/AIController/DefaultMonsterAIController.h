// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "DefaultMonsterAIController.generated.h"

/**
 * 
 */
UCLASS()
class CAVESBASIC_API ADefaultMonsterAIController : public AAIController
{
	GENERATED_BODY()
	
public:
    ADefaultMonsterAIController();
    UBehaviorTree* GetBehaviorTree() { return BehaviorTree; };

protected:
    virtual void BeginPlay() override;

    virtual void FindRandomLocation();

protected:
    // Behavior Tree를 시작하는 함수
    UPROPERTY(EditDefaultsOnly, Category = "AI")
    UBehaviorTree* BehaviorTree;

    UPROPERTY(EditDefaultsOnly, Category = "AI")
    UBlackboardComponent* BlackboardComponent;
};
