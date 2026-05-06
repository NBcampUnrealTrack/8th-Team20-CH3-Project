// MonsterAIController.h

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MonsterAIController.generated.h"

UCLASS()
class TEAMPROJECT_API AMonsterAIController : public AAIController
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override; // 게임이 시작할때 작동하는 함수.
private:
	FTimerHandle ChaseTimerHandle; // 타이머를 관리하기 위한 핸들.
	void ChasePlayer(); // 플레이어를 찾아서 쫒아가는 함수.

	UPROPERTY(EditAnywhere, Category = "AI")
	float ChaseInterval = 0.2f; // 플레이어 위치를 갱신하는 변수 = 1초에 5번 위치를 갱신.
};
