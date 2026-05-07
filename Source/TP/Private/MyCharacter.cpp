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

AMyCharacter::AMyCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    GetCapsuleComponent()->InitCapsuleSize(DefaultCapsuleRadius, DefaultCapsuleHalfHeight);

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

void AMyCharacter::UpdateCapsuleSize()
{
    const float TargetHalfHeight = bIsRunning ? RunCapsuleHalfHeight : DefaultCapsuleHalfHeight;
    GetCapsuleComponent()->SetCapsuleSize(DefaultCapsuleRadius, TargetHalfHeight);
}