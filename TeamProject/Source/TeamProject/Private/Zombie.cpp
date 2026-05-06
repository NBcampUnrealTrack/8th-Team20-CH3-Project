#include "Zombie.h"
#include "Kismet/GameplayStatics.h"
#include "MonsterAIController.h"
#include "GameFramework/CharacterMovementComponent.h"

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
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!PlayerPawn)
	{
		return;
	}

	float DistanceToPlayer = FVector::Dist(
		GetActorLocation(), PlayerPawn->GetActorLocation()
	);

	if (DistanceToPlayer <= AttackRange)
	{
		Attack(PlayerPawn);
	}
}

void AZombie::Attack(AActor* TargetActor)
{
	if (!TargetActor)
	{
		return;
	}

	if (!bCanAttack)
	{
		return;
	}

	bCanAttack = false;
	bisAttacking = true;

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->DisableMovement();
	}

	AAIController* AICon = Cast<AAIController>(GetController());
	if (AICon)
	{
		AICon->StopMovement();
	}

	GetWorldTimerManager().SetTimer(
		AttackCooldownTimerHandle,
		this,
		&AZombie::ResetAttack,
		AttackCooldown,
		false
	);

	GetWorldTimerManager().SetTimer(
		AttackDurationTimerHandle,
		this,
		&AZombie::EndAttack,
		AttackDuration,
		false
	);
}

void AZombie::EndAttack()
{
	bisAttacking = false;
}

void AZombie::ResetAttack()
{
	bCanAttack = true;

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
}
