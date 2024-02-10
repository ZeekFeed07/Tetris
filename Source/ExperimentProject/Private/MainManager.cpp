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

	_FieldOfGeometry.SetNum(_WidthOfField);

	for (int32 i = 0; i < _WidthOfField; ++i) _FieldOfGeometry[i].SetNum(_HeightOfField);

	// Инициализация первого материала
	UMaterialInterface* MainMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Script/Engine.MaterialInstanceConstant'/Game/Materials/MI_BaseGeometry_Blue.MI_BaseGeometry_Blue'"));
	if (MainMaterial)
	{
		_ListOfMaterials.Add(MainMaterial);
	}
	else
	{
		UE_LOG(LogMainManager, Display, TEXT("Cannot load Material. (MainManager.cpp | AMainManager::AMainManager)"))
	}
}

//###############################################################################################\\
//###################################### Protected Methods ######################################\\
//###############################################################################################\\

void AMainManager::BeginPlay()
{
	Super::BeginPlay();

	_World = GetWorld();
	FString PlayerName = "Zeek_Feed";

	if (_World)
	{ 
		StartGame(PlayerName);
	}
	else
	{
		UE_LOG(LogMainManager, Error, TEXT("Unable to get access to the World. (MainManager.cpp | AMainManager::BeginPlay)"))
	}
}

//###############################################################################################\\
//####################################### Public Methods ########################################\\
//###############################################################################################\\

void AMainManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (_bIsGameActive)
	{
		//float Seconds = UGameplayStatics::GetRealTimeSeconds(_World);
		//GEngine->AddOnScreenDebugMessage(1, _TimerRate, FColor::Orange, FString::Printf(TEXT("Time: %d"), _ResultData.Score), true, FVector2D(2.0f, 2.0f));
		//float Seconds = _World->GetTime().GetWorldTimeSeconds();
		//GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::Orange, FString::Printf(TEXT("Time: %d"), Seconds), true, FVector2D(2.0f, 2.0f));
	}
}

// Функция запуска игры
void AMainManager::StartGame(const FString PlayerName)
{
	_bIsGameActive = ACTIVATED;

	if (_World)
	{
		// Создание фигуры и имплементация её движения
		SelectFigure(_FirstFigure, EFigureDirection::TOP);

		// Тайминг движения
		_World->GetTimerManager().SetTimer(_TileMovingTimer, this, &AMainManager::DropFigure, _TimerRate, true);
		_World->GetTimerManager().SetTimer(_TimeCountingTimer, this, &AMainManager::IncreaseTimer, 1.0f, true);
	}
	else
	{
		UE_LOG(LogMainManager, Error, TEXT("Unable to get access to the World. (MainManager.cpp | AMainManager::StartGame)"))
	}
}

// Функция окончания игры
void AMainManager::EndGame()
{
	_bIsGameActive = DEACTIVATED;
	_World->GetTimerManager().ClearTimer(_TileMovingTimer);
	_World->GetTimerManager().ClearTimer(_TimeCountingTimer);

	_ResultData.GameTime = _Seconds;
	_Seconds = 0;	

	if (_EndGameWidget)
	{
		_EndGameWidget->AddToViewport();
		FInputModeUIOnly Mode;
		//Mode.SetWidgetToFocus(_EndGameWidget->GetCachedWidget()); // Вызывает ошибку что нельзя сфокусироваться на данном виджете
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
		Coord YZ = _FigureCoords[i];
		FVector Location{ 
			0, 
			YZ.First * TileSize, 
			_SpawnCoordinates.Z - YZ.Second * TileSize 
		};
		_CurrentFigure[i]->SetActorLocation(Location);
	}
}

void AMainManager::AddFigureToField(const Coord StartCoords)
{
	int32 Y = StartCoords.First;
	int32 Z = StartCoords.Second;

	int32 RandElem = FMath::Rand() % _ListOfMaterials.Num();
	
	for (auto& CoordsElem : _FigureCoords)
	{
		int32 DeltaY = CoordsElem.First;
		int32 DeltaZ = CoordsElem.Second;

		FActorSpawnParameters Param; Param.Owner = this;
		ABaseGeometry* SpawnedFigure = _World->SpawnActor<ABaseGeometry>(Param);
		SpawnedFigure->SetMainMaterial(_ListOfMaterials[RandElem]);

		_FieldOfGeometry[Y + DeltaY][Z + DeltaZ] = SpawnedFigure;
		_CurrentFigure.Add(SpawnedFigure);
	}

	// Установка оси вращения фигуры
	int32 YPivot = _CurrFigurePivot.First;
	int32 ZPivot = _CurrFigurePivot.Second;
	_FieldOfGeometry[YPivot][ZPivot]->SetPivot(true);

	UpdateFigure();
}

