// Fill out your copyright notice in the Description page of Project Settings.

#include "SGameModeBase.h"
#include "TimerManager.h"

#include "../AI/STrackerBot.h"
#include "../Components/SHealthComponent.h"
#include "SGameStateBase.h"
#include "SPlayerState.h"
#include "../Player/SPlayerController.h"



ASGameModeBase::ASGameModeBase()
{
	TimeBetweenWaves = 2.f;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 2.f;

	GameStateClass = ASGameStateBase::StaticClass();
	PlayerStateClass = ASPlayerState::StaticClass();

	PlayerControllerClass = ASPlayerController::StaticClass();

}



void ASGameModeBase::StartPlay()
{
	Super::StartPlay();

	PrepareForNextWave();
	//UE_LOG(LogTemp, Warning, TEXT("GameMode StartPlay"));

}

void ASGameModeBase::PostLogin(APlayerController * NewPlayer)
{
	Super::PostLogin(NewPlayer);
	FillPlayerControllerArray(NewPlayer);
}

void ASGameModeBase::FillPlayerControllerArray(APlayerController* NewPlayer)
{

	ASPlayerController *PC = Cast<ASPlayerController>(NewPlayer);
	if (PC)
		PlayerControllers.AddUnique(PC);

	/*for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
	{

		UE_LOG(LogTemp, Warning, TEXT("Full Array"));
		ASPlayerController *PC = Cast<ASPlayerController>(It->Get());
		if (PC)
			PlayerControllers.AddUnique(PC);
	}*/

	UE_LOG(LogTemp, Warning, TEXT("Array Num = %i"), PlayerControllers.Num());
}


void ASGameModeBase::Tick(float DeltaSecondes)
{
	Super::Tick(DeltaSecondes);

	CheckWaveSate();
	CheckAllPlayerLive();
	UE_LOG(LogTemp, Error, TEXT("Tick = %f"), DeltaSecondes);
}


void ASGameModeBase::StartWave()
{
	WaveCount = FMath::RandRange(1, 3);
	NrOfBotsToSpawn = 2 * WaveCount;
	//UE_LOG(LogTemp, Warning, TEXT("StartWave Num = %i"), NrOfBotsToSpawn);
	GetWorldTimerManager().SetTimer(TimerHandle_BotSpawner, this, &ASGameModeBase::SpawnBotTimerElapsed, 1.f, true, 0.f);

	SetWaveState(EWaveState::WaveInProgress);
}


void ASGameModeBase::SpawnBotTimerElapsed()
{
	SpawnNewBot();

	NrOfBotsToSpawn--;

	if (NrOfBotsToSpawn <= 0)
		EndWave();
}

void ASGameModeBase::EndWave()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_BotSpawner);
	GetWorldTimerManager().ClearTimer(TimerHandle_NextWavesStart);

	SetWaveState(EWaveState::WaitingToComplete);

}


void ASGameModeBase::PrepareForNextWave()
{
	GetWorldTimerManager().SetTimer(TimerHandle_NextWavesStart, this, &ASGameModeBase::StartWave, TimeBetweenWaves, false, 0.f);
	SetWaveState(EWaveState::WaitingToStart);

}

void ASGameModeBase::CheckWaveSate()
{
	if (NrOfBotsToSpawn > 0 || GetWorldTimerManager().IsTimerActive(TimerHandle_NextWavesStart))
		return;

	bool bIsAnyBotAlive = false;

	for (FConstPawnIterator it = GetWorld()->GetPawnIterator(); it; it++) // Or   for(TActorIterator<APawn> Itr(GetWorld()); Itr; ++Itr)
	{
		APawn* Testpawn = it->Get();
		if(Testpawn == nullptr || Testpawn->IsPlayerControlled())
			continue;

		Testpawn = Cast<ASTrackerBot>(it->Get());
		if (Testpawn)
		{
			auto HC = Cast<USHealthComponent>(Testpawn->GetComponentByClass(USHealthComponent::StaticClass()));
			if (HC && HC->GetHealth() > 0.f)
			{
				bIsAnyBotAlive = true;
				break;
			}

		}
	}

	UE_LOG(LogTemp, Error, TEXT("BotAlive = %i"), bIsAnyBotAlive);

	if (!bIsAnyBotAlive)
	{
		SetWaveState(EWaveState::WaveComplete);
		PrepareForNextWave();
	}

}

