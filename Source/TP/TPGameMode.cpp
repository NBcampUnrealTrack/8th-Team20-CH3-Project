//TPGameMode.cpp

#include "TPGameMode.h"
#include "MyCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "TimerManager.h"
#include "Blueprint/UserWidget.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

ATPGameMode::ATPGameMode()
{
    static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(
        TEXT("/Game/Blueprint/BP_MyCharacter")
    );

    if (PlayerPawnBPClass.Class != NULL)
    {
        DefaultPawnClass = PlayerPawnBPClass.Class;
    }

    Score = 0;
    KillCount = 0;
    TargetKillCount = 10;
    //타이머 설정
    TimeLimit = 120.0f;
    RemainingTime = TimeLimit;
}

void ATPGameMode::BeginPlay()
{
    Super::BeginPlay();

    FString CurrentLevelName = GetWorld()->GetMapName();

    if (CurrentLevelName.Contains(TEXT("TitleLevel")) ||
        CurrentLevelName.Contains(TEXT("MainMenuLevel")) ||
        CurrentLevelName.Contains(TEXT("LoadingLevel")))
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
    AMyCharacter* PlayerCharacter =
        Cast<AMyCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

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

FText ATPGameMode::GetFormattedTime() const
{
    int32 TotalSeconds = FMath::Max(0, FMath::FloorToInt(RemainingTime));

    int32 Minutes = TotalSeconds / 60;
    int32 Seconds = TotalSeconds % 60;

    FString TimeString = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);

    return FText::FromString(TimeString);
}