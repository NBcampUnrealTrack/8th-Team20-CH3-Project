//TPGameMode.cpp

#include "TPGameMode.h"
#include "TPCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "TimerManager.h"
#include "Blueprint/UserWidget.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

ATPGameMode::ATPGameMode()
{
    static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(
        TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter")
    );

    if (PlayerPawnBPClass.Class != NULL)
    {
        DefaultPawnClass = PlayerPawnBPClass.Class;
    }

    Score = 0;
    KillCount = 0;
    TargetKillCount = 10;
    //Ÿ�̸� �ð� ����
    TimeLimit = 30.0f;
    RemainingTime = TimeLimit;
}

void ATPGameMode::BeginPlay()
{
    Super::BeginPlay();

    FString CurrentLevelName = GetWorld()->GetMapName();

    if (CurrentLevelName.Contains(TEXT("TitleLevel")) ||
        CurrentLevelName.Contains(TEXT("MainMenuLevel")))
    {
        UE_LOG(LogTemp, Warning, TEXT("===== MENU LEVEL: TIMER NOT STARTED ====="));
        return;
    }

    StartGame();
}

void ATPGameMode::StartGame()
{
    Score = 0;
    KillCount = 0;
    TargetKillCount = 10;

    RemainingTime = TimeLimit;

    GetWorldTimerManager().SetTimer(
        GameTimerHandle,
        this,
        &ATPGameMode::UpdateTimer,
        1.0f,
        true
    );

    // HP 체크: 0.2초마다 실행
    GetWorldTimerManager().SetTimer(
        HealthCheckTimerHandle,
        this,
        &ATPGameMode::CheckPlayerHealth,
        0.2f,
        true
    );

    UE_LOG(LogTemp, Warning, TEXT("===== TIMER START ====="));
}

void ATPGameMode::UpdateTimer()
{
    RemainingTime -= 1.0f;

    UE_LOG(LogTemp, Warning, TEXT("Remaining Time: %.0f"), RemainingTime);

    if (RemainingTime <= 0.0f)
    {
        RemainingTime = 0.0f;
        GameOver();
    }
}

void ATPGameMode::CheckPlayerHealth()
{
    ATPCharacter* PlayerCharacter =
        Cast<ATPCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

    if (PlayerCharacter && PlayerCharacter->CurrentHealth <= 0.0f)
    {
        UE_LOG(LogTemp, Warning, TEXT("Player HP is 0"));
        GameOver();
    }
}


void ATPGameMode::GameOver()
{
    GetWorldTimerManager().ClearTimer(GameTimerHandle);
    GetWorldTimerManager().ClearTimer(HealthCheckTimerHandle);

    UE_LOG(LogTemp, Warning, TEXT("===== GAME OVER ====="));

    if (GameOverWidgetClass)
    {
        UUserWidget* GameOverWidget =
            CreateWidget<UUserWidget>(GetWorld(), GameOverWidgetClass);

        if (GameOverWidget)
        {
            GameOverWidget->AddToViewport();
        }
    }
}