#pragma once

#include "CoreMinimal.h"
#include "MonsterCharacter.h"
#include "Zombie.generated.h"

UCLASS()
class TP_API AZombie : public AMonsterCharacter
{
	GENERATED_BODY()
public:
	AZombie();
	virtual void Attack(AActor* TargetActor) override;
protected:
	virtual void BeginPlay() override;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|Attack")
	float AttackRange = 100.0f; // 공격할 수 있는 범위.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|Attack")
	float AttackCooldown = 3.3f; // 공격 쿨타임.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|Attack")
	float AttackCheckInterval = 0.2f; // 몇 초마다 플레이어 거리 검사.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|Attack")
	float AttackDuration = 3.0f; // 공격 상태 유지.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zombie|Attack")
	bool bisAttacking = false;
private:
	bool bCanAttack = true; // 공격 가능한지 여부
	FTimerHandle AttackDurationTimerHandle;
	FTimerHandle AttackCheckTimerHandle;
	FTimerHandle AttackCooldownTimerHandle;

	void CheckAttackRange();
	void EndAttack();
	void ResetAttack();
};
