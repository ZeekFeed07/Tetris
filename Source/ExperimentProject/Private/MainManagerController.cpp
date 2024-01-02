#include "MainManagerController.h"

AMainManagerController::AMainManagerController()
{

}

void AMainManagerController::BeginPlay()
{
	Super::BeginPlay();

	TetrisManager = Cast<AMainManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AMainManager::StaticClass()));
	if (TetrisManager)
	{
		TetrisManager->SetController(this);
	}
	else
	{
		UE_LOG(LogMainManager, Error, TEXT("Tetris Manager doesn't exist. (MainManagerController.cpp | AMainManagerController::AMainManagerController"))
	}
}

void AMainManagerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	++TickCounter;
	++TickCounter2;
}

void AMainManagerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAxis("MovementSideways", this, &AMainManagerController::Movement_AD);
	InputComponent->BindAxis("MovementFrontBack", this, &AMainManagerController::Movement_WS);
	//UE_LOG(LogMainManager, Display, TEXT("%d"), TickCounter)
	//InputComponent->BindAction("MoveLeft", IE_Pressed, this, &AMainManagerController::MovementHandleLeft);
	//InputComponent->BindAction("MoveRight", IE_Pressed, this, &AMainManagerController::MovementHandleRight);
}

void AMainManagerController::Movement_AD(float Delta)
{
	if (TickCounter > RequiredTickBeforeMove)
	{
		TetrisManager->HandleMovementSideways(Delta);
		TickCounter = 0;
	}
}
void AMainManagerController::Movement_WS(float Delta)
{
	if (Delta < 0 && GetWorld()->GetTimeSeconds() - LastLaunchSec > 0.3f)
	{
		TetrisManager->HandleMovementFrontBack(Delta);

		LastLaunchSec = GetWorld()->GetTimeSeconds();
	}
	else if (Delta >= 0)
	{
		TetrisManager->HandleMovementFrontBack(Delta);
	}
}