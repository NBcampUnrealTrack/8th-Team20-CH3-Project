#include "MyCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Animation/AnimInstance.h"
// ✅ Enhanced Input 헤더 추가
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include "CombatComponent.h"
#include "Blueprint/UserWidget.h"

AMyCharacter::AMyCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    GetCapsuleComponent()->InitCapsuleSize(DefaultCapsuleRadius, DefaultCapsuleHalfHeight);

    CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("ActorComponent"));


    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent);
    SpringArm->TargetArmLength = 400.f;
    SpringArm->bUsePawnControlRotation = true;
    SpringArm->bEnableCameraLag = true;
    SpringArm->CameraLagSpeed = 10.f;

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
    Camera->bUsePawnControlRotation = false;

    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
    GetCharacterMovement()->JumpZVelocity = JumpZVelocity;
    GetCharacterMovement()->AirControl = 0.35f;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);

    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    MaxHealth = 100.0f;
    CurrentHealth = MaxHealth;
    MaxAmmo = 30;
    CurrentAmmo = MaxAmmo;
    ReserveAmmo = 90;
    ReloadTime = 1.5f;
    bIsReloading = false;
    GrenadeCount = 3;

    WeaponName = TEXT("Rifle");
    Score = 0;
    KillCount = 0;
    MissionObjective = TEXT("Eliminate Targets");
    MissionProgress = 0;
    MissionGoal = 10;
    bIsDead = false;
}

void AMyCharacter::BeginPlay()
{
    Super::BeginPlay();
    UpdateCapsuleSize();
    UpdateMovementSpeed();
}

void AMyCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    CurrentSpeed = GetVelocity().Size2D();
    UpdateCharacterState();
}

// ✅ Enhanced Input 방식으로 전면 교체
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EnhancedInputComponent->BindAction(TabAction, ETriggerEvent::Started, this, &AMyCharacter::ShowTabScoreboard);
        EnhancedInputComponent->BindAction(TabAction, ETriggerEvent::Completed, this, &AMyCharacter::HideTabScoreboard);
    }

    // Enhanced Input Subsystem에 MappingContext 등록
    if (APlayerController* PC = Cast<APlayerController>(Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
            ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }

    if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        // 이동
        EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyCharacter::Move);
        // 시점
        EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyCharacter::Look);
        // 달리기
        EIC->BindAction(RunAction, ETriggerEvent::Started, this, &AMyCharacter::StartRun);
        EIC->BindAction(RunAction, ETriggerEvent::Completed, this, &AMyCharacter::StopRun);
        // 점프
        EIC->BindAction(JumpAction, ETriggerEvent::Started, this, &AMyCharacter::Jump);
        EIC->BindAction(JumpAction, ETriggerEvent::Completed, this, &AMyCharacter::StopJumping);
        EIC->BindAction(BasicAttackAction, ETriggerEvent::Started, this, &AMyCharacter::BasicAction);
        EIC->BindAction(ReloadAction, ETriggerEvent::Started, this, &AMyCharacter::ReloadInput);
        EIC->BindAction(ThrowSkillAction, ETriggerEvent::Started,this,&AMyCharacter::ThrowSkillInput);
    }
}

void AMyCharacter::Move(const FInputActionValue& Value)
{
    const FVector2D MovementVector = Value.Get<FVector2D>();
    if (Controller == nullptr) return;

    const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
    const FVector  Forward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    const FVector  Right = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    AddMovementInput(Forward, MovementVector.Y);
    AddMovementInput(Right, MovementVector.X);
}

void AMyCharacter::Look(const FInputActionValue& Value)
{
    const FVector2D LookVector = Value.Get<FVector2D>();
    AddControllerYawInput(LookVector.X);
    AddControllerPitchInput(LookVector.Y);
}

void AMyCharacter::StartRun()
{
    bIsRunning = true;
    UpdateMovementSpeed();
    UpdateCapsuleSize();
}

void AMyCharacter::StopRun()
{
    bIsRunning = false;
    UpdateMovementSpeed();
    UpdateCapsuleSize();
}

void AMyCharacter::Jump()
{
    if (GetCharacterMovement()->IsMovingOnGround())
    {
        bIsJumping = true;
        Super::Jump();

        if (JumpMontage)
        {
            if (UAnimInstance* Anim = GetMesh()->GetAnimInstance())
            {
                Anim->Montage_Play(JumpMontage, 1.f);
            }
        }
    }
}

void AMyCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
    Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
    if (GetCharacterMovement()->IsMovingOnGround())
    {
        bIsJumping = false;
    }
}

void AMyCharacter::UpdateCharacterState()
{
    if (bIsJumping || GetCharacterMovement()->IsFalling())
    {
        CharacterState = GetCharacterMovement()->Velocity.Z > 0.f
            ? ECharacterState::Jumping
            : ECharacterState::Falling;
        return;
    }

    CharacterState = (CurrentSpeed > 10.f)
        ? (bIsRunning ? ECharacterState::Running : ECharacterState::Walking)
        : ECharacterState::Idle;
}

void AMyCharacter::UpdateMovementSpeed()
{
    GetCharacterMovement()->MaxWalkSpeed = bIsRunning ? RunSpeed : WalkSpeed;
}

void AMyCharacter::BasicAction()
{
    if (bIsReloading)
    {
        return;
    }

    if (CurrentAmmo <= 0)
    {
        ReloadInput();
        return;
    }

    CurrentAmmo--;

    if (CombatComponent)
    {
        CombatComponent->BasicAttack();
    }

    UE_LOG(LogTemp, Warning, TEXT("Ammo: %d / %d"), CurrentAmmo, ReserveAmmo);
}

void AMyCharacter::ReloadInput()
{
    if (bIsReloading)
    {
        return;
    }

    if (CurrentAmmo >= MaxAmmo)
    {
        return;
    }

    if (ReserveAmmo <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No Reserve Ammo"));
        return;
    }

    bIsReloading = true;

    UE_LOG(LogTemp, Warning, TEXT("Reloading..."));

    GetWorldTimerManager().SetTimer(
        ReloadTimerHandle,
        this,
        &AMyCharacter::FinishReload,
        ReloadTime,
        false
    );
}

void AMyCharacter::FinishReload()
{
    const int32 NeededAmmo = MaxAmmo - CurrentAmmo;
    const int32 AmmoToReload = FMath::Min(NeededAmmo, ReserveAmmo);

    CurrentAmmo += AmmoToReload;
    ReserveAmmo -= AmmoToReload;

    bIsReloading = false;

    UE_LOG(LogTemp, Warning, TEXT("Reload Complete: %d / %d"), CurrentAmmo, ReserveAmmo);
}

void AMyCharacter::UpdateCapsuleSize()
{
    const float TargetHalfHeight = bIsRunning ? RunCapsuleHalfHeight : DefaultCapsuleHalfHeight;
    GetCapsuleComponent()->SetCapsuleSize(DefaultCapsuleRadius, TargetHalfHeight);
}

void AMyCharacter::ThrowSkillInput()
{
    if (GrenadeCount <= 0)
    {
        GEngine->AddOnScreenDebugMessage
        (
            -1,
            2.f,
            FColor::Red,
            TEXT("No Grenades")
        );

        return;
    }

    GrenadeCount--;

    GEngine->AddOnScreenDebugMessage(
        -1,
        2.f,
        FColor::Green,
        FString::Printf(TEXT("Grenades Left : %d"), GrenadeCount)
    );

    if (CombatComponent)
    {
        CombatComponent->ThrowSkill();
    }
}

void AMyCharacter::TakeDamageAmount(float DamageAmount)
{
    if (bIsDead) return;

    CurrentHealth -= DamageAmount;
    if (CurrentHealth <= 0.0f)
    {
        CurrentHealth = 0.0f;
        bIsDead = true;
    }
}

void AMyCharacter::ShowTabScoreboard()
{
    if (!TabScoreboardWidget && TabScoreboardClass)
    {
        TabScoreboardWidget = CreateWidget<UUserWidget>(Cast<APlayerController>(GetController()), TabScoreboardClass);
    }
    if (TabScoreboardWidget && !TabScoreboardWidget->IsInViewport())
    {
        TabScoreboardWidget->AddToViewport(10);
    }
}

void AMyCharacter::HideTabScoreboard()
{
    if (TabScoreboardWidget && TabScoreboardWidget->IsInViewport())
    {
        TabScoreboardWidget->RemoveFromParent();
    }
}