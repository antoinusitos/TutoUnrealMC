// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerState.h"
#include "TutoPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class TUTOMULTIJOUEURC_API ATutoPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:

	virtual void PostInitializeComponents();
	
	UPROPERTY(BlueprintReadOnly, Replicated)
	int32 TheDeath;

	UPROPERTY(BlueprintReadOnly, Replicated)
	int32 TheKills;

	UPROPERTY(BlueprintReadOnly, Replicated)
	int32 TheScore;

	UPROPERTY(BlueprintReadOnly, Replicated)
	FString TheName;
};
