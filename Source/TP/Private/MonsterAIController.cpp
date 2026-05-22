// MonsterAIController.cpp

#include "MonsterAIController.h"
#include "Kismet/GameplayStatics.h"
#include "Zombie.h"

void AMonsterAIController::BeginPlay()
{
	Super::BeginPlay();

	CachedZombie = Cast<AZombie>(GetPawn());

	GetWorldTimerManager().SetTimer(
		AITimerHandle, // 타이머를 구분하기 위한 핸들.
		this, // 이 클래스 안의 함수를 실행.
		&AMonsterAIController::UpdateAI, // 반복 실행할 함수.
		AIUpdateInterval, // 반복할 시간 = 0.2f.
		true // true면 반복.
	);
}

void AMonsterAIController::UpdateAI()
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0); // 0번 플레이어 pawn 가져오기.
	APawn* MonsterPawn = GetPawn(); // 이 AIContrioller가 조종 중인 몬스터 pawn 가져오기.

	if (!PlayerPawn || !MonsterPawn) // 0번 플레이어나 몬스터를 못찾으면 함수 종료.
	{
		return;
	}

	float DistanceToPlayer = FVector::Dist(  // 두 사이의 거리 계산.
		MonsterPawn->GetActorLocation(), // 몬스터 위치.
		PlayerPawn->GetActorLocation() // 플레이어 위치.
	); 

	if (!bHasDetectedPlayer) // 플레이어 발견을 못했으면...
	{
		if (CanDetectPlayer(PlayerPawn)) // 플레이어가 탐지 거리 안에 있는지 확인.
		{
			bHasDetectedPlayer = true; // 있으면 플레이어 발견 변수 true!
		}
		else 
		{
			StopChase(); // 발견을 못하면 이동 멈춤.
			return; // 함수 종료.
		}
	}

	if (DistanceToPlayer > CachedZombie->LoseRadius) // 플레이어가 탐지 거리보다 멀어졌다면...
	{
		bHasDetectedPlayer = false; // 플레이어 발견 변수 false.
		StopChase(); // 이동 멈춤.
		return; // 함수 종료.
	}

	ChasePlayer(PlayerPawn); // 플레이어가 탐지 범위에있고 추격 범위 안에있으면 플레이어 추적.
}

bool AMonsterAIController::CanDetectPlayer(APawn* PlayerPawn) const
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn || !PlayerPawn)
	{
		return false;
	}

	AZombie* Zombie = Cast<AZombie>(ControlledPawn);
	if (!Zombie)
	{
		return false;
	}

	float Distance = FVector::Dist(
		ControlledPawn->GetActorLocation(),
		PlayerPawn->GetActorLocation()
	);

	return Distance <= Zombie->DetectRadius;
}

void AMonsterAIController::ChasePlayer(APawn* PlayerPawn) 
{
	if (!PlayerPawn) // 플레이어가 추적 범위에 없으면...
	{
		return; // 함수 종료.
	}

	MoveToActor(PlayerPawn); // 추적 범위에 있으면 플레이어를 향해 이동.
}

void AMonsterAIController::StopChase()
{
	StopMovement(); // 현제 이동을 멈춤.
}

