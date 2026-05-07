// Zombie.cpp

# include "Zombie.h"
# include "Kismet/GameplayStatics.h"
# include "MonsterAIController.h"
# include "GameFramework/CharacterMovementComponent.h"

AZombie::AZombie()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AZombie::BeginPlay()
{
	Super::BeginPlay();
	GetWorldTimerManager().SetTimer(
		AttackCheckTimerHandle, // 이 타이머를 구분하기 위한 핸들.
		this, // 이 클래스 안의 함수를 실행.
		&AZombie::CheckAttackRange, // 반복 실행할 함수.
		AttackCheckInterval, // 반복할 시간 = 0.2f.
		true // true 면 반복.
	);
}

void AZombie::CheckAttackRange()
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0); // 현재 플레이어 폰 가져오기
	if (!PlayerPawn)
	{
		return; // 플레이어가 없으면 함수 종료
	}

	float DistanceToPlayer = FVector::Dist(
		GetActorLocation(), PlayerPawn->GetActorLocation() // 좀비와 플레이어 사이 거리 계산
	);

	if (DistanceToPlayer <= AttackRange)
	{
		Attack(PlayerPawn); // 공격 범위 안이면 공격 실행
	}
}

void AZombie::Attack(AActor* TargetActor)
{
	if (!TargetActor)
	{
		return; // 공격 대상이 없으면 종료
	}

	if (!bCanAttack)
	{
		return; // 현재 공격 불가능 상태면 종료
	}

	bCanAttack = false; // 공격 쿨타임 시작
	bisAttacking = true; // 공격 중 상태 활성화

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->DisableMovement(); // 공격 중 이동 막기
	}

	AAIController* AICon = Cast<AAIController>(GetController()); // 현재 AI 컨트롤러 가져오기
	if (AICon)
	{
		AICon->StopMovement(); // 이동 중지
	}

	GetWorldTimerManager().SetTimer(
		AttackCooldownTimerHandle,
		this,
		&AZombie::ResetAttack,
		AttackCooldown,
		false
	); // 일정 시간 뒤 공격 가능 상태로 변경

	GetWorldTimerManager().SetTimer(
		AttackDurationTimerHandle,
		this,
		&AZombie::EndAttack,
		AttackDuration,
		false
	); // 공격 애니메이션 종료 타이머
}

void AZombie::EndAttack()
{
	bisAttacking = false; // 공격 상태 종료
}

void AZombie::ResetAttack()
{
	bCanAttack = true; // 다시 공격 가능 상태로 변경

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking); // 이동 다시 활성화
	}
}