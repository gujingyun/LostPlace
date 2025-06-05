// Copyright Epic Games, Inc. All Rights Reserved.

#include "LostPlaceGameMode.h"
#include "LostPlaceCharacter.h"
#include "UObject/ConstructorHelpers.h"

ALostPlaceGameMode::ALostPlaceGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
