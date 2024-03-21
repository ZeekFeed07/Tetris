#include "MainManager.h"
#include "..\Public\MainManager.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogMainManager)

#define CLEAR true
#define INITIALIZE false

//###############################################################################################\\
//######################################## Constructors  ########################################\\
//###############################################################################################\\

AMainManager::AMainManager()
{
	PrimaryActorTick.bCanEverTick = true;

	// Width = Y
	// Height = Z

	// Field initialization
	_FieldOfGeometry.SetNum(_WidthOfField);
	for (int32 i = 0; i < _WidthOfField; ++i) _FieldOfGeometry[i].SetNum(_HeightOfField);

	// Initializing the first material
	UMaterialInterface* FirstMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Script/Engine.MaterialInstanceConstant'/Game/Materials/MI_MainColor_Blue.MI_MainColor_Blue'"));
	if (FirstMaterial)
	{
		_ListOfMaterials.Add(FirstMaterial);
	}
	else
	{
		UE_LOG(LogMainManager, Display, TEXT("Cannot load Material. (MainManager.cpp | AMainManager::AMainManager)"))
	}

	// Choosing random figure
	_NextFigureType = RandomFigure();

}

//###############################################################################################\\
//###################################### Protected Methods ######################################\\
//###############################################################################################\\

void AMainManager::BeginPlay()
{
	/*
						// ******************************************************* \\
						|| ***** The game starting by AMainManagerController ***** ||
						\\ ******************************************************* //
	*/

	Super::BeginPlay();
	_World = GetWorld();
	
	// Choosing random material 
	_CurrentMaterial = _ListOfMaterials[_ListOfMaterials.Num() > 0 ? FMath::Rand() % _ListOfMaterials.Num() : 0];
	_NextMaterial =    _ListOfMaterials[_ListOfMaterials.Num() > 0 ? FMath::Rand() % _ListOfMaterials.Num() : 0];
}

//###############################################################################################\\
//####################################### Public Methods ########################################\\
//###############################################################################################\\

void AMainManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (_bIsGameActive)
	{
		_ResultData.GameTime = _World->GetTimeSeconds();
	}

}

void AMainManager::StartGame()
{
	if (_World && !_bIsGameActive)
	{
		_bIsGameActive = ACTIVATED;

		if (_MainGameplayWidget)
		{
			_MainGameplayWidget->AddToViewport();
		}

		// Choosing figure and adding to field
		SelectFigure(_FirstFigure, EFigureDirection::TOP);
		

		// Movement timer
		_World->GetTimerManager().SetTimer(_TileMovingTimer, this, &AMainManager::DropFigure, _DropTimerRate, true);
	}
	else if (_bIsGameActive)
	{
		UE_LOG(LogMainManager, Warning, TEXT("The game is already starrted"))
	}
	else
	{
		UE_LOG(LogMainManager, Error, TEXT("Unable to get access to the World. (MainManager.cpp | AMainManager::StartGame)"))
	}
}

void AMainManager::EndGame()
{
	_bIsGameActive = DEACTIVATED;
	_World->GetTimerManager().ClearTimer(_TileMovingTimer);

	if (_EndGameWidget)
	{
		_EndGameWidget->AddToViewport();
		FInputModeUIOnly Mode;
		_Controller->SetInputMode(Mode);
		_Controller->bShowMouseCursor = true;
	}
	else
	{
		UE_LOG(LogMainManager, Error, TEXT("Widget not found. (MainManager.cpp | AMainManager::EndGame)"));
	}
}

bool AMainManager::IsGameEnded()
{
	return !_bIsGameActive;
}

void AMainManager::UpdateFigure()
{
	float TileSize = ABaseGeometry::Size;
	for(int32 i = 0; i < _CurrentFigure.Num(); ++i)
	{
		FCoord YZ = _FigureCoords[i];
		FVector Location{ 
			0, 
			YZ.First * TileSize, 
			_SpawnCoordinates.Z - YZ.Second * TileSize 
		};
		_CurrentFigure[i]->SetActorLocation(Location);
	}
}

void AMainManager::AddFigureToField(const FCoord StartCoords)
{
	int32 Y = StartCoords.First;
	int32 Z = StartCoords.Second;
	
	for (auto& CoordsElem : _FigureCoords)
	{
		int32 DeltaY = CoordsElem.First;
		int32 DeltaZ = CoordsElem.Second;

		FActorSpawnParameters Param; Param.Owner = this;
		ABaseGeometry* SpawnedFigure = _World->SpawnActor<ABaseGeometry>(Param);

		SpawnedFigure->SetMainMaterial(_CurrentMaterial);
		SpawnedFigure->MainMesh->SetStaticMesh(_BaseGeometryMesh);

		_FieldOfGeometry[Y + DeltaY][Z + DeltaZ] = SpawnedFigure;
		_CurrentFigure.Add(SpawnedFigure);
	}

	// Setting the rotation axis of the figure
	int32 YPivot = _CurrFigurePivot.First;
	int32 ZPivot = _CurrFigurePivot.Second;
	_FieldOfGeometry[YPivot][ZPivot]->SetPivot(true);
}

