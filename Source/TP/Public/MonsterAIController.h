// MonsterAIController.h

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MonsterAIController.generated.h"

UCLASS()
class TP_API AMonsterAIController : public AAIController
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override; // 게임이 시작할때 작동하는 함수.

private:
	FTimerHandle AITimerHandle; // ai 판단을 반복 실행하기 위한 타이머 핸들.

	void UpdateAI(); // 탐지와 추적을 판단하는 함수.
	bool CanDetectPlayer(APawn* PlayerPawn) const; // 플레이어가 탐지 거리 안에 있는지 확인하는 함수.
	void ChasePlayer(APawn* PlayerPawn); // 플레이어를 추적하는 함수.
	void StopChase(); // 추적을 멈추는 함수.

	bool bHasDetectedPlayer = false; // 플레이어를 발견했는지 저장하는 변수.

	float AIUpdateInterval = 0.2f; // ai 판단을 몇 초마다 갱신할지 정하는 변수.
	float DetectRadius = 800.0f; // 몬스터 인식 사거리.
	float LoseRadius = 1200.0f; // 몬스터 인식 해제 사거리.
};