void AMainManager::DropFigure()
{
	//float Seconds = _World->GetTime().GetWorldTimeSeconds();
	GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::Orange, FString::Printf(TEXT("Time: %i"), _Seconds), true, FVector2D(2.0f, 2.0f));

	InitMovement(0, 0, CLEAR);
	if (!IsIntersect(EFigureDirection::BOTTOM))
	{
		// Добавление новых блоков для последующей прорисовки
		InitMovement(0, 1, INITIALIZE);
	}
	else
	{
		// Возвращение указателей после зануления при приземлении
		InitMovement(0, 0, INITIALIZE);

		CheckAndClearLine();

		_FigureCoords.Empty();
		_CurrentFigure.Empty();

		if ((_bIsGameActive = !IsGameEnded()) != 0)
		{
			SelectFigure(RandomFigure(), EFigureDirection::TOP);
		}
		else // TODO: никогда не происходит
		{
			UE_LOG(LogMainManager, Display, TEXT("The End"))
		}

	}
	UpdateFigure();
}

// Проверка на пересечение с границей или объектом в зависимости от направления
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
		Coord ZY = GeometryCoords;
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

void AMainManager::SelectFigure(const EFigureType Type, const EFigureDirection Direction)
{
	int32 ActualPosY = _SpawnCoordinates.Y / ABaseGeometry::Size;
	// Создание силуета фигуры
	switch (Type)
	{
	case EFigureType::SQUARE:
		_FigureCoords.SetNum(4);
		_FigureCoords[0] = Coord(0, 0);
		_FigureCoords[1] = _CurrFigurePivot = Coord(0, 1);
		_FigureCoords[2] = Coord(1, 0);
		_FigureCoords[3] = Coord(1, 1);
		break;
	case EFigureType::UNDERLINE:
		_FigureCoords.SetNum(4);
		_FigureCoords[0] = Coord(0, 0);
		_FigureCoords[1] = _CurrFigurePivot = Coord(1, 0);
		_FigureCoords[2] = Coord(2, 0);
		_FigureCoords[3] = Coord(3, 0);
		break;
	case EFigureType::THREESIDE:
		_FigureCoords.SetNum(4);
		_FigureCoords[0] = Coord(1, 0);
		_FigureCoords[1] = Coord(0, 1);
		_FigureCoords[2] = _CurrFigurePivot = Coord(1, 1);
		_FigureCoords[3] = Coord(2, 1);
		break;
	case EFigureType::Z_TYPE:
		_FigureCoords.SetNum(4);
		_FigureCoords[0] = Coord(0, 0);
		_FigureCoords[1] = _CurrFigurePivot = Coord(1, 0);
		_FigureCoords[2] = Coord(1, 1);
		_FigureCoords[3] = Coord(2, 1);
		break;
	case EFigureType::Z_TYPE_M:
		_FigureCoords.SetNum(4);
		_FigureCoords[0] = Coord(0, 1);
		_FigureCoords[1] = _CurrFigurePivot = Coord(1, 0);
		_FigureCoords[2] = Coord(1, 1);
		_FigureCoords[3] = Coord(2, 0);
		break;
	case EFigureType::L_TYPE:
		_FigureCoords.SetNum(4);
		_FigureCoords[0] = Coord(0, 0);
		_FigureCoords[1] = _CurrFigurePivot = Coord(0, 1);
		_FigureCoords[2] = Coord(1, 1);
		_FigureCoords[3] = Coord(2, 1);
		break;
	case EFigureType::L_TYPE_M:
		_FigureCoords.SetNum(4);
		_FigureCoords[0] = Coord(2, 0);
		_FigureCoords[1] = Coord(0, 1);
		_FigureCoords[2] = Coord(1, 1);
		_FigureCoords[3] = _CurrFigurePivot = Coord(2, 1);
		break;
	default:
		UE_LOG(LogMainManager, Error, TEXT("Unexpected type of EFigureType. (MainManager.cpp | AMainManager::SelectFigure)"))
		return;
	}

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

	AddFigureToField();
}

