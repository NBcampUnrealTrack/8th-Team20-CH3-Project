//TPGameMode.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Blueprint/UserWidget.h"
#include "TPGameMode.generated.h"

UCLASS(minimalapi)
class ATPGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    ATPGameMode();

protected:
    virtual void BeginPlay() override;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Rule")
    int32 Score;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Rule")
    int32 KillCount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Rule")
    int32 TargetKillCount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Rule")
    float TimeLimit;

    UPROPERTY(BlueprintReadOnly, Category = "Game Rule")
    float RemainingTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> GameOverWidgetClass;

public:
    UFUNCTION(BlueprintCallable, Category = "Game Rule")
    void StartGame();
    
    UFUNCTION(BlueprintPure, Category = "Game Rule")
    FText GetFormattedTime() const;

private:
    FTimerHandle GameTimerHandle;
    FTimerHandle HealthCheckTimerHandle;

    void UpdateTimer();
    void CheckPlayerHealth();
    void GameOver();
};