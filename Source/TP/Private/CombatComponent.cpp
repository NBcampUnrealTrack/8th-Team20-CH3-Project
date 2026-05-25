// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "ThrowProjectile.h"
#include "MonsterCharacter.h"

#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "TimerManager.h"

UCombatComponent::UCombatComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UCombatComponent::BasicAttack()
{
    AActor* Owner = GetOwner();

    if (Owner == nullptr)
    {
        return;
    }

    FVector Start;
    FRotator Rotation;

    Owner->GetActorEyesViewPoint(Start, Rotation);

    FVector End = Start + Rotation.Vector() * BasicAttackRange;

    FHitResult HitResult;

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(Owner);

    bool bHit = GetWorld()->LineTraceSingleByChannel(
        HitResult,
        Start,
        End,
        ECC_Visibility,
        Params
    );

    /*DrawDebugLine(
        GetWorld(),
        Start,
        End,
        FColor::Red,
        false,
        1.0f
    ); */

    if (bHit)
    {
        AActor* HitActor = HitResult.GetActor();

        if (HitActor)
        {
            AMonsterCharacter* Monster = Cast<AMonsterCharacter>(HitActor);

            if (Monster)
            {
                UGameplayStatics::ApplyDamage(
                    Monster,
                    BasicAttackDamage,
                    Owner->GetInstigatorController(),
                    Owner,
                    UDamageType::StaticClass()
                );

                bHitEnemy = true;
                GetWorld()->GetTimerManager().SetTimer(
                    HitIndicatorTimerHandle,
                    this,
                    &UCombatComponent::ResetHitIndicator,
                    0.15f,
                    false
                );

                UE_LOG(LogTemp, Warning, TEXT("좀비 피격: %s"), *Monster->GetName());
            }
        }
    }
}

void UCombatComponent::ThrowSkill()
{
    if (bCanUseThrowSkill == false)
    {
        UE_LOG(LogTemp, Warning, TEXT("Throw Skill Cooldown"));
        return;
    }

    AActor* Owner = GetOwner();

    if (Owner == nullptr)
    {
        return;
    }

    if (ThrowProjectileClass == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("ThrowProjectileClass is not set"));
        return;
    }

    FVector SpawnLocation;
    FRotator SpawnRotation;

    Owner->GetActorEyesViewPoint(SpawnLocation, SpawnRotation);

    SpawnLocation = SpawnLocation + SpawnRotation.Vector() * 100.0f;

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = Owner;
    SpawnParams.Instigator = Cast<APawn>(Owner);

    GetWorld()->SpawnActor<AThrowProjectile>(
        ThrowProjectileClass,
        SpawnLocation,
        SpawnRotation,
        SpawnParams
    );

    bCanUseThrowSkill = false;

    GetWorld()->GetTimerManager().SetTimer(
        ThrowCooldownTimerHandle,
        this,
        &UCombatComponent::ResetThrowCooldown,
        ThrowCooldown,
        false
    );

    UE_LOG(LogTemp, Warning, TEXT("Throw Skill Used"));
}

void UCombatComponent::ResetThrowCooldown()
{
    bCanUseThrowSkill = true;

    UE_LOG(LogTemp, Warning, TEXT("Throw Skill Ready"));
}

void UCombatComponent::ResetHitIndicator()
{
    bHitEnemy = false;
}

