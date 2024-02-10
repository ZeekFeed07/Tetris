#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "MainManager.h"

#include "MainManagerController.generated.h"

UCLASS()
class EXPERIMENTPROJECT_API AMainManagerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AMainManagerController();

protected: // Protected Methods
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void SetupInputComponent() override; 

	virtual void Movement_AD(float Delta);

	virtual void Movement_WS(float Delta);

public:

	UFUNCTION(BlueprintCallable)
	AMainManager* GetManager();

protected: // Protected Variables
	
	AMainManager* TetrisManager;

private:
	int32 TickCounter = 0;
	int32 TickCounter2 = 0;

	float LastLaunchSec = 0;

	const int32 RequiredTickBeforeMove = 3;

};