void AMainManager::DropFigure()
{	
	InitPointer(0, 0, CLEAR);
	if (!IsIntersect(EFigureDirection::BOTTOM))
	{
		// Dropping pointers 
		InitPointer(0, 1, INITIALIZE);
	}
	else
	{
		// Increase points with each landing
		IncreaseScore(25);
		// Reseting cleared pointers 
		InitPointer(0, 0, INITIALIZE);

		CheckAndClearLine();

		_FigureCoords.Empty();
		_CurrentFigure.Empty();

		_CurrentFigureType = _NextFigureType;
		_NextFigureType = RandomFigure();

		int32 RandMaterialIndex = _ListOfMaterials.Num() > 0 ? FMath::Rand() % _ListOfMaterials.Num() : 0;
		_CurrentMaterial = _NextMaterial;
		_NextMaterial = _ListOfMaterials[RandMaterialIndex];

		SelectFigure(_CurrentFigureType, EFigureDirection::TOP);
	}

	UpdateFigure();
}

// Checking for border or figure crossings depending on direction
bool AMainManager::IsIntersect(EFigureDirection MovingDirection)
{
	int32 DeltaY = 0;
	int32 DeltaZ = 0;

	switch (MovingDirection)
	{
	case EFigureDirection::TOP:
		break;
	case EFigureDirection::RIGHT:
		DeltaY = 1;
		break;
	case EFigureDirection::LEFT:
		DeltaY = -1;
		break;
	case EFigureDirection::BOTTOM:
		DeltaZ = 1;
		break;
	default:
		UE_LOG(LogMainManager, Display, TEXT("Unexpected type of EFigureDirection. (MainManager.cpp | AMainManager::IsIntersect)"))
		return true;
	}

	for (auto& GeometryCoords : _FigureCoords)
	{
		FCoord ZY = GeometryCoords;
		int32 Y = ZY.First;
		int32 Z = ZY.Second;
		int32 SizeY = _FieldOfGeometry.Num();
		int32 SizeZ = _FieldOfGeometry[0].Num();
		bool ConditionY = 0 <= Y + DeltaY && Y + DeltaY < SizeY;
		bool ConditionZ = 0 <= Z + DeltaZ && Z + DeltaZ < SizeZ;

		if (!ConditionY || !ConditionZ || _FieldOfGeometry[Y + DeltaY][Z + DeltaZ]) return true;
	}

	return false;
}

// Getting coordinates of a figure by its type
bool AMainManager::CoordsFromType(EFigureType Type, TArray<FCoord>& NewFigure, FCoord& Pivot)
{
	bool flag = true;
	switch (Type)
	{
	case EFigureType::SQUARE:
		NewFigure.SetNum(4);
		NewFigure[0] = FCoord(0, 0);
		NewFigure[1] = Pivot = FCoord(0, 1);
		NewFigure[2] = FCoord(1, 0);
		NewFigure[3] = FCoord(1, 1);
		break;
	case EFigureType::UNDERLINE:
		NewFigure.SetNum(4);
		NewFigure[0] = FCoord(0, 0);
		NewFigure[1] = Pivot = FCoord(1, 0);
		NewFigure[2] = FCoord(2, 0);
		NewFigure[3] = FCoord(3, 0);
		break;
	case EFigureType::THREESIDE:
		NewFigure.SetNum(4);
		NewFigure[0] = FCoord(1, 0);
		NewFigure[1] = FCoord(0, 1);
		NewFigure[2] = Pivot = FCoord(1, 1);
		NewFigure[3] = FCoord(2, 1);
		break;
	case EFigureType::Z_TYPE:
		NewFigure.SetNum(4);
		NewFigure[0] = FCoord(0, 0);
		NewFigure[1] = Pivot = FCoord(1, 0);
		NewFigure[2] = FCoord(1, 1);
		NewFigure[3] = FCoord(2, 1);
		break;
	case EFigureType::Z_TYPE_M:
		NewFigure.SetNum(4);
		NewFigure[0] = FCoord(0, 1);
		NewFigure[1] = Pivot = FCoord(1, 0);
		NewFigure[2] = FCoord(1, 1);
		NewFigure[3] = FCoord(2, 0);
		break;
	case EFigureType::L_TYPE:
		NewFigure.SetNum(4);
		NewFigure[0] = FCoord(0, 0);
		NewFigure[1] = Pivot = FCoord(0, 1);
		NewFigure[2] = FCoord(1, 1);
		NewFigure[3] = FCoord(2, 1);
		break;
	case EFigureType::L_TYPE_M:
		NewFigure.SetNum(4);
		NewFigure[0] = FCoord(2, 0);
		NewFigure[1] = FCoord(0, 1);
		NewFigure[2] = FCoord(1, 1);
		NewFigure[3] = Pivot = FCoord(2, 1);
		break;
	default:
		UE_LOG(LogMainManager, Error, TEXT("Unexpected type of EFigureType. (MainManager.cpp | AMainManager::CoordsFromType)"))
		flag = false;
	}

	return flag;
}

