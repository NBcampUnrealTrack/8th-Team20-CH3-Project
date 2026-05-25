#include "MyCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Animation/AnimInstance.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include "CombatComponent.h"
#include "Blueprint/UserWidget.h"
#include "Engine/Engine.h"

AMyCharacter::AMyCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    GetCapsuleComponent()->InitCapsuleSize(DefaultCapsuleRadius, DefaultCapsuleHalfHeight);
    CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("ActorComponent"));

    WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
    WeaponMesh->SetupAttachment(GetMesh(), TEXT("WeaponSocket"));

    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent);
    SpringArm->TargetArmLength = 400.f;
    SpringArm->bUsePawnControlRotation = true;
    SpringArm->bEnableCameraLag = true;
    SpringArm->CameraLagSpeed = 10.f;

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
    Camera->bUsePawnControlRotation = false;

    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    GetCharacterMovement()->bOrientRotationToMovement = false;
    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
    GetCharacterMovement()->JumpZVelocity = JumpZVelocity;
    GetCharacterMovement()->AirControl = 0.35f;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);

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
    MissionObjective = TEXT("Eliminate All Targets");
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
    RotateCharacterToController(DeltaTime);
}

void AMyCharacter::NotifyControllerChanged()
{
    Super::NotifyControllerChanged();

    if (APlayerController* PC = Cast<APlayerController>(Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
            ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }
}

void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent);
    if (!EIC) return;

    EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyCharacter::Move);
    EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyCharacter::Look);

    EIC->BindAction(RunAction, ETriggerEvent::Started, this, &AMyCharacter::StartRun);
    EIC->BindAction(RunAction, ETriggerEvent::Completed, this, &AMyCharacter::StopRun);

    EIC->BindAction(JumpAction, ETriggerEvent::Started, this, &AMyCharacter::Jump);
    EIC->BindAction(JumpAction, ETriggerEvent::Completed, this, &AMyCharacter::StopJumping);

    EIC->BindAction(FireAction, ETriggerEvent::Started, this, &AMyCharacter::BasicAction);
    EIC->BindAction(ReloadAction, ETriggerEvent::Started, this, &AMyCharacter::ReloadInput);
    EIC->BindAction(ThrowAction, ETriggerEvent::Started, this, &AMyCharacter::ThrowSkillInput);

    EIC->BindAction(TabAction, ETriggerEvent::Started, this, &AMyCharacter::ShowTabScoreboard);
    EIC->BindAction(TabAction, ETriggerEvent::Completed, this, &AMyCharacter::HideTabScoreboard);
}

void AMyCharacter::RotateCharacterToController(float DeltaTime)
{
    if (!Controller) return;

    const FRotator TargetRotation = FRotator(0.f, Controller->GetControlRotation().Yaw, 0.f);
    const FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, RotationInterpSpeed);
    SetActorRotation(NewRotation);
}

void AMyCharacter::Move(const FInputActionValue& Value)
{
    if (!Controller) return;

    const FVector2D MovementVector = Value.Get<FVector2D>();
    const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
    const FVector Forward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    const FVector Right = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

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
        PlayCharacterMontage(JumpMontage);
    }
}

void AMyCharacter::StopJumping()
{
    Super::StopJumping();
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
        CharacterState = GetCharacterMovement()->Velocity.Z > 0.f ? ECharacterState::Jumping : ECharacterState::Falling;
        return;
    }

    CharacterState = (CurrentSpeed > 10.f) ? (bIsRunning ? ECharacterState::Running : ECharacterState::Walking) : ECharacterState::Idle;
}

void AMyCharacter::UpdateMovementSpeed()
{
    GetCharacterMovement()->MaxWalkSpeed = bIsRunning ? RunSpeed : WalkSpeed;
}

void AMyCharacter::UpdateCapsuleSize()
{
    const float TargetHalfHeight = bIsRunning ? RunCapsuleHalfHeight : DefaultCapsuleHalfHeight;
    GetCapsuleComponent()->SetCapsuleSize(DefaultCapsuleRadius, TargetHalfHeight);
}

void AMyCharacter::BasicAction()
{
    if (bIsReloading || bIsDead) return;

    if (CurrentAmmo <= 0)
    {
        ReloadInput();
        return;
    }

    CurrentAmmo--;

    PlayCharacterMontage(ShootMontage);

    if (CombatComponent)
    {
        CombatComponent->BasicAttack();
    }

    UE_LOG(LogTemp, Warning, TEXT("Ammo: %d / %d"), CurrentAmmo, ReserveAmmo);
}

void AMyCharacter::ReloadInput()
{
    if (bIsReloading || bIsDead) return;
    if (CurrentAmmo >= MaxAmmo) return;

    if (ReserveAmmo <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No Reserve Ammo"));
        return;
    }

    bIsReloading = true;
    UE_LOG(LogTemp, Warning, TEXT("Reloading..."));

    PlayCharacterMontage(ReloadMontage);

    GetWorldTimerManager().SetTimer(ReloadTimerHandle, this, &AMyCharacter::FinishReload, ReloadTime, false);
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

// ★ 수류탄 작동 무조건 성공하게끔 보장하는 핵심 수정본 함수
void AMyCharacter::ThrowSkillInput()
{
    if (bIsDead) return;

    // [수정 1] 키가 눌렸음을 알리는 온스크린 디버그 문구를 조건문 맨 위로 탈출시켜 강제 실행 보장
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("Q Key Input Success! - Throw Skill"));
    }

    // [수정 2] 개수 검사 전에 애니메이션부터 무조건 실행하도록 선배치
    if (ThrowGrenadeMontage)
    {
        PlayCharacterMontage(ThrowGrenadeMontage);
    }

    // [수정 3] 개수 제약 조건 및 미사용 텍스트는 내부 기능 처리를 방해하지 않도록 안전하게 분리
    if (GrenadeCount <= 0)
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Warning: Grenade Count is 0"));
        }
        return;
    }

    GrenadeCount--;

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

    UE_LOG(LogTemp, Warning, TEXT("TakeDamage 들어옴"));
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

void AMyCharacter::PlayCharacterMontage(UAnimMontage* MontageToPlay)
{
    if (bIsDead || !MontageToPlay) return;

    if (GetMesh() && GetMesh()->GetAnimInstance())
    {
        PlayAnimMontage(MontageToPlay);
    }
}