// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "DSCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DSTargetComponent.h"
#include "DSLockArmComponent.h"

//////////////////////////////////////////////////////////////////////////
// ADSCharacter
//READ THIS FIRST
//https://docs.unrealengine.com/en-US/Gameplay/Framework/Pawn/Character

//TODO 11: Initialize the ADSCharacter
ADSCharacter::ADSCharacter()
{
    //SET class values, play around with these after everything is workin
	LockonControlRotationRate = 10.f;
	TargetSwitchMouseDelta = 3.f;
	TargetSwitchAnalogValue = .8f;
	TargetSwitchMinDelaySeconds = .5f;
	BreakLockMouseDelta = 10.f;
	BrokeLockAimingCooldown = .5f;

	// Set size for collision capsule (This is a comment)
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera. (This is a comment)
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement (This is a comment)
    //Call GetCharacterMovement() and set the bOrientRotationToMovement to true on the UCharacterMovementComponent component that this ACharacter has
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
    //Call GetCharacterMovement() and set the RotationRate to FRotator(0.0f, 540.0f, 0.0f)
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
    //Call GetCharacterMovement() and set the JumpZVelocity to 600.f
	GetCharacterMovement()->JumpZVelocity = 600.f;
    //Call GetCharacterMovement() and set the AirControl to 0.2f
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision) (This is a comment)
    //CREATE the CameraLockArm Component
	CameraLockArm = CreateDefaultSubobject<UDSLockArmComponent>(TEXT("CameraLockArm"));
    //ATTACH the CameraLockArm to the RootComponent
	CameraLockArm->SetupAttachment(RootComponent);
    //SET the CameraLockArm's Relative Location to FVector(0.f, 0.f, 50.f)
	CameraLockArm->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
	
	// Create a follow camera (This is a comment)
    //CREATE the FollowCamera Component
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
    //ATTACH the FollowCamera to the CameraLockArm and this time, pass this USpringArmComponent::SocketName in as a second argument to the SetupAttachment() function, ALT+G into USpringArmComponent::SocketName and see what it is
	FollowCamera->SetupAttachment(CameraLockArm, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
    // Camera does not rotate relative to arm (This is a comment)
    //SET the FollowCamera's  bUsePawnControlRotation variable to false
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create target component (This is a comment)
    //CREATE the TargetComponent
	TargetComponent = CreateDefaultSubobject<UDSTargetComponent>(TEXT("TargetComponent"));
    //ATTACH the TargetComponent to the RootComponent
	TargetComponent->SetupAttachment(GetRootComponent());
}

//////////////////////////////////////////////////////////////////////////
//TODO 0: Study the Input
void ADSCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings

	/* Assert PlayerInputComponent exists */
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ADSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ADSCharacter::MoveRight);

	// Handle mouse aiming input
	PlayerInputComponent->BindAxis("Turn", this, &ADSCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ADSCharacter::LookUp);

	// Handle analog stick aiming input
	PlayerInputComponent->BindAxis("TurnRate", this, &ADSCharacter::TurnAtRate);	
	PlayerInputComponent->BindAxis("LookUpRate", this, &ADSCharacter::LookUpAtRate);

	// Action inputs
	PlayerInputComponent->BindAction("ToggleCameraLock", IE_Pressed, CameraLockArm, &UDSLockArmComponent::ToggleCameraLock);
	PlayerInputComponent->BindAction("ToggleSoftLock", IE_Pressed, CameraLockArm, &UDSLockArmComponent::ToggleSoftLock);
}

//TODO 12: Implement Forward movement
void ADSCharacter::MoveForward(float Val)
{
    //IF the Controller is NOT null AND Val is NOT null 
	if ((Controller != nullptr) && (Val != 0.0f))
	{
		// find out which way is forward (This is a comment)
        //DECLARE a FRotator called Rotation and using a Ternary operator, Check if CameraLockArm->CameraTarget IS nullptr, IF true CALL Controller->GetControlRotation() otherwise CALL (CameraLockArm->CameraTarget->GetOwner()->GetActorLocation() - GetActorLocation()).GetSafeNormal().Rotation()
		const FRotator Rotation = CameraLockArm->CameraTarget == nullptr ? Controller->GetControlRotation() : (CameraLockArm->CameraTarget->GetOwner()->GetActorLocation() - GetActorLocation()).GetSafeNormal().Rotation();
        //DECLARE a FRotator named YawRotation and set its YAW to Rotation.Yaw
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector (This is a comment)
        //DECLARE a FVector called Direction and SET it to the return value of FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X), Getting its Direction on the Forward (Unreal X)
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        //CALL AddMovementInput passing in the DIrection and Val as the Scale Value
		AddMovementInput(Direction, Val);
	}
    //ENDIF
}

