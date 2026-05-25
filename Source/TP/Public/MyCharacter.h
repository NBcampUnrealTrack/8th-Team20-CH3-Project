#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Blueprint/UserWidget.h"
#include "MyCharacter.generated.h"

class UCombatComponent;
class UInputMappingContext;
class UInputAction;
class UAnimMontage;
struct FInputActionValue;

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
    Idle        UMETA(DisplayName = "Idle"),
    Walking     UMETA(DisplayName = "Walking"),
    Running     UMETA(DisplayName = "Running"),
    Jumping     UMETA(DisplayName = "Jumping"),
    Falling     UMETA(DisplayName = "Falling"),
};

UCLASS()
class TP_API AMyCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AMyCharacter();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float CurrentHealth;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float MaxHealth;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    int32 CurrentAmmo;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    int32 MaxAmmo;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    int32 ReserveAmmo = 90;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    float ReloadTime = 1.5f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
    bool bIsReloading = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grenade")
    int32 GrenadeCount = 3;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    FString WeaponName;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
    class UStaticMeshComponent* WeaponMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Score")
    int32 Score;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Score")
    int32 KillCount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
    FString MissionObjective;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
    int32 MissionProgress;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
    int32 MissionGoal;

    UFUNCTION(BlueprintCallable, Category = "Stats")
    void TakeDamageAmount(float DamageAmount);

    UPROPERTY(BlueprintReadOnly, Category = "Stats")
    bool bIsDead;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UUserWidget> TabScoreboardClass;

    UPROPERTY()
    UUserWidget* TabScoreboardWidget;

    void ShowTabScoreboard();
    void HideTabScoreboard();

protected:
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    virtual void NotifyControllerChanged() override;

public:
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    class USpringArmComponent* SpringArm;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    class UCameraComponent* Camera;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Speed")
    float WalkSpeed = 400.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Speed")
    float RunSpeed = 800.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Speed")
    float JumpZVelocity = 600.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Rotation")
    float RotationInterpSpeed = 10.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Capsule")
    float DefaultCapsuleHalfHeight = 88.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Capsule")
    float DefaultCapsuleRadius = 34.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Capsule")
    float RunCapsuleHalfHeight = 80.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
    ECharacterState CharacterState = ECharacterState::Idle;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
    bool bIsRunning = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
    bool bIsJumping = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
    float CurrentSpeed = 0.f;

    // ── Enhanced Input 에셋 매칭명으로 직관화 ──────────────────
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputMappingContext* DefaultMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* MoveAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* LookAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* RunAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* JumpAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* FireAction;          // IA_Fire 매칭용

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* ReloadAction;        // IA_Reload_Am 매칭용

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* ThrowAction;         // IA_Grow_Throw 매칭용

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
    UInputAction* TabAction;

    // ── 애니메이션 몽타주 (★ 사격 몽타주 추가) ──────────────────
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* JumpMontage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* ShootMontage;        // ★ 추가 완료!

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* ThrowGrenadeMontage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* ReloadMontage;

    UFUNCTION(BlueprintCallable, Category = "Animation")
    void PlayCharacterMontage(UAnimMontage* MontageToPlay);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    UCombatComponent* CombatComponent;

private:
    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    void StartRun();
    void StopRun();

    virtual void Jump() override;
    virtual void StopJumping() override;
    virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;

    void RotateCharacterToController(float DeltaTime);
    void UpdateCharacterState();
    void UpdateCapsuleSize();
    void UpdateMovementSpeed();

    void BasicAction();
    void ReloadInput();
    void FinishReload();
    void ThrowSkillInput();

    FTimerHandle ReloadTimerHandle;
};