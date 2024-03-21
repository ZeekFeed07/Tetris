#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseGeometry.h"
#include "Chaos/Pair.h"
#include "Blueprint/UserWidget.h"
#include "Runtime/UMG/Public/UMG.h"

#include "MainManager.generated.h"

#define ACTIVATED true
#define DEACTIVATED false

DECLARE_LOG_CATEGORY_EXTERN(LogMainManager, All, All)
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDelegateBP);

USTRUCT(Blueprintable)
struct FCoord
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	int32 First;

	UPROPERTY(BlueprintReadWrite)
	int32 Second;

	
	FCoord(int32 first = 0, int32 second = 0) : First(first), Second(second)
	{
	}
};

// Type of figure
UENUM(BlueprintType)
enum class EFigureType : uint8
{
	SQUARE,
	UNDERLINE,
	THREESIDE,
	Z_TYPE,
	Z_TYPE_M, // Mirrored
	L_TYPE,
	L_TYPE_M  // Mirrored
};
// Figure normals
UENUM(BlueprintType)
enum class EFigureDirection : uint8
{
	TOP,
	RIGHT,
	BOTTOM, 
	LEFT
};

USTRUCT(Blueprintable)
struct FStatData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	float GameTime = 0.f;

	UPROPERTY(BlueprintReadWrite)
	int32 Score = 0;

	UPROPERTY(BlueprintReadWrite)
	int32 Coins = 0;
};

UCLASS()
class EXPERIMENTPROJECT_API AMainManager : public AActor
{
	GENERATED_BODY()

public:
	AMainManager();

protected:
	virtual void BeginPlay() override;

public:

	virtual void Tick(float DeltaTime) override;

	virtual void StartGame();

	virtual void EndGame();

	virtual bool IsGameEnded();

	virtual void UpdateFigure();

	virtual void AddFigureToField(const FCoord StartCoords = FCoord(0, 0));

	UFUNCTION() // Macro for timer
	virtual void DropFigure();

	virtual bool IsIntersect(EFigureDirection MovingDirection);

	UFUNCTION(BlueprintCallable)
	virtual bool CoordsFromType(EFigureType Type, TArray<FCoord>& NewFigure, FCoord& Pivot);

	virtual void SelectFigure(const EFigureType Type, const EFigureDirection Direction = EFigureDirection::TOP);

	virtual void IncreaseScore(const int32 NumToIncrease);

	virtual EFigureType RandomFigure();

	// ===================== Events ===================== //

	virtual void HandleMovementSideways(float Delta);

	virtual void HandleMovementBottom(float Delta);

	virtual void HandleMovementRotate();

	UPROPERTY(BlueprintAssignable)
	FDelegateBP FigureSelected;

	// ===================== Getters ===================== //
	UFUNCTION(BlueprintCallable)
	virtual float GetDropTimerRate() const;

	UFUNCTION(BlueprintCallable)
	virtual float GetMovingDelta() const;

	UFUNCTION(BlueprintCallable)
	virtual float GetDroppingDelta() const;

	UFUNCTION(BlueprintCallable)	
	virtual FStatData GetResultData() const;

	UFUNCTION(BlueprintCallable)
	virtual EFigureType GetNextFigureType() const;

	UFUNCTION(BlueprintCallable)
	virtual UMaterialInterface* GetNextMaterial() const;

	// ===================== Setters ===================== //
	virtual void SetController(APlayerController* Controller);

private:
	void PrintFieldInLog() const;

	virtual void InitPointer(int32 DeltaY, int32 DeltaZ, bool bIsVanish);

	virtual void CheckAndClearLine();

	virtual void CheckAndRotate();

	virtual void DropField(int32 Line);

private:

	UPROPERTY(VisibleAnywhere, Category = "Main Settings")
	bool _bIsGameActive = DEACTIVATED;

	UPROPERTY(EditAnywhere, Category = "Main Settings")
	float _DropTimerRate = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Main Settings")
	float _DroppingDelta = .3f;

	UPROPERTY(EditAnywhere, Category = "Main Settings")
	float _MovingDelta = .3f;

	UPROPERTY(EditAnywhere, Category = "Main Settings")
	EFigureType _FirstFigure = EFigureType::SQUARE;

	UPROPERTY(EditAnywhere, Category = "Main Settings")
	FVector _SpawnCoordinates = FVector(0, 400.0f, 1900.0f);

	UPROPERTY(EditInstanceOnly, Category = "Main Settings")
	TArray<UMaterialInterface*> _ListOfMaterials;

	UMaterialInterface* _CurrentMaterial;
	UMaterialInterface* _NextMaterial;

	UPROPERTY(EditAnywhere, Category = "Main Settings")
	UStaticMesh* _BaseGeometryMesh;

	UPROPERTY(EditAnywhere, Category = "UI")
	UUserWidget* _EndGameWidget;

	UPROPERTY(EditAnywhere, Category = "UI")
	UUserWidget* _MainGameplayWidget;

	FStatData _ResultData;

	const int32 _FigureTypeNum = 7;

	EFigureType _CurrentFigureType;
	EFigureType _NextFigureType;
	
	const float _BottomLine = 50;

	UWorld* _World;
	APlayerController* _Controller = nullptr;

	FTimerHandle _TileMovingTimer;
	FTimerHandle _TimeCountingTimer;
	
	// Array containing the Figure
	TArray<ABaseGeometry*> _CurrentFigure;
	// Rotational axis
	FCoord _CurrFigurePivot;
	// Array of rotation _FieldOfGeometry coordinates 
	TArray<FCoord> _FigureCoords;
	float _Degrees = -UE_HALF_PI;
	 
	// Matrix of all figures
	TArray<TArray<ABaseGeometry*>> _FieldOfGeometry;

	// Field height and widht
	const int32 _HeightOfField = 20;
	const int32 _WidthOfField = 10;
};

FCoord RotateCoord(FCoord C1, FCoord C2, float Angle);