//TODO 13: Implement Strafing
void ADSCharacter::MoveRight(float Val)
{
    //IF the Controller is NOT null AND Val is NOT null 
	if ((Controller != NULL) && (Val != 0.0f))
	{
		// find out which way is right (This is a comment)
         //DECLARE a FRotator called Rotation and using a Ternary operator, Check if CameraLockArm->CameraTarget IS nullptr, IF true CALL Controller->GetControlRotation() otherwise CALL (CameraLockArm->CameraTarget->GetOwner()->GetActorLocation() - GetActorLocation()).GetSafeNormal().Rotation()
		const FRotator Rotation = CameraLockArm->CameraTarget == nullptr ? Controller->GetControlRotation() : (CameraLockArm->CameraTarget->GetOwner()->GetActorLocation() - GetActorLocation()).GetSafeNormal().Rotation();
        //DECLARE a FRotator named YawRotation and set its YAW to Rotation.Yaw
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector (This is a comment)
         //DECLARE a FVector called Direction and SET it to the return value of FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X), Getting its Direction on the Right (Unreal Y)
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
        //CALL AddMovementInput passing in the DIrection and Val as the Scale Value
		AddMovementInput(Direction, Val);
	}
    //ENDIF
}

//TODO 14: Implement Turning (Mouse Input)
void ADSCharacter::Turn(float Val)
{
    //DECLARE a float called TimeSinceLastTargetSwitch and assign it to the return value of GetWorld()->GetRealTimeSeconds() - LastTargetSwitchTime
	float TimeSinceLastTargetSwitch = GetWorld()->GetRealTimeSeconds() - LastTargetSwitchTime;

    //IF CameraLockArm->IsCameraLockedToTarget()
	if (CameraLockArm->IsCameraLockedToTarget())
	{
		// Should break soft-lock? (This is a comment)
        //IF CameraLockArm->bUseSoftLock is true AND FMath::Abs(Val) is GREATER THAN BreakLockMouseDelta
		if (CameraLockArm->bUseSoftLock && FMath::Abs(Val) > BreakLockMouseDelta)
		{
            //CALL CameraLockArm->BreakTargetLock()
			CameraLockArm->BreakTargetLock();
            //SET BrokeLockTime to the return of GetWorld()->GetRealTimeSeconds()
			BrokeLockTime = GetWorld()->GetRealTimeSeconds();
            //SET CameraLockArm->bSoftlockRequiresReset to true
			CameraLockArm->bSoftlockRequiresReset = true;
		}
        // Should try switch target? (This is a comment)
        //ELSE IF FMath::Abs(Val) Is GREATER THAN TargetSwitchMouseDelta AND TimeSinceLastTargetSwitch Is GREATER THAN TargetSwitchMinDelaySeconds		
		else if(FMath::Abs(Val) > TargetSwitchMouseDelta 
			&& TimeSinceLastTargetSwitch > TargetSwitchMinDelaySeconds)	// Prevent switching multiple times using a single movement
		{
            //IF Val is LESS than 0 (Ternary ??)
			if (Val < 0)
                //CALL CameraLockArm->SwitchTarget() and Pass in EDirection::Left (switching it to the target on the left)
				CameraLockArm->SwitchTarget(EDirection::Left);
			else //ELSE
                //CALL CameraLockArm->SwitchTarget() and Pass in EDirection::Right (switching it to the target on the right)
				CameraLockArm->SwitchTarget(EDirection::Right);
            //ENDIF

            //SET LastTargetSwitchTime to the return value of GetWorld()->GetRealTimeSeconds()
			LastTargetSwitchTime = GetWorld()->GetRealTimeSeconds();
		}
        //END ELSE IF
	}
    //ELSE
	else
	{
		// If camera lock was recently broken by a large mouse delta, allow a cooldown time to prevent erratic camera movement (This is a comment)
        //DECLARE a bool called bRecentlyBrokeLock and SET it to (GetWorld()->GetRealTimeSeconds() - BrokeLockTime) < BrokeLockAimingCooldown
		bool bRecentlyBrokeLock = (GetWorld()->GetRealTimeSeconds() - BrokeLockTime) < BrokeLockAimingCooldown;	
        //IF NOT bRecentlyBrokeLock
		if(!bRecentlyBrokeLock)
            //CALL AddControllerYawInput() and pass in Val
			AddControllerYawInput(Val);	
	}	
    //ENDIF
}

