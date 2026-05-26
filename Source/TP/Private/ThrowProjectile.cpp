// Fill out your copyright notice in the Description page of Project Settings.


#include "ThrowProjectile.h"

#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Engine/OverlapResult.h"

AThrowProjectile::AThrowProjectile()
{
    PrimaryActorTick.bCanEverTick = false;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    RootComponent = Mesh;

    Mesh->SetSimulatePhysics(false);
    Mesh->SetNotifyRigidBodyCollision(true);

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->InitialSpeed = 1500.0f;
    ProjectileMovement->MaxSpeed = 1500.0f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->ProjectileGravityScale = 1.0f;
}

void AThrowProjectile::BeginPlay()
{
    Super::BeginPlay();

    Mesh->OnComponentHit.AddDynamic(this, &AThrowProjectile::OnProjectileHit);
}

void AThrowProjectile::OnProjectileHit(
    UPrimitiveComponent* HitComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComponent,
    FVector NormalImpulse,
    const FHitResult& Hit
)
{
    Explode();
}

void AThrowProjectile::Explode()
{
    TArray<FOverlapResult> OverlapResults;

    FCollisionShape Sphere = FCollisionShape::MakeSphere(ExplosionRadius);

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);

    bool bHasOverlap = GetWorld()->OverlapMultiByObjectType(
        OverlapResults,
        GetActorLocation(),
        FQuat::Identity,
        FCollisionObjectQueryParams(ECC_Pawn),
        Sphere,
        QueryParams
    );

    if (bHasOverlap)
    {
        for (const FOverlapResult& Result : OverlapResults)
        {
            AActor* HitActor = Result.GetActor();

            if (!HitActor)
            {
                continue;
            }

            FVector Start = GetActorLocation();
            FVector End = HitActor->GetActorLocation();

            FHitResult BlockHit;

            FCollisionQueryParams LineParams;
            LineParams.AddIgnoredActor(this);
            LineParams.AddIgnoredActor(HitActor);

            bool bBlocked = GetWorld()->LineTraceSingleByChannel(
                BlockHit,
                Start,
                End,
                ECC_Visibility,
                LineParams
            );

            // 중간에 벽이나 오브젝트가 막고 있으면 스킵
            if (bBlocked)
            {
                continue;
            }

            UGameplayStatics::ApplyDamage(
                HitActor,
                ExplosionDamage,
                GetInstigatorController(),
                this,
                nullptr
            );

            UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *HitActor->GetName());
        }
    }

    /*DrawDebugSphere(
        GetWorld(),
        GetActorLocation(),
        ExplosionRadius,
        32,
        FColor::Orange,
        false,
        2.0f
    );*/

    UE_LOG(LogTemp, Warning, TEXT("Projectile Exploded"));

    Destroy();
}