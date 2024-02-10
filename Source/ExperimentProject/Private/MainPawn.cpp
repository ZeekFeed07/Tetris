#include "MainPawn.h"

AMainPawn::AMainPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	// Для того чтобы объект мог перемещаться в пространстве
	SetRootComponent(CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Main")));
}

void AMainPawn::BeginPlay()
{
	Super::BeginPlay();
}

void AMainPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMainPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

