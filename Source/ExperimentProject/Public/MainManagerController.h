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

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void SetupInputComponent() override; 

	virtual void Movement_AD(float Delta);

	virtual void Movement_S(float Delta);

	virtual void Movement_W();

public:

	UFUNCTION(BlueprintCallable)
	AMainManager* GetManager();

protected:
	
	AMainManager* TetrisManager;

private:
	float LastTimeSec = 0;

	const int32 RequiredTickBeforeMove = 3;

};