void AMainManager::IncreaseScore(const int32 NumToIncrease)
{
	_ResultData.Score += NumToIncrease;
}

EFigureType AMainManager::RandomFigure()
{
	int32 RandInt = FMath::Rand() % _FigureTypeNum;

	switch (RandInt)
	{
	case 0:
		return EFigureType::SQUARE;
		break;
	case 1:
		return EFigureType::UNDERLINE;
		break;
	case 2:
		return EFigureType::THREESIDE;
		break;
	case 3:
		return EFigureType::Z_TYPE;
		break;
	case 4:
		return EFigureType::Z_TYPE_M;
		break;
	case 5:
		return EFigureType::L_TYPE;
		break;
	case 6:
		return EFigureType::L_TYPE_M;
		break;
	default:
		UE_LOG(LogMainManager, Display, TEXT("Wrong type of EFigureType. (MainManager.cpp | AMainManager::RandomFigure)"))
		break;
	}

	UE_LOG(LogMainManager, Error, TEXT("Something go wrong. (MainManager.cpp | AMainManager::RandomFigure"))
	return EFigureType::SQUARE;
}

// =================================События нажатия кнопок ================================= //
void AMainManager::HandleMovementSideways(float Delta)
{
	if (!Delta) return;
	int32 Sgn = FMath::Sign(Delta);

	InitMovement(0, 0, CLEAR);
	if (Sgn > 0 ? !IsIntersect(EFigureDirection::RIGHT) : !IsIntersect(EFigureDirection::LEFT))
	{
		InitMovement(Sgn, 0, INITIALIZE);
	}

	UpdateFigure();
}
	
void AMainManager::HandleMovementFrontBack(float Delta)
{
	if (!Delta) return;
	int32 Sgn = FMath::Sign(Delta);

	InitMovement(0, 0, CLEAR);
	if (Sgn > 0 && !IsIntersect(EFigureDirection::BOTTOM))
	{
		InitMovement(0, Sgn, INITIALIZE);
	}
	else if(Sgn < 0)
	{
		CheckAndRotate();
		InitMovement(0, 0, INITIALIZE);
	}

	UpdateFigure();
}

// ================================= Getters ================================= //

float AMainManager::GetTimerRate() const
{
	return _TimerRate;
}

FStatData AMainManager::GetResultData() const
{
	return _ResultData;
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
	//for (int32 i = _HeightOfField - 1; i >= 0; --i)
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

void AMainManager::InitMovement(int32 DeltaY, int32 DeltaZ, bool bIsVanish)
{
	for (int32 i = 0; i < _CurrentFigure.Num(); ++i)
	{
		Coord ZY = _FigureCoords[i];
		int32 Y = ZY.First;
		int32 Z = ZY.Second;

		_FieldOfGeometry[Y + DeltaY][Z + DeltaZ] = bIsVanish ? nullptr : _CurrentFigure[i];
		_FigureCoords[i].First  += DeltaY;
		_FigureCoords[i].Second += DeltaZ;
	}
	_CurrFigurePivot = Coord(_CurrFigurePivot.First + DeltaY, _CurrFigurePivot.Second + DeltaZ);
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
		Coord Temp = RotateCoord(CoordElem, _CurrFigurePivot, _Degrees);

		bool Range = 0 <= Temp.First && Temp.First < _WidthOfField &&
				     0 <= Temp.Second && Temp.Second < _HeightOfField;

		if (!Range || _FieldOfGeometry[Temp.First][Temp.Second]) return;
	}
	TArray<Coord> NewFigureCoords;
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
}

void AMainManager::IncreaseTimer()
{
	++_Seconds;
}

//###############################################################################################\\
//######################################## Extras ###############################################\\
//###############################################################################################\\

Coord RotateCoord(Coord C1, Coord C2, float Angle)
{
	int32 ResCos = FMath::RoundToFloat(FMath::Cos(Angle));
	int32 ResSin = FMath::RoundToFloat(FMath::Sin(Angle));

	Coord Tmp = Coord();
	Coord C3 = Coord(C1.First - C2.First, C1.Second - C2.Second);
	
	// Матрица вращения
	Tmp.First  =  C3.First * ResCos + C3.Second * ResSin;
	Tmp.Second = -C3.First * ResSin + C3.Second * ResCos;

	return Coord(Tmp.First + C2.First, Tmp.Second + C2.Second);
}