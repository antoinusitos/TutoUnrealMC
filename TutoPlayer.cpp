// Fill out your copyright notice in the Description page of Project Settings.

#include "TutoMultijoueurC.h"
#include "TutoPlayer.h"
#include "DrawDebugHelpers.h"


// Sets default values
ATutoPlayer::ATutoPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set this actor to be replicate.
	bReplicates = true;

	FireLength = 1000;
	Health = 100;
	Damage = 10;
}

ATutoPlayer::ATutoPlayer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Create a CameraComponent 
	FirstPersonCameraComponent = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->AttachParent = GetCapsuleComponent();
	// Position the camera a bit above the eyes
	FirstPersonCameraComponent->RelativeLocation = FVector(0, 0, BaseEyeHeight);
	// Allow the pawn to control rotation.
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	FirstPersonMesh = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("FirstPersonMesh"));
	FirstPersonMesh->SetOnlyOwnerSee(true);         // only the owning player will see this mesh
	FirstPersonMesh->AttachParent = FirstPersonCameraComponent;
	FirstPersonMesh->bCastDynamicShadow = false;
	FirstPersonMesh->CastShadow = false;

	// everyone but the owner can see the regular body mesh
	GetMesh()->SetOwnerNoSee(true);

}

void ATutoPlayer::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);


	DOREPLIFETIME(ATutoPlayer, Health);
	DOREPLIFETIME(ATutoPlayer, Damage);
}

// Called when the game starts or when spawned
void ATutoPlayer::BeginPlay()
{
	Super::BeginPlay();

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("TEST MULTIPLAYER"));
	}
	
}

// Called every frame
void ATutoPlayer::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

// Called to bind functionality to input
void ATutoPlayer::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	InputComponent->BindAxis("Forward", this, &ATutoPlayer::Forward);
	InputComponent->BindAxis("Right", this, &ATutoPlayer::Right);
	InputComponent->BindAxis("LookUp", this, &ATutoPlayer::AddControllerPitchInput);
	InputComponent->BindAxis("LookAround", this, &ATutoPlayer::AddControllerYawInput);

	InputComponent->BindAction("Fire", IE_Pressed, this, &ATutoPlayer::Fire);
	InputComponent->BindAction("Jump", IE_Pressed, this, &ATutoPlayer::OnStartJump);
	InputComponent->BindAction("Jump", IE_Released, this, &ATutoPlayer::OnStopJump);
	InputComponent->BindAction("ShowScore", IE_Pressed, this, &ATutoPlayer::ShowScores);
	InputComponent->BindAction("ShowScore", IE_Released, this, &ATutoPlayer::StopShowScores);

}

void ATutoPlayer::Forward(float Value)
{
	if (Controller != NULL && Value != 0.0f)
	{
		FRotator Rotation = Controller->GetControlRotation();
		if (GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling()) 
		{ 
			Rotation.Pitch = 0.0f; 
		}
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ATutoPlayer::Right(float Value)
{
	if (Controller != NULL && Value != 0.0f)
	{
		FRotator Rotation = Controller->GetControlRotation();
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void ATutoPlayer::OnStartJump()
{
	bPressedJump = true;
}
void ATutoPlayer::OnStopJump()
{
	bPressedJump = false;
}


void ATutoPlayer::Fire()
{
	if (Role < ROLE_Authority)
	{
		ServerFire();
	}
}

bool ATutoPlayer::ServerFire_Validate()
{
	return true;
}

void ATutoPlayer::ServerFire_Implementation()
{	
	//location the PC is focused on
	const FVector Start = FirstPersonCameraComponent->GetComponentLocation();

	//1000 units in facing direction of PC (1000 units in front of the camera)
	const FVector End = Start + (FirstPersonCameraComponent->GetForwardVector() * FireLength);

	FHitResult HitInfo;
	FCollisionQueryParams QParams;
	ECollisionChannel Channel = ECollisionChannel::ECC_Visibility;
	FCollisionQueryParams OParams = FCollisionQueryParams::DefaultQueryParam;

	if (GetWorld()->LineTraceSingleByChannel(HitInfo, Start, End, ECollisionChannel::ECC_Visibility))
	{
		auto MyPC = Cast<ATutoPlayer>(HitInfo.GetActor());
		if (MyPC) {
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("" + PlayerState->PlayerName + " hits " + MyPC->PlayerState->PlayerName));
			MulticastDebug();
			MyPC->TakeHit(Damage, this);
		}
	}
}

void ATutoPlayer::MulticastDebug()
{	
	if (Role == ROLE_Authority)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Hit SomeOne !"));
		}
	}
}

void ATutoPlayer::ServerMulticastDebug_Implementation()
{
	MulticastDebug();
}

bool ATutoPlayer::ServerMulticastDebug_Validate()
{
	return true;
}

void ATutoPlayer::TakeHit(int32 Damage, ATutoPlayer* Causer)
{
	Health -= Damage;

	if (Role < ROLE_Authority)
	{
		ServerTakeHit(Damage, Causer);
	}
	else if (Role == ROLE_Authority)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("" + PlayerState->PlayerName + " life : " + FString::FromInt(Health)));
		}
		if (Health <= 0)
		{
			auto MyPS = Cast<ATutoPlayerState>(Causer->PlayerState);
			if (MyPS)
			{
				MyPS->TheKills += 1;
				MyPS->TheScore += 100;
			}
			auto ThePS = Cast<ATutoPlayerState>(PlayerState);
			if (ThePS)
			{
				ThePS->TheDeath += 1;
			}
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Scores Updated !"));
			}
			
			TArray<APlayerStart*> Tab;
			//find all PlayerStart
			for (TActorIterator<APlayerStart> ActorItr(GetWorld()); ActorItr; ++ActorItr)
			{
				Tab.Add(*ActorItr);
			}
			int32 random = FMath::RandRange(0, Tab.Num()-1);
			SetActorLocation(Tab[random]->GetActorLocation());
			SetActorRotation(Tab[random]->GetActorRotation());
			ResetStats();
		}
	}
}

void ATutoPlayer::ServerTakeHit_Implementation(int32 Damage, ATutoPlayer* Causer)
{
	TakeHit(Damage, Causer);
}

bool ATutoPlayer::ServerTakeHit_Validate(int32 Damage, ATutoPlayer* Causer)
{
	return true;
}

void ATutoPlayer::ShowScores()
{
	ShowingScore = true;
	RefreshScores();
}


void ATutoPlayer::StopShowScores()
{
	ShowingScore = false;
}

void ATutoPlayer::ResetStats()
{
	if (Role == ROLE_Authority)
	{
		Health = 100;
	}
	else if (Role < ROLE_Authority)
	{
		ServerResetStats();
	}
}

void ATutoPlayer::ServerResetStats_Implementation()
{
	ResetStats();
}

bool ATutoPlayer::ServerResetStats_Validate()
{
	return true;
}