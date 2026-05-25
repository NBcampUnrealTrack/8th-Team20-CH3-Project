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

AMyCharacter::AMyCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    GetCapsuleComponent()->InitCapsuleSize(DefaultCapsuleRadius, DefaultCapsuleHalfHeight);

    CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("ActorComponent"));

    WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
    WeaponMesh->SetupAttachment(GetMesh(), TEXT("WeaponSocket"));

    // ── 스프링 암 ────────────────────────────────
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent);
    SpringArm->TargetArmLength = 400.f;
    SpringArm->bUsePawnControlRotation = true;   // 카메라 붐은 컨트롤러(마우스) 방향을 따라감
    SpringArm->bEnableCameraLag = true;
    SpringArm->CameraLagSpeed = 10.f;

    // ── 카메라 ───────────────────────────────────
    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
    Camera->bUsePawnControlRotation = false;

    // ── ★ 핵심 회전 설정 ─────────────────────────
    // bOrientRotationToMovement = false : 이동 방향 자동 회전 OFF
    // bUseControllerRotationYaw = false : Tick에서 수동 보간 처리
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    GetCharacterMovement()->bOrientRotationToMovement = false; // ★ 이동 방향 자동 회전 OFF
    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
    GetCharacterMovement()->JumpZVelocity = JumpZVelocity;
    GetCharacterMovement()->AirControl = 0.35f;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);

    // ── 스탯 초기화 ──────────────────────────────
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

    // ★ 마우스 방향으로 캐릭터 몸 보간 회전
    RotateCharacterToController(DeltaTime);
}

// ──────────────────────────────────────────────────
// ★ MappingContext 등록 — NotifyControllerChanged에서 처리
//   SetupPlayerInputComponent 시점엔 Controller가 없을 수 있어
//   컨트롤러가 확실히 연결된 이후 호출되는 이 함수에서 등록
// ──────────────────────────────────────────────────
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

// ──────────────────────────────────────────────────
// 입력 바인딩 — 중복 캐스팅 제거, 단일 EIC 블록으로 통합
// ──────────────────────────────────────────────────
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent);
    if (!EIC) return;

    // 이동 / 시점
    EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyCharacter::Move);
    EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyCharacter::Look);

    // 달리기
    EIC->BindAction(RunAction, ETriggerEvent::Started, this, &AMyCharacter::StartRun);
    EIC->BindAction(RunAction, ETriggerEvent::Completed, this, &AMyCharacter::StopRun);

    // 점프
    EIC->BindAction(JumpAction, ETriggerEvent::Started, this, &AMyCharacter::Jump);
    EIC->BindAction(JumpAction, ETriggerEvent::Completed, this, &AMyCharacter::StopJumping);

    // 전투 (로직은 건드리지 않음)
    EIC->BindAction(BasicAttackAction, ETriggerEvent::Started, this, &AMyCharacter::BasicAction);
    EIC->BindAction(ReloadAction, ETriggerEvent::Started, this, &AMyCharacter::ReloadInput);
    EIC->BindAction(ThrowSkillAction, ETriggerEvent::Started, this, &AMyCharacter::ThrowSkillInput);

    // UI
    EIC->BindAction(TabAction, ETriggerEvent::Started, this, &AMyCharacter::ShowTabScoreboard);
    EIC->BindAction(TabAction, ETriggerEvent::Completed, this, &AMyCharacter::HideTabScoreboard);
}

// ──────────────────────────────────────────────────
// ★ 마우스 Yaw → 캐릭터 몸 보간 회전
// ──────────────────────────────────────────────────
void AMyCharacter::RotateCharacterToController(float DeltaTime)
{
    if (!Controller) return;

    // 컨트롤러 Yaw만 추출 (Pitch·Roll 제거 → 몸은 수평 회전만)
    const FRotator TargetRotation = FRotator(0.f, Controller->GetControlRotation().Yaw, 0.f);

    const FRotator NewRotation = FMath::RInterpTo(
        GetActorRotation(),
        TargetRotation,
        DeltaTime,
        RotationInterpSpeed   // 기본값 10.f, 에디터에서 조정 가능
    );

    SetActorRotation(NewRotation);
}

// ──────────────────────────────────────────────────
// 이동 — 카메라 Yaw 기준 방향 벡터로 입력
// ──────────────────────────────────────────────────
void AMyCharacter::Move(const FInputActionValue& Value)
{
    if (!Controller) return;

    const FVector2D MovementVector = Value.Get<FVector2D>();
    const FRotator  YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
    const FVector   Forward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    const FVector   Right = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    AddMovementInput(Forward, MovementVector.Y);
    AddMovementInput(Right, MovementVector.X);
}

// ──────────────────────────────────────────────────
// 시점 — 마우스 입력을 컨트롤러에 전달
//         Yaw → RotateCharacterToController()가 몸에 반영
//         Pitch → SpringArm이 카메라 각도만 변경 (몸 미반영)
// ──────────────────────────────────────────────────
void AMyCharacter::Look(const FInputActionValue& Value)
{
    const FVector2D LookVector = Value.Get<FVector2D>();
    AddControllerYawInput(LookVector.X);
    AddControllerPitchInput(LookVector.Y);
}

// ──────────────────────────────────────────────────
// 달리기
// ──────────────────────────────────────────────────
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

// ──────────────────────────────────────────────────
// 점프
// ──────────────────────────────────────────────────
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

// ──────────────────────────────────────────────────
// 상태 / 속도 / 캡슐 업데이트
// ──────────────────────────────────────────────────
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

// ── 전투/UI 로직은 원본 그대로 유지 ─────────────────
// BasicAction(), ReloadInput(), FinishReload(),
// ThrowSkillInput(), TakeDamageAmount(),
// ShowTabScoreboard(), HideTabScoreboard()
// ──────────────────────────────────────────────────
// 전투
// ──────────────────────────────────────────────────
void AMyCharacter::BasicAction()
{
    if (bIsReloading) return;

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
    if (bIsReloading) return;
    if (CurrentAmmo >= MaxAmmo) return;

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

void AMyCharacter::ThrowSkillInput()
{
    if (GrenadeCount <= 0)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("No Grenades"));
        return;
    }

    GrenadeCount--;
    GEngine->AddOnScreenDebugMessage(
        -1, 2.f, FColor::Green,
        FString::Printf(TEXT("Grenades Left: %d"), GrenadeCount)
    );

    if (CombatComponent)
    {
        CombatComponent->ThrowSkill();
    }
}

// ──────────────────────────────────────────────────
// 피해 / 사망
// ──────────────────────────────────────────────────
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

// ──────────────────────────────────────────────────
// UI — 탭 스코어보드
// ──────────────────────────────────────────────────
void AMyCharacter::ShowTabScoreboard()
{
    if (!TabScoreboardWidget && TabScoreboardClass)
    {
        TabScoreboardWidget = CreateWidget<UUserWidget>(
            Cast<APlayerController>(GetController()), TabScoreboardClass);
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