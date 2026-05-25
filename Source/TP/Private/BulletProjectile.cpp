// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletProjectile.h"

#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

ABulletProjectile::ABulletProjectile()
{
    PrimaryActorTick.bCanEverTick = false;

    BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletMesh"));
    RootComponent = BulletMesh;

    BulletMesh->SetSimulatePhysics(false);
    BulletMesh->SetNotifyRigidBodyCollision(true);
    BulletMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    BulletMesh->SetCollisionObjectType(ECC_WorldDynamic);
    BulletMesh->SetCollisionResponseToAllChannels(ECR_Block);

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->InitialSpeed = 5000.f;
    ProjectileMovement->MaxSpeed = 5000.f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->ProjectileGravityScale = 0.f;

    InitialLifeSpan = 3.f;
}

void ABulletProjectile::BeginPlay()
{
    Super::BeginPlay();

    BulletMesh->OnComponentHit.AddDynamic(this, &ABulletProjectile::OnBulletHit);
}

void ABulletProjectile::OnBulletHit(
    UPrimitiveComponent* HitComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComponent,
    FVector NormalImpulse,
    const FHitResult& Hit
)
{
    if (!OtherActor || OtherActor == this || OtherActor == GetOwner())
    {
        return;
    }

    UGameplayStatics::ApplyDamage(
        OtherActor,
        Damage,
        GetInstigatorController(),
        this,
        nullptr
    );

    Destroy();
}
