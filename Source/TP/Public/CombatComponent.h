#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

class AThrowProjectile;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TP_API UCombatComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UCombatComponent();

public:
    void BasicAttack();
    void ThrowSkill();

    UPROPERTY(BlueprintReadOnly, Category = "Combat")
    bool bHitEnemy = false;

private:
    UPROPERTY(EditAnywhere, Category = "Basic Attack")
    float BasicAttackDamage = 25.0f;

    UPROPERTY(EditAnywhere, Category = "Basic Attack")
    float BasicAttackRange = 2500.0f;

    UPROPERTY(EditAnywhere, Category = "Skill")
    TSubclassOf<AThrowProjectile> ThrowProjectileClass;

    UPROPERTY(EditAnywhere, Category = "Skill")
    float ThrowCooldown = 5.0f;

    bool bCanUseThrowSkill = true;

    FTimerHandle ThrowCooldownTimerHandle;
    FTimerHandle HitIndicatorTimerHandle;

private:
    void ResetThrowCooldown();
    void ResetHitIndicator();
};