// Figure setup before placing
void AMainManager::SelectFigure(const EFigureType Type, const EFigureDirection Direction)
{
	int32 ActualPosY = _SpawnCoordinates.Y / ABaseGeometry::Size;
	
	bool IsCreated = CoordsFromType(Type, _FigureCoords, _CurrFigurePivot);
	
	if (IsCreated)
	{
		_CurrFigurePivot.First += ActualPosY;

		for (auto& CoordElem : _FigureCoords)
		{
			CoordElem.First += ActualPosY;
			if (_FieldOfGeometry[CoordElem.First][CoordElem.Second])
			{
				EndGame();
				return;
			}
		}
	}
	else
	{
		UE_LOG(LogMainManager, Warning, TEXT("The Figure has not been created. (MainManager.cpp | AMainManager::SelectFigure)"))
	}

	FigureSelected.Broadcast();
	AddFigureToField();
	UpdateFigure();
}

void AMainManager::IncreaseScore(const int32 NumToIncrease)
{
	_ResultData.Score += NumToIncrease;	
	_ResultData.Coins = _ResultData.Score / 100;
}

EFigureType AMainManager::RandomFigure()
{
	int32 RandInt = FMath::Rand() % _FigureTypeNum;

	switch (RandInt)
	{
	case 0:
		return EFigureType::SQUARE;
	case 1:
		return EFigureType::UNDERLINE;
	case 2:
		return EFigureType::THREESIDE;
	case 3:
		return EFigureType::Z_TYPE;
	case 4:
		return EFigureType::Z_TYPE_M;
	case 5:
		return EFigureType::L_TYPE;
	case 6:
		return EFigureType::L_TYPE_M;
	default:
		UE_LOG(LogMainManager, Display, TEXT("Wrong type of EFigureType. (MainManager.cpp | AMainManager::RandomFigure)"))
		break;
	}

	UE_LOG(LogMainManager, Error, TEXT("Something go wrong. (MainManager.cpp | AMainManager::RandomFigure"))
	return EFigureType::SQUARE;
}

// ================================= Events ================================= //
void AMainManager::HandleMovementSideways(float Delta)
{
	if (!Delta) return;
	int32 Sgn = FMath::Sign(Delta);

	InitPointer(0, 0, CLEAR);
	if (Sgn > 0 ? !IsIntersect(EFigureDirection::RIGHT) : !IsIntersect(EFigureDirection::LEFT))
	{
		InitPointer(Sgn, 0, INITIALIZE);
	}

	UpdateFigure();
}

void AMainManager::HandleMovementBottom(float Delta)
{
	InitPointer(0, 0, CLEAR);

	if (!IsIntersect(EFigureDirection::BOTTOM))
	{
		InitPointer(0, Delta / FMath::Abs(Delta), INITIALIZE);
	}

	UpdateFigure();
}

void AMainManager::HandleMovementRotate()
{
	InitPointer(0, 0, CLEAR);
	CheckAndRotate();
	InitPointer(0, 0, INITIALIZE);
	UpdateFigure();
}

// ================================= Getters ================================= //
float AMainManager::GetDropTimerRate() const
{
	return _DropTimerRate;
}

float AMainManager::GetMovingDelta() const
{
	return _MovingDelta;
}

float AMainManager::GetDroppingDelta() const
{
	return _DroppingDelta;
}

FStatData AMainManager::GetResultData() const
{
	return _ResultData;
}

EFigureType AMainManager::GetNextFigureType() const
{
	return _NextFigureType; 
}

UMaterialInterface* AMainManager::GetNextMaterial() const
{
	return _NextMaterial;
}
// ================================= Setters ================================= //
void AMainManager::SetController(APlayerController* Controller)
{
	_Controller = Controller;
}

