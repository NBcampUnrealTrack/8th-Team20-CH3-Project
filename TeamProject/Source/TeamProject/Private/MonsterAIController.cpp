// MonsterAIController.cpp

#include "MonsterAIController.h"
#include "Kismet/GameplayStatics.h"
//#include "Zombie.h"

void AMonsterAIController::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(
		ChaseTimerHandle, // 이 타이머를 구분하기 위한 핸들.
		this, // 이 클래스 안의 함수를 실행.
		&AMonsterAIController::ChasePlayer, // 반복 실행할 함수.
		ChaseInterval, // 반복할 시간 = 0.2f.
		true // true 면 반복.
	);
}

void AMonsterAIController::ChasePlayer()
{
	// PlayerPawn 변수에 0번 플레이어 Pawn을 대입.
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	if (!PlayerPawn) // 안전코드, 0번 플레이어를 못찾았으면 함수종료.
	{
		return;
	}

	MoveToActor(PlayerPawn); // 몬스터에게 플레이어 이동하라고 하는 함수. 
}