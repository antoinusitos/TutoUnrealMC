// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "TutoPlayer.generated.h"

UCLASS()
class TUTOMULTIJOUEURC_API ATutoPlayer : public ACharacter
{
	GENERATED_BODY()

public:

	//
	///
	/** FUNCTIONS **/
	///
	//

	// Sets default values for this character's properties
	ATutoPlayer();

	// Constructor for AFPSCharacter
	ATutoPlayer(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	// Handles the forward axis
	void Forward(float Value);

	// Handles the right axis
	void Right(float Value);

	// Handles the Fire on the client
	void Fire();

	// Handles the Fire on the server
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerFire();
	virtual void ServerFire_Implementation();
	virtual bool ServerFire_Validate();

	void MulticastDebug();

	// Handles the Fire on the server
	UFUNCTION(Reliable, NetMulticast, WithValidation)
	void ServerMulticastDebug();
	virtual void ServerMulticastDebug_Implementation();
	virtual bool ServerMulticastDebug_Validate();

	// Handles the Fire on the server
	void TakeHit(int32 Damage, ATutoPlayer* Causer);

	// Handles the Fire on the server (server side)
	UFUNCTION(Reliable, NetMulticast, WithValidation)
	void ServerTakeHit(int32 Damage, ATutoPlayer* Causer);
	virtual void ServerTakeHit_Implementation(int32 Damage, ATutoPlayer* Causer);
	virtual bool ServerTakeHit_Validate(int32 Damage, ATutoPlayer* Causer);

	//sets jump flag when key is pressed
	UFUNCTION()
	void OnStartJump();
	//clears jump flag when key is released
	UFUNCTION()
	void OnStopJump();

	// Show scores
	void ShowScores();

	// Stop Show scores
	void StopShowScores();

	// Reset Stats of the player
	void ResetStats();

	// Reset Stats of the player (server side)
	UFUNCTION(Reliable, NetMulticast, WithValidation)
	void ServerResetStats();
	virtual void ServerResetStats_Implementation();
	virtual bool ServerResetStats_Validate();

	// Refresh the HUD of scores
	UFUNCTION(BlueprintImplementableEvent)
	void RefreshScores();
	
	//
	///
	/** PROPERTIES **/
	///
	//

	// First person camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	UCameraComponent* FirstPersonCameraComponent;

	// Distance for the hit when fire
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float FireLength;

	// Health of the player 
	UPROPERTY(Replicated)
	int32 Health;

	// Damage of the player
	UPROPERTY(Replicated)
	int32 Damage;

	// Pawn mesh: 1st person view (arms; seen only by self)
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* FirstPersonMesh;

	// Showing score
	UPROPERTY(BlueprintReadOnly, Category = Scores)
	bool ShowingScore;
};