void ASGameModeBase::ResurrectionPlayer(ASPlayerController * PC)
{
	if (!PC) return;

	PC->PlayerLive--;

	/// PC->GetPawn != NULL
	if (PC->IsPlayerAlive())
	{
		FTimerHandle TimerHandle_PlayerRespon;
		FTimerDelegate TimerDel;
		TimerDel.BindUFunction(this, FName("RestartDeadPlayers"), PC);
		GetWorldTimerManager().SetTimer(TimerHandle_PlayerRespon, TimerDel, 5.f, false, 5.f);
		UE_LOG(LogTemp, Error, TEXT("ResurrectionPlayer PlayerLive = %i"), PC->PlayerLive);
	}

	//PC->PlayerLive--;

}

void ASGameModeBase::RestartDeadPlayers(ASPlayerController* PC)
{
	RestartPlayer(PC);
	//GetWorldTimerManager().ClearTimer(TimerHandle_PlayerRespon);
	UE_LOG(LogTemp, Warning, TEXT("CheckAnyPlayerActive == delete Timer PlayerRespon PlayerLive = %i"), PC->PlayerLive);


}


void ASGameModeBase::CheckAllPlayerLive()
{
	bool bPlayerAlive = false;
	for(auto PC : PlayerControllers) // for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
	{
		UE_LOG(LogTemp, Warning, TEXT("CheckAnyPlayerActive == ForLoop start"));
		//ASPlayerController *PC = Cast<ASPlayerController>(It->Get());
		if (PC && PC->IsPlayerAlive())
		{
			bPlayerAlive = true;
		}
	}

	/// No Player Alive
	if (!bPlayerAlive)
		GameOVer();
}



void ASGameModeBase::GameOVer()
{
	EndWave();
	for (ASPlayerController* PController : PlayerControllers)
		PController->ConsoleCommand("Quit");
	/// TODO Finish up the Match, Present 'Game over' ot Player
	SetWaveState(EWaveState::CameOver);
	UE_LOG(LogTemp, Warning, TEXT("GAME OVER!!! Players are Dead"));
}



void ASGameModeBase::SetWaveState(EWaveState NewState)
{
	ASGameStateBase *GS = GetGameState<ASGameStateBase>();
	if (ensureAlways(GS))
	{
		GS->SetWaveState(NewState);
	}
}



/// Sam's Function No More Need
//void ASGameModeBase::CheckAnyPlayerActive()
//{
//	//My Code
//	bool bNoPlayerAlive = true;
//	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("CheckAnyPlayerActive == ForLoop start"));
//		ASPlayerController *PC = Cast<ASPlayerController>(It->Get());
//		if (PC && PC->GetPawn() == nullptr && PC->PlayerLive != 0)
//		{
//			/// My code
//			/*UE_LOG(LogTemp, Warning, TEXT("CheckAnyPlayerActive == PlayerControllr Part, PlayrLive = %i"), PC->PlayerLive);
//			PC->PlayerLive--;
//			bNoPlayerAlive = false;
//			TimerDel.BindUFunction(this, FName("RestartDeadPlayers"), PC);
//			GetWorldTimerManager().SetTimer(TimerHandle_PlayerRespon, TimerDel, 0.f, false, -5.f);*/
//
//			/// Sam's Code
//			/*APawn *MyPawn = PC->GetPawn();
//			USHealthComponent *HealComp = Cast<USHealthComponent>(MyPawn->GetComponentByClass(USHealthComponent::StaticClass()));
//			if (ensure(HealComp) && HealComp->GetHealth() > 0.f)
//			{
//				/// A Player Still Alive
//				return;
//			}*/
//		}
//	}
//	
//  // Sam's Code
//  GameOver();
//
//
//	// My Code
//	/// No Player Alive
//	if (!bNoPlayerAlive)
//		GameOVer();
//
//}