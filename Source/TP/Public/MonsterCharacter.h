#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MonsterCharacter.generated.h"

UCLASS()
class TP_API AMonsterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMonsterCharacter(); // 기본 생성자

protected:
	virtual void BeginPlay() override; // 게임 시작 시 호출
	virtual void Die(); // 몬스터 사망 처리 함수

public:
	bool IsAttacking = false; // 현재 공격 중인지 확인하는 변수

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster|Stats")
	bool IsDead = false; // 몬스터 사망 여부

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Movement")
	float WalkSpeed = 300.0f; // 몬스터 이동 속도

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Stats")
	int32 MaxHealth = 100; // 몬스터 최대 체력

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Monster|Stats")
	int32 CurrentHealth = 100; // 몬스터 현재 체력

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Stats")
	int32 AttackPower = 10; // 몬스터 공격력

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Stats")
	int32 DefensePower = 5; // 몬스터 방어력

public:
	virtual void Attack(AActor* TargetActor); // 공격 함수

	UFUNCTION(BlueprintCallable, Category = "Monster|Combat")
	virtual void ApplyDamageToMonster(int32 DamageAmount); // 데미지를 받는 함수

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Monster|Combat")
	bool Dead() const; // 사망 여부 반환 함수
};