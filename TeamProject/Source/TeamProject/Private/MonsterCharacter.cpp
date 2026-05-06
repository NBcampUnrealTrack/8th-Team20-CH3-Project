#include "MonsterCharacter.h"
#include "MonsterAIController.h"
#include "GameFramework//CharacterMovementComponent.h"

AMonsterCharacter::AMonsterCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	AIControllerClass = AMonsterAIController::StaticClass(); // 이 몬스터를 조종할 AI 컨트롤러 클래스 지정.
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned; // 몬스터가 레밸에 배치되거나 스폰되었을때 자동으로 AI 컨트롤러가 붙도록 설정.
}

void AMonsterCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	CurrentHealth = MaxHealth;
}

void AMonsterCharacter::Attack(AActor* TagetActor) // 구현은 각 자식클래스에서.
{

}


