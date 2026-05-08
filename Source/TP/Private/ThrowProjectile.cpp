// Fill out your copyright notice in the Description page of Project Settings.


#include "ThrowProjectile.h"

#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

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
    UGameplayStatics::ApplyRadialDamage(
        GetWorld(),
        ExplosionDamage,
        GetActorLocation(),
        ExplosionRadius,
        nullptr,
        TArray<AActor*>(),
        this,
        GetInstigatorController(),
        true
    );

    DrawDebugSphere(
        GetWorld(),
        GetActorLocation(),
        ExplosionRadius,
        32,
        FColor::Orange,
        false,
        2.0f
    );

    UE_LOG(LogTemp, Warning, TEXT("Projectile Exploded"));

    Destroy();
}