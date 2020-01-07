// Fill out your copyright notice in the Description page of Project Settings.

#include "DSLockArmComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DSTargetComponent.h"
#include "GameFramework/Pawn.h"

//TODO 0: Macro definition of print() function, so we dont have to make the same call all over the place. MAKE CODING EASIER
#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Green, text)

//TODO 1: Study the Components variable defaults
UDSLockArmComponent::UDSLockArmComponent()
{
	MaxTargetLockDistance = 750.f;
	bDrawDebug = true;

	TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	bUsePawnControlRotation = true; // Rotate the arm based on the controller
	bEnableCameraLag = true;
	bEnableCameraRotationLag = false;
	CameraLagSpeed = 3.f;
	CameraRotationLagSpeed = 2.f;
	CameraLagMaxDistance = 100.f;
}

//TODO 10: Update the Component every frame
void UDSLockArmComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    //CALL Super Function
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    //IF IsCameraLockedToTarget()
	if (IsCameraLockedToTarget())
	{	
        //DRAW a Debug Sphere passing in the following arguments ( GetWorld(), CameraTarget->GetComponentLocation(), 20.f, 16, FColor::Red )
		DrawDebugSphere(GetWorld(), CameraTarget->GetComponentLocation(), 20.f, 16, FColor::Red); //Draw target point

		// Break lock if player is too far from target (This is a comment)
        //IF  (CameraTarget->GetComponentLocation() - GetComponentLocation()).Size() > MaxTargetLockDistance + CameraTarget->GetScaledSphereRadius()
		if ((CameraTarget->GetComponentLocation() - GetComponentLocation()).Size() > MaxTargetLockDistance + CameraTarget->GetScaledSphereRadius())
		{
            //IF bUseSoftLock is true
			if (bUseSoftLock)
			{
				// Try switch to a new target in range (This is a comment)
                //IF UDSTargetComponent* NewCameraTarget = GetLockTarget()
				if (UDSTargetComponent* NewCameraTarget = GetLockTarget())
                    //CALL LockToTarget passing in the NewCameraTarget
					LockToTarget(NewCameraTarget);
				else //ELSE
                    //CALL BreakTargetLock()
					BreakTargetLock();
                //ENDIF
			}
            //ELSE
			else 
			{ 
                //CALL BreakTargetLock()
				BreakTargetLock(); 
			}
            //ENDIF
		}
        //ENDIF
	}
    //ELSE
	else
	{	
        // Attempt to auto target nearby enemy (This is a comment)
        //IF bUseSoftLock is true
		if (bUseSoftLock) 
		{
            //IF UDSTargetComponent* NewCameraTarget = GetLockTarget()
			if (UDSTargetComponent* NewCameraTarget = GetLockTarget())
			{
                // Soft-lock is reset? (This is a comment)
                //IF NOT bSoftlockRequiresReset
				if (!bSoftlockRequiresReset) 
                    //CALL LockToTarget passing in NewCameraTarget
					LockToTarget(NewCameraTarget);
                //ENDIF
			}
            //ELSE // If player forcibly broke soft-lock, reset it when no target is within range (This is a comment)
			else 
			{
                //SET bSoftlockRequiresReset to false
				bSoftlockRequiresReset = false;
			}
            //ENDIF
		}
        //ENDIF
	}
    //ENDIF

	//Draw debug (This is how we will be visually debugging this semester. More to come ...) (This is a comment)
    //IF bDrawDebug is true
	if (bDrawDebug)
	{
        //FOREACH UDSTargetComponent* Target in TargetComponments (CALL GetTargetComponents(), there is no TargetComponments variable, Look at the GetTargetComponents() function)
		for (UDSTargetComponent* Target : GetTargetComponents())
		{
            //Draw Debug Line passing in  GetWorld(), GetOwner()->GetActorLocation(), Target->GetComponentLocation(), FColor::Green
			DrawDebugLine(GetWorld(), GetOwner()->GetActorLocation(), Target->GetComponentLocation(), FColor::Green);
		}
        //ENDFOR

		//Draw target inclusion sphere (This is a comment)
        //Draw Debug Sphere passing in GetWorld(), GetOwner()->GetActorLocation(), MaxTargetLockDistance, 32, FColor::Cyan
		DrawDebugSphere(GetWorld(), GetOwner()->GetActorLocation(), MaxTargetLockDistance, 32, FColor::Cyan);

        //Draw Debug String using the UKismetSystemLibrary
		UKismetSystemLibrary::DrawDebugString(this, FVector::ZeroVector, bUseSoftLock ? "Soft-lock Enabled" : "Soft-lock Disabled", GetOwner(), FLinearColor::Green);
		
        //IF bSoftlockRequiresReset is true
		if (bSoftlockRequiresReset)
            //Draw Debug String using the UKismetSystemLibrary 
			UKismetSystemLibrary::DrawDebugString(this, FVector(0.f, 0.f, -10.f), "Soft-lock Requires Reset", GetOwner(), FLinearColor::Green);
        //ENDIF
	}
    //ENDIF
}

//TODO 8: Toggle the camera lock to a new lock target
void UDSLockArmComponent::ToggleCameraLock()
{
    // Soft-lock supersedes player input (This is a comment)
    //IF bUseSoftLock is true
	if (bUseSoftLock)  
	{
        //SET bSoftlockRequiresReset to false
		bSoftlockRequiresReset = false;
        //RETURN
		return;
	}
    //ENDIF

	//If CameraTarget is set, unset it (This is a comment)
    //IF IsCameraLockedToTarget()
	if (IsCameraLockedToTarget())
	{
        //CALL BreakTargetLock()
		BreakTargetLock();
        //RETURN
		return;
	}
    //ENDIF

    //DECLARE a UDSTargetComponent* called NewCameraTarget and SET it to the return value of GetLockTarget()
	UDSTargetComponent* NewCameraTarget = GetLockTarget();

    //IF NewCameraTarget NOT EQUAL nullptr
	if(NewCameraTarget != nullptr)
	{
        //PRINT "Testing"
		print(TEXT("Testing"));
        //CALL LockToTarget() passing in NewCameraTarget
		LockToTarget(NewCameraTarget);
	}
    //ENDIF
}

//TODO 9: Toggle to Soft Lock mode
void UDSLockArmComponent::ToggleSoftLock()
{
    //TOGGLE bUseSoftLock
	bUseSoftLock = !bUseSoftLock;

    //IF bUseSoftLock is true
	if (bUseSoftLock)
	{
        //PRINT "Soft-lock enabled"
		print(TEXT("Soft-lock enabled"));
        //SET bSoftlockRequiresReset to false;
		bSoftlockRequiresReset = false;
	}
    //ELSE
	else
	{
        //CALL BreakTargetLock()
		BreakTargetLock();
        //PRINT "Soft-lock disabled"
		print(TEXT("Soft-lock disabled"));
	}
    //ENDIF
}
//TODO 5: Lock to a target
void UDSLockArmComponent::LockToTarget(UDSTargetComponent* NewTargetComponent)
{
    //SET the CameraTarget to the NewTargetComponent
	CameraTarget = NewTargetComponent;
    //SET bEnableCameraRotationLag to true
	bEnableCameraRotationLag = true;
	//GetCharacterMovement()->bOrientRotationToMovement = false;
}

//TODO 6: Break from a locked target
void UDSLockArmComponent::BreakTargetLock()
{
    //IF IsCameraLockedToTarget()
	if (IsCameraLockedToTarget())
	{
        //SET CameraTarget to nullptr
		CameraTarget = nullptr;
		//GetController()->SetControlRotation(FollowCamera->GetForwardVector().Rotation()); //REMOVE
        //SET bEnableCameraRotationLag to false
		bEnableCameraRotationLag = false;
		//GetCharacterMovement()->bOrientRotationToMovement = true; //REMOVE
	}
    //ENDIF
}

//TODO 3: Return the Locked Target
UDSTargetComponent* UDSLockArmComponent::GetLockTarget()
{
    //DECLARE a TArray of Type UDSTargetComponent* called AvailableTargets and Assign it the return value of GetTargetComponents()
	TArray<UDSTargetComponent*> AvailableTargets = GetTargetComponents();
    //IF the AvailableTargets is empty
	if (AvailableTargets.Num() == 0)
        //RETURN nullptr
		return nullptr;
    //ENDIF

	// Get the target with the smallest angle difference from the camera forward vector (This is a comment)
    //DECLARE a float called ClosestDotToCenter and assign it to 0
	float ClosestDotToCenter = 0.f;
    
    //DECLARE a UDSTargetComponent called TargetComponent and initialize it with nullptr as a default value
	UDSTargetComponent* TargetComponent = nullptr;

    /*
        The dot product can be used to find the length of a vector or the angle between two vectors.
        The cross product is used to find a vector which is perpendicular to the plane spanned by two vectors.
    */
    //FOR i = 0, i < AvailableTargets.Num, i++
	for (int32 i = 0; i < AvailableTargets.Num(); i++)
	{
        //DECLARE a float called Dot, and assign it to the return value of the FVector::DotProduct(GetForwardVector(), (AvailableTargets[i]->GetComponentLocation() - GetComponentLocation()).GetSafeNormal())
		float Dot = FVector::DotProduct(GetForwardVector(), (AvailableTargets[i]->GetComponentLocation() - GetComponentLocation()).GetSafeNormal());
        //IF the Dot product is GREATER than the ClosestDotToCenter
		if (Dot > ClosestDotToCenter)
		{
            //SET the ClosestDotToCenter to Dot
			ClosestDotToCenter = Dot;
            //SET the TargetComponent to AvailableTargets[i]
			TargetComponent = AvailableTargets[i];
		}
        //ENDIF
	}
    //ENDFOR

    //RETURN TargetComponent
	return TargetComponent;
}

//TODO 4: Switch to Locked targets (TArray documentation)
void UDSLockArmComponent::SwitchTarget(EDirection SwitchDirection)
{
    //IF the camera is not locked to a target then return
	if (!IsCameraLockedToTarget()) 
        return;

    //DECLARE a TArray of type UDSTargetComponent* called AvailableTargets and assign it to the return value of GetTargetComponents()
	TArray<UDSTargetComponent*> AvailableTargets = GetTargetComponents();	// Get targets within lock-on range	

    // Must have an existing camera target and 1 additional target (This is a comment)
    //IF The number of AvailableTargets is LESS than 2, then return, Must have an existing camera target and 1 additional target
	if (AvailableTargets.Num() < 2) 
        return;	

    //Calculate the Current Target Direction (This is a comment)
    //DECLARE a FVector called CurrentTargetDir and calculate its direction --> (CameraTarget->GetComponentLocation() - GetComponentLocation()).GetSafeNormal() //These will not be provided in ICA's
	FVector CurrentTargetDir = (CameraTarget->GetComponentLocation() - GetComponentLocation()).GetSafeNormal();

    //DECLARE a TArray of type UDSTargetComponent* called ViableTargets
	TArray<UDSTargetComponent*> ViableTargets;

    //FOREACH Target in AvailableTargets
	for (UDSTargetComponent* Target : AvailableTargets)
	{
		//  Don't consider current target as a switch target (This is a comment)
        //IF the Target is EQUAL to CameraTarget
		if (Target == CameraTarget) 
            continue;
           //continue
        //ENDIF

        //Calculate the Target Direction (This is a comment)
        //DECLARE a FVector called TargetDir and Assign it to (Target->GetComponentLocation() - GetComponentLocation()).GetSafeNormal(); //These will not be provided in ICA's
		FVector TargetDir = (Target->GetComponentLocation() - GetComponentLocation()).GetSafeNormal();
        
        //Obtain the Cross Product between Current Target Direction and the Target Direction (This is a comment)
        //DECLARE a FVector called Cross and Assign it to FVector::CrossProduct(CurrentTargetDir, TargetDir)
		FVector Cross = FVector::CrossProduct(CurrentTargetDir, TargetDir);

        //SwitchDirection is EQUAL to EDirection::Right AND Crozz.Z is GREATER than 0 (This is a comment)
        //IF the SwitchDirection is EQUAL to EDirection::Left AND Crozz.Z is LESS than 0 OR 
		if ((SwitchDirection == EDirection::Left && Cross.Z < 0.f)	// Negative Z indicates left
			|| (SwitchDirection == EDirection::Right && Cross.Z > 0.f))	// Positive Z indicates right
		{
            //ADD the Target to the ViableTargets as a Unique Element i.e look up TArray documentation
			ViableTargets.AddUnique(Target);
		}
        //ENDIF
	}
    //END FOREACH

    //IF There are NO ViableTargets, return 
	if (ViableTargets.Num() == 0) 
        return;
    //ENDIF
	/*
	Select the target with the smallest angle difference to the current target (This is a comment)
	*/
    //DECLARE a int32 called BBestDotIdx and assign it to 0
	int32 BestDotIdx = 0;
    //FOR i = 1, i < Number of ViableTargets, i++
	for (int32 i = 1; i < ViableTargets.Num(); i++)
	{
        //DECLARE a float called BestDot and assign it to the Dot Product between the CurrentTargetDir and the Direction of the ViableTarget at the BestDotIdx --> FVector::DotProduct(CurrentTargetDir, (ViableTargets[BestDotIdx]->GetComponentLocation() - GetComponentLocation()).GetSafeNormal());
		float BestDot = FVector::DotProduct(CurrentTargetDir, 
                                            (ViableTargets[BestDotIdx]->GetComponentLocation() - GetComponentLocation()).GetSafeNormal());
        //DECLARE a float called TestDot and assign it to the DOt Product between the CurrentTargetDir and the Direction of the ViableTarget at i --> 		FVector::DotProduct(CurrentTargetDir, (ViableTargets[i]->GetComponentLocation() - GetComponentLocation()).GetSafeNormal());
        float TestDot = FVector::DotProduct(CurrentTargetDir, 
                                            (ViableTargets[i]->GetComponentLocation() - GetComponentLocation()).GetSafeNormal());

		// Higher dot product indicates this target vector has a smaller angle than the previous best (This is a comment)
        //IF TestDot is GREATER than BestDot
		if (TestDot > BestDot)
            //SET BestDotIdx to i
			BestDotIdx = i;
        //ENDIF
	}
    //ENDFOR

    //CALL LockToTarget( ViableTargets[BestDotIdx] )
	LockToTarget(ViableTargets[BestDotIdx]);
}

//TODO 2: Search and return targetable components
TArray<UDSTargetComponent*> UDSLockArmComponent::GetTargetComponents()
{
    //DECLARE a TArray of Type UPrimitiveComponent* called TargetPrims
	TArray<UPrimitiveComponent*> TargetPrims;
    //DEClARE a TArray of type TEnumAsByte<EObjectTypeQuery> called ObjectTypes and add one element {EObjectTypeQuery::ObjectTypeQuery2}
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes = { EObjectTypeQuery::ObjectTypeQuery2 }; // World dynamic object type

	// Overlap check for targetable component
    //CALL UKismetSystemLibrary::SphereOverlapComponents(Owner, ComponentLocation, MaxTargetLockDistance, TArrayOfObjectTypes, UDSTargetComponent's Class, TArrayOfActorsToIgnore, OutComponents)
	UKismetSystemLibrary::SphereOverlapComponents(GetOwner(),GetComponentLocation(),MaxTargetLockDistance, ObjectTypes,UDSTargetComponent::StaticClass(),TArray<AActor*>{GetOwner()},TargetPrims);

    //DECLARE a TArray of type UDSTargetComponent* called TargetComps
	TArray<UDSTargetComponent*> TargetComps;
    //FOREACH component in TagrgetPrims
	for (UPrimitiveComponent* Comp : TargetPrims)
	{
        //ADD the component to TargetComps
		TargetComps.Add(Cast<UDSTargetComponent>(Comp));
	}
    //END FOREACH

    //RETURN TargetComps
	return TargetComps;
}

//TODO 7: Return true is Camera is locked to a target otherwise false. Don't use a if statement 
bool UDSLockArmComponent::IsCameraLockedToTarget()
{
    //RETURN CameraTarget NOT EQUAL TO nullptr
    //return false; //this is here temporarily so it compiles, should be replace by actual working code
	return CameraTarget != nullptr;
}
