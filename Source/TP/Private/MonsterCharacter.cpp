#include "MonsterCharacter.h"
#include "MonsterAIController.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../TPGameMode.h"

AMonsterCharacter::AMonsterCharacter()
{
	PrimaryActorTick.bCanEverTick = false; // Tick 함수 비활성화.

	AIControllerClass = AMonsterAIController::StaticClass(); // 이 몬스터를 조종할 AI 컨트롤러 클래스 지정.
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned; // 몬스터가 레벨에 배치되거나 스폰되었을 때 자동으로 AI 컨트롤러가 빙의되도록 설정.
}

void AMonsterCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed; // 몬스터 이동 속도 설정.

	CurrentHealth = MaxHealth; // 게임 시작 시 현재 체력을 최대 체력으로 초기화.
}

void AMonsterCharacter::Attack(AActor* TagetActor) // 공격 함수 구현은 자식 클래스에서 진행.
{

}

void AMonsterCharacter::ApplyDamageToMonster(int32 DamageAmount)
{
	if (IsDead)
	{
		return;
	}

	int32 FinalDamage = FMath::Max(0, DamageAmount - DefensePower);

	CurrentHealth -= FinalDamage;

	UE_LOG(LogTemp, Warning, TEXT("최종 데미지: %d / 체력: %d"), FinalDamage, CurrentHealth);

	if (CurrentHealth <= 0)
	{
		Die(); 
	}
}

float AMonsterCharacter::TakeDamage(
	float DamageAmount,
	FDamageEvent const& DamageEvent,
	AController* EventInstigator,
	AActor* DamageCauser
)
{
	if (IsDead)
	{
		return 0.0f;
	}

	int32 IntDamage = FMath::RoundToInt(DamageAmount);

	UE_LOG(LogTemp, Warning, TEXT("TakeDamage 들어옴: %d"), IntDamage);

	ApplyDamageToMonster(IntDamage);
	PlayHitAnimation();

	return DamageAmount;
}

bool AMonsterCharacter::Dead() const
{
	return IsDead; // 사망 여부 반환.
}

void AMonsterCharacter::Die()
{
	if (IsDead) // 이미 죽었다면 중복 실행 방지.
	{
		return;
	}

	IsDead = true; // 사망 상태 설정.
	IsAttacking = false; // 공격 상태 종료.

	ATPGameMode* GameMode =
		Cast<ATPGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	if (GameMode)
	{
		GameMode->Score += 100;
		GameMode->KillCount += 1;

		UE_LOG(LogTemp, Warning,
			TEXT("Monster Dead! Score: %d KillCount: %d"),
			GameMode->Score,
			GameMode->KillCount);
	}

	AAIController* AICon = Cast<AAIController>(GetController());

	if (AICon)
	{
		AICon->StopMovement(); // AI 이동 중지.
	}

	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 캡슐 충돌 비활성화.
	}

	if (GetMesh())
	{
		GetMesh()->SetCollisionProfileName(TEXT("Ragdoll")); // 래그돌 충돌 프로필 적용.
		GetMesh()->SetSimulatePhysics(true); // 물리 시뮬레이션 활성화.
		GetMesh()->SetEnableGravity(true); // 중력 활성화.
		GetMesh()->WakeAllRigidBodies(); // 모든 물리 바디 깨우기.
	}

	SetLifeSpan(3.0f); // 5초 후 액터 제거.
}

void AMonsterCharacter::PlayHitAnimation()
{
	GetMesh()->GetAnimInstance()->Montage_Play(HitMontage);
}