//###############################################################################################\\
//####################################### Private Methods #######################################\\
//###############################################################################################\\

void AMainManager::PrintFieldInLog() const
{
	UE_LOG(LogMainManager, Display, TEXT("============================="))
	for (int32 i = 0; i < _HeightOfField; ++i)
	{
		FString str = "";
		for (int32 j = 0; j < _WidthOfField; ++j)
		{
			int32 num = _FieldOfGeometry[j][i] ? 1 : 0;
			str += FString::FromInt(num);
			str += " ";
		}
		UE_LOG(LogMainManager, Display, TEXT("%s"), *str)
	}
	UE_LOG(LogMainManager, Display, TEXT("============================="))
}

// Initializaton/Deleting figure in figure grid with shifted by <Y, Z>
void AMainManager::InitPointer(int32 DeltaY, int32 DeltaZ, bool bIsVanish)
{
	for (int32 i = 0; i < _CurrentFigure.Num(); ++i)
	{
		FCoord ZY = _FigureCoords[i];
		int32 Y = ZY.First;
		int32 Z = ZY.Second;

		_FieldOfGeometry[Y + DeltaY][Z + DeltaZ] = bIsVanish ? nullptr : _CurrentFigure[i];
		_FigureCoords[i].First  += DeltaY;
		_FigureCoords[i].Second += DeltaZ;
	}
	_CurrFigurePivot = FCoord(_CurrFigurePivot.First + DeltaY, _CurrFigurePivot.Second + DeltaZ);
}

void AMainManager::CheckAndClearLine()
{
	int32 MinZ, MaxZ;
	MinZ = MaxZ = _FigureCoords[0].Second;

	for (auto& CoordsElem : _FigureCoords)
	{
		if (CoordsElem.Second > MaxZ) MaxZ = CoordsElem.Second;
		if (CoordsElem.Second < MinZ) MinZ = CoordsElem.Second;
	}

	for (int32 i = MinZ; i <= MaxZ; ++i)
	{
		bool Checker = true;
		for (int32 j = 0; j < _WidthOfField; ++j)
		{
			if (!_FieldOfGeometry[j][i])
			{
				Checker = false;
				break;
			}
		}
		
		if (Checker)
		{
			DropField(i);
		}

	}
}

void AMainManager::CheckAndRotate()
{
	for (auto& CoordElem : _FigureCoords)
	{
		FCoord Temp = RotateCoord(CoordElem, _CurrFigurePivot, _Degrees);

		bool Range = 0 <= Temp.First && Temp.First < _WidthOfField &&
				     0 <= Temp.Second && Temp.Second < _HeightOfField;

		if (!Range || _FieldOfGeometry[Temp.First][Temp.Second]) return;
	}
	TArray<FCoord> NewFigureCoords;
	for (int32 i = 0; i < _FigureCoords.Num(); ++i)
	{
		NewFigureCoords.Add(RotateCoord(_FigureCoords[i], _CurrFigurePivot, _Degrees));
	}

	_FigureCoords = NewFigureCoords;
}

void AMainManager::DropField(int32 Line)
{
	float TileSize = ABaseGeometry::Size;

	for (int32 i = 0; i < _WidthOfField; ++i)
	{
		_FieldOfGeometry[i][Line]->Destroy();
		_FieldOfGeometry[i][Line] = nullptr;

		for (int32 j = Line; j > 0; --j)
		{
			_FieldOfGeometry[i][j] = _FieldOfGeometry[i][j - 1];
			if(_FieldOfGeometry[i][j])
				_FieldOfGeometry[i][j]->SetActorLocation(FVector(0, i * TileSize, _SpawnCoordinates.Z - j * TileSize));
		}

		_FieldOfGeometry[i][0] = nullptr;
	}

	// Increase score to 100 after destroying line
	IncreaseScore(100);
}

//###############################################################################################\\
//######################################## Extras ###############################################\\
//###############################################################################################\\

// Rotates Coordinate C1 around C2 coordinate by Angle 
FCoord RotateCoord(FCoord C1, FCoord C2, float Angle)
{
	int32 ResCos = FMath::RoundToFloat(FMath::Cos(Angle));
	int32 ResSin = FMath::RoundToFloat(FMath::Sin(Angle));

	FCoord Tmp = FCoord();
	FCoord C3 = FCoord(C1.First - C2.First, C1.Second - C2.Second);
	
	// Rotation matrix
	Tmp.First  =  C3.First * ResCos + C3.Second * ResSin;
	Tmp.Second = -C3.First * ResSin + C3.Second * ResCos;

	return FCoord(Tmp.First + C2.First, Tmp.Second + C2.Second);
}