#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyCharacter.generated.h"


UENUM(BlueprintType)
enum class ECharacterState : uint8
{
    Idle        UMETA(DisplayName = "Idle"),
    Walking     UMETA(DisplayName = "Walking"),
    Running     UMETA(DisplayName = "Running"),
    Jumping     UMETA(DisplayName = "Jumping"),
    Falling     UMETA(DisplayName = "Falling"),
};

// ✅ MYGAME_API → TP_API 로 수정
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

public:
    virtual void Tick(float DeltaTime) override;

    // 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    class USpringArmComponent* SpringArm;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    class UCameraComponent* Camera;

    // 이동 속도
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Speed")
    float WalkSpeed = 400.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Speed")
    float RunSpeed = 800.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Speed")
    float JumpZVelocity = 600.f;

    // 캡슐 크기
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Capsule")
    float DefaultCapsuleHalfHeight = 88.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Capsule")
    float DefaultCapsuleRadius = 34.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Capsule")
    float RunCapsuleHalfHeight = 80.f;

    // 상태
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
    ECharacterState CharacterState = ECharacterState::Idle;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
    bool bIsRunning = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
    bool bIsJumping = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
    float CurrentSpeed = 0.f;

    // ✅ Enhanced Input Actions (에디터에서 할당)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputMappingContext* DefaultMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* MoveAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* LookAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* RunAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* JumpAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* BasicAttackAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* ReloadAction;

    // 애니메이션 몽타주
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    class UAnimMontage* JumpMontage;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    class UCombatComponent* CombatComponent;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputAction* ThrowSkillAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    UInputAction* TabAction;


private:
    // ✅ Enhanced Input 콜백 (float 파라미터 제거)
    void Move(const struct FInputActionValue& Value);
    void Look(const struct FInputActionValue& Value);
    void StartRun();
    void StopRun();

    virtual void Jump() override;
    virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;

    void UpdateCharacterState();
    void UpdateCapsuleSize();
    void UpdateMovementSpeed();
    void BasicAction(); 
    
    void ReloadInput();
    void FinishReload();

    void ThrowSkillInput();

    FTimerHandle ReloadTimerHandle;
};