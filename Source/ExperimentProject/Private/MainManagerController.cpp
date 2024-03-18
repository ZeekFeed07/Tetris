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

	TetrisManager->StartGame();
}

void AMainManagerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMainManagerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	LastTimeSec = GetWorld()->GetTimeSeconds();

	InputComponent->BindAxis("MovementSideways", this, &AMainManagerController::Movement_AD);
	InputComponent->BindAxis("MovementBottom", this, &AMainManagerController::Movement_S);

	InputComponent->BindAction("RotateInput", IE_Pressed, this, &AMainManagerController::Movement_W);
}

void AMainManagerController::Movement_AD(float Delta)
{
	float DeltaSec = GetWorld()->GetTimeSeconds() - LastTimeSec;

	if (DeltaSec > TetrisManager->GetMovingDelta() && Delta != 0)
	{
		TetrisManager->HandleMovementSideways(Delta);
		LastTimeSec = GetWorld()->GetTimeSeconds();
	}
}
void AMainManagerController::Movement_S(float Delta)
{
	float DeltaSec = GetWorld()->GetTimeSeconds() - LastTimeSec;

	if (DeltaSec > TetrisManager->GetDroppingDelta() && Delta > 0)
	{
		TetrisManager->HandleMovementBottom(Delta);
		LastTimeSec = GetWorld()->GetTimeSeconds();
	}
}

void AMainManagerController::Movement_W()
{
	TetrisManager->HandleMovementRotate();
}

AMainManager* AMainManagerController::GetManager()
{
	return TetrisManager;
}