//TODO 15: Implement Looking Up (Mouse Input)
void ADSCharacter::LookUp(float Val)
{
    //IF NOT CameraLockArm->IsCameraLockedToTarget()
	if (!CameraLockArm->IsCameraLockedToTarget())
        //CALL AddControllerPitchInput() and Pass in Val
		AddControllerPitchInput(Val);
}

//TODO 16: Implement Turning at given rate (Analog Stick Input)
void ADSCharacter::TurnAtRate(float Val)
{	
	// Ensure the analog stick returned to neutral since last target switch attempt (This is a comment)
    //IF FMath::Abs(Val) Is LESS THAN .1f
	if (FMath::Abs(Val) < .1f)
        //SET bAnalogSettledSinceLastTargetSwitch to true true
		bAnalogSettledSinceLastTargetSwitch = true;

    //IF ( CameraLockArm->IsCameraLockedToTarget() is true AND ( FMath::Abs(Val) is GREATER THAN TargetSwitchAnalogValue) AND bAnalogSettledSinceLastTargetSwitch is true)
	if (CameraLockArm->IsCameraLockedToTarget() && (FMath::Abs(Val) > TargetSwitchAnalogValue) && bAnalogSettledSinceLastTargetSwitch)
	{
        //IF Val Is LESS THAN 0
		if (Val < 0)
            //CALL CameraLockArm->SwitchTarget() and Pass in EDirection::Left (switching it to the target on the left)
			CameraLockArm->SwitchTarget(EDirection::Left);
        //ELSE
		else
            //CameraLockArm->SwitchTarget() and Pass in EDirection::Right (switching it to the target on the right)
			CameraLockArm->SwitchTarget(EDirection::Right);
        //ENDIF
        //SET bAnalogSettledSinceLastTargetSwitch to false;
		bAnalogSettledSinceLastTargetSwitch = false;
	}
    //ELSE
	else
	{
		// calculate delta for this frame from the rate information (This is a comment)
        //CALL AddControllerYawInput passing in Val * BaseTurnRate * GetWorld()->GetDeltaSeconds()
		AddControllerYawInput(Val * BaseTurnRate * GetWorld()->GetDeltaSeconds());
	}
    //ENDIF
}

//TODO 17: Implement Looking Up (Analog Stick Input)
void ADSCharacter::LookUpAtRate(float Val)
{
	// calculate delta for this frame from the rate information (This is a comment)
    //IF NOT CameraLockArm->IsCameraLockedToTarget()
	if (!CameraLockArm->IsCameraLockedToTarget())
        //CALL AddControllerPitchInput() Val * BaseLookUpRate * GetWorld()->GetDeltaSeconds()
		AddControllerPitchInput(Val * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

//TODO 18: Update the Actor (We are using TickActor this time) 
//Tick is the actual tick function. TickActor is the function that calls Tick, which in return is called by ExecuteTick
//SO we are doing a pre-update update (Engines have a multiple updates i.e pre-update, update, post-update and Fixed-Update for physics)
//Have a look at https://docs.unrealengine.com/en-us/Programming/UnrealArchitecture/Actors/Ticking
void ADSCharacter::TickActor(float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction)
{
    //CALL Super
	Super::TickActor(DeltaTime, TickType, ThisTickFunction);

    //IF CameraLockArm->IsCameraLockedToTarget()
	if (CameraLockArm->IsCameraLockedToTarget())
	{
		// Vector from player to target (This is a comment)
        //DECLARE a FVector called TargetVect and SET it to CameraLockArm->CameraTarget->GetComponentLocation() - CameraLockArm->GetComponentLocation()
		FVector TargetVect = CameraLockArm->CameraTarget->GetComponentLocation() - CameraLockArm->GetComponentLocation();
        //DECLARE a FRotator called TargetRot and SET it to TargetVect.GetSafeNormal().Rotation()
		FRotator TargetRot = TargetVect.GetSafeNormal().Rotation();
        //DECLARE a FRotator called FRotator and SET it to the return value of GetControlRotation()
		FRotator CurrentRot = GetControlRotation();
        //DECLARE a FRotator and Interpolate (Lerp) it using the return value from Calling FMath::RInterpTo(CurrentRot, TargetRot, DeltaTime, LockonControlRotationRate);
		FRotator NewRot = FMath::RInterpTo(CurrentRot, TargetRot, DeltaTime, LockonControlRotationRate);

		// Update control rotation to face target (This is a comment)
        //CALL GetController()->SetControlRotation() passing in the New Rotation (NewRot)
		GetController()->SetControlRotation(NewRot);
	}
    //ENDIF
}
