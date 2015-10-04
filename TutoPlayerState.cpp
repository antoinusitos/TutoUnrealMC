// Fill out your copyright notice in the Description page of Project Settings.

#include "TutoMultijoueurC.h"
#include "TutoPlayerState.h"

void ATutoPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	TheDeath = 0;
	TheKills = 0;
	TheScore = 0;
	TheName = "Player";
}

void ATutoPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATutoPlayerState, TheDeath);
	DOREPLIFETIME(ATutoPlayerState, TheKills);
	DOREPLIFETIME(ATutoPlayerState, TheScore);
	DOREPLIFETIME(ATutoPlayerState, TheName);
}