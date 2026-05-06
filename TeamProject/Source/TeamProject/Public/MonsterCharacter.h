#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MonsterCharacter.generated.h"

UCLASS()
class TEAMPROJECT_API AMonsterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMonsterCharacter();
protected:
	virtual void BeginPlay() override;

public:
	bool IsAttacking = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Movement")
	float WalkSpeed = 300.0f; // 몬스터 스피드.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Stats")
	int32 MaxHealth = 100.0f; // 몬스터 최대 HP.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Monster|Stats")
	int32 CurrentHealth = 100.0f; // 몬스터 현제 HP.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Stats")
	int32 AttackPower = 10.0f; // 몬스터 공격력.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Stats")
	int32 DefensePower = 5.0f; // 몬스터 방어력.

	virtual void Attack(AActor* TagetActor); // 공격 함수.
};
