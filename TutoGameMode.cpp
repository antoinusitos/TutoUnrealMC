// Fill out your copyright notice in the Description page of Project Settings.

#include "TutoMultijoueurC.h"
#include "TutoGameMode.h"


ATutoGameMode::ATutoGameMode(const FObjectInitializer& ObjectInitializer) :
Super(ObjectInitializer)
{
	DefaultPawnClass = ATutoPlayer::StaticClass();
}