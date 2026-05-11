#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ThrowProjectile.generated.h"

class UStaticMeshComponent;
class UProjectileMovementComponent;

UCLASS()
class TP_API AThrowProjectile : public AActor
{
    GENERATED_BODY()

public:
    AThrowProjectile();

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(VisibleAnywhere, Category = "Component")
    UStaticMeshComponent* Mesh;

    UPROPERTY(VisibleAnywhere, Category = "Component")
    UProjectileMovementComponent* ProjectileMovement;

    UPROPERTY(EditAnywhere, Category = "Damage")
    float ExplosionDamage = 50.0f;

    UPROPERTY(EditAnywhere, Category = "Damage")
    float ExplosionRadius = 400.0f;

private:
    UFUNCTION()
    void OnProjectileHit(
        UPrimitiveComponent* HitComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComponent,
        FVector NormalImpulse,
        const FHitResult& Hit
    );

    void Explode();
};