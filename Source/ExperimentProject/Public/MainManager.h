#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseGeometry.h"
#include "Chaos/Pair.h"

#include "MainManager.generated.h"

#define ACTIVATED true
#define DEACTIVATED false

DECLARE_LOG_CATEGORY_EXTERN(LogMainManager, All, All)

using Coord = Chaos::Pair<int32, int32>;

// Тип фигуры
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
// Нормаль фигуры
UENUM(BlueprintType)
enum class EFigureDirection : uint8
{
	TOP,
	RIGHT,
	BOTTOM, 
	LEFT
};

UCLASS()
class EXPERIMENTPROJECT_API AMainManager : public AActor
{
	GENERATED_BODY()

public:	// Constructors
	AMainManager();

protected: // Protected Methods
	virtual void BeginPlay() override;

public:	// Public Methods
	virtual void Tick(float DeltaTime) override;

	virtual void StartGame(const FString PlayerName);

	virtual void EndGame();

	virtual bool IsGameEnded();

	virtual void UpdateFigure();

	virtual void AddFigureToField(const Coord StartCoords = Coord(0, 0));

	UFUNCTION() // Макрос нужен для таймера
	virtual void DropFigure();

	virtual bool IsIntersect(EFigureDirection MovingDirection);

	virtual void SelectFigure(const EFigureType Type, const EFigureDirection Direction = EFigureDirection::TOP);

	virtual void IncreaseScore(const int32 NumToIncrease);

	virtual EFigureType RandomFigure();

	virtual void HandleMovementSideways(float Delta);

	virtual void HandleMovementFrontBack(float Delta);

	// ===================== Getters ===================== //
	virtual float GetTimerRate() const;

private: // Private Methods
	void PrintFieldInLog() const;

	virtual void InitMovement(int32 DeltaY, int32 DeltaZ, bool bIsVanish);

	virtual void CheckAndClearLine();

	virtual void CheckAndRotate();

	virtual void DropField(int32 Line);

private: // Private Variables
	UPROPERTY(EditAnywhere, Category = "Main Settings")
	int32 _Score = 0;

	UPROPERTY(VisibleAnywhere, Category = "Main Settings")
	bool _bIsGameActive = ACTIVATED;

	UPROPERTY(EditAnywhere, Category = "Main Settings")
	float _TimerRate = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Main Settings")
	EFigureType _FirstFigure = EFigureType::SQUARE;

	UPROPERTY(EditAnywhere, Category = "Main Settings")
	FVector _SpawnCoordinates = FVector(0, 400, 1900);

	UPROPERTY(EditInstanceOnly, Category = "Main Settings")
	TArray<UMaterialInterface*> _ListOfMaterials;

	const int32 FigureTypeNum = 7;

	const float _BottomLine = 50;

	UWorld* _World;
	FTimerHandle _TileMovingTimer;
	
	// Массив содержащий всю фигуру
	TArray<ABaseGeometry*> _CurrentFigure;
	// Ось вращения
	Coord _CurrFigurePivot;
	// Координаты фигуры в массиве _FieldOfGeometry
	TArray<Coord> _FigureCoords;
	float _Degrees = -UE_HALF_PI;

	// Массив поля фигур
	TArray<TArray<ABaseGeometry*>> _FieldOfGeometry;

	// Высота и ширина поля
	const int32 _HeightOfField = 20;
	const int32 _WidthOfField = 10;
};

Coord RotateCoord(Coord C1, Coord C2, float Angle);