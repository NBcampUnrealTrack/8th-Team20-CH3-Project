// Zombie.h

# pragma once

# include "CoreMinimal.h"
# include "MonsterCharacter.h"
# include "Zombie.generated.h"

UCLASS()
class TP_API AZombie : public AMonsterCharacter
{
	GENERATED_BODY()

public:
	AZombie(); // 좀비 기본 생성자

	virtual void Attack(AActor* TargetActor) override; // 공격 함수 오버라이드

protected:
	virtual void BeginPlay() override; // 게임 시작 시 실행되는 함수

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|Attack")
	float AttackRange = 100.0f; // 공격할 수 있는 범위

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|Attack")
	float AttackCooldown = 3.3f; // 공격 후 다시 공격하기까지의 시간

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|Attack")
	float AttackCheckInterval = 0.2f; // 플레이어와 거리 체크 주기

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|Attack")
	float AttackDuration = 3.0f; // 공격 상태 유지 시간

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zombie|Attack")
	bool bisAttacking = false; // 현재 공격 중인지 여부

private:
	bool bCanAttack = true; // 공격 가능 여부 체크

	FTimerHandle AttackDurationTimerHandle; // 공격 상태 유지 타이머
	FTimerHandle AttackCheckTimerHandle; // 공격 범위 검사 타이머
	FTimerHandle AttackCooldownTimerHandle; // 공격 쿨타임 타이머

	void CheckAttackRange(); // 플레이어가 공격 범위 안에 있는지 검사
	void EndAttack(); // 공격 상태 종료 함수
	void ResetAttack(); // 공격 가능 상태로 초기화
};