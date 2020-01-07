// Fill out your copyright notice in the Description page of Project Settings.

#include "DSLockArmComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DSTargetComponent.h"
#include "GameFramework/Pawn.h"

//TODO 0: Macro definition of print() function, so we dont have to make the same call all over the place. MAKE CODING EASIER


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


    //IF IsCameraLockedToTarget()

	{	
        //DRAW a Debug Sphere passing in the following arguments ( GetWorld(), CameraTarget->GetComponentLocation(), 20.f, 16, FColor::Red )


		// Break lock if player is too far from target (This is a comment)
        //IF  (CameraTarget->GetComponentLocation() - GetComponentLocation()).Size() > MaxTargetLockDistance + CameraTarget->GetScaledSphereRadius()

		{
            //IF bUseSoftLock is true

			{
				// Try switch to a new target in range (This is a comment)
                //IF UDSTargetComponent* NewCameraTarget = GetLockTarget()

                    //CALL LockToTarget passing in the NewCameraTarget

				//ELSE
                    //CALL BreakTargetLock()

                //ENDIF
			}
            //ELSE
			{ 
                //CALL BreakTargetLock()

			}
            //ENDIF
		}
        //ENDIF
	}
    //ELSE
	{	
        // Attempt to auto target nearby enemy (This is a comment)
        //IF bUseSoftLock is true
		
		{
            //IF UDSTargetComponent* NewCameraTarget = GetLockTarget()

			{
                // Soft-lock is reset? (This is a comment)
                //IF NOT bSoftlockRequiresReset

                    //CALL LockToTarget passing in NewCameraTarget

                //ENDIF
			}
            //ELSE // If player forcibly broke soft-lock, reset it when no target is within range (This is a comment)

			{
                //SET bSoftlockRequiresReset to false

			}
            //ENDIF
		}
        //ENDIF
	}
    //ENDIF

	//Draw debug (This is how we will be visually debugging this semester. More to come ...) (This is a comment)
    //IF bDrawDebug is true

	{
        //FOREACH UDSTargetComponent* Target in TargetComponments (CALL GetTargetComponents(), there is no TargetComponments variable, Look at the GetTargetComponents() function)

		{
            //Draw Debug Line passing in  GetWorld(), GetOwner()->GetActorLocation(), Target->GetComponentLocation(), FColor::Green

		}
        //ENDFOR

		//Draw target inclusion sphere (This is a comment)
        //Draw Debug Sphere passing in GetWorld(), GetOwner()->GetActorLocation(), MaxTargetLockDistance, 32, FColor::Cyan


        //Draw Debug String using the UKismetSystemLibrary

		
        //IF bSoftlockRequiresReset is true

            //Draw Debug String using the UKismetSystemLibrary 

        //ENDIF
	}
    //ENDIF
}

//TODO 8: Toggle the camera lock to a new lock target
void UDSLockArmComponent::ToggleCameraLock()
{
    // Soft-lock supersedes player input (This is a comment)
    //IF bUseSoftLock is true

	{
        //SET bSoftlockRequiresReset to false

        //RETURN
		return;
	}
    //ENDIF

	//If CameraTarget is set, unset it (This is a comment)
    //IF IsCameraLockedToTarget()

	{
        //CALL BreakTargetLock()

        //RETURN

	}
    //ENDIF

    //DECLARE a UDSTargetComponent* called NewCameraTarget and SET it to the return value of GetLockTarget()


    //IF NewCameraTarget NOT EQUAL nullptr

	{
        //PRINT "Testing"

        //CALL LockToTarget() passing in NewCameraTarget

	}
    //ENDIF
}

//TODO 9: Toggle to Soft Lock mode
void UDSLockArmComponent::ToggleSoftLock()
{
    //TOGGLE bUseSoftLock


    //IF bUseSoftLock is true

	{
        //PRINT "Soft-lock enabled"

        //SET bSoftlockRequiresReset to false;

	}
    //ELSE

	{
        //CALL BreakTargetLock()

        //PRINT "Soft-lock disabled"

	}
    //ENDIF
}
//TODO 5: Lock to a target
void UDSLockArmComponent::LockToTarget(UDSTargetComponent* NewTargetComponent)
{
    //SET the CameraTarget to the NewTargetComponent

    //SET bEnableCameraRotationLag to true

	//GetCharacterMovement()->bOrientRotationToMovement = false;
}

//TODO 6: Break from a locked target
void UDSLockArmComponent::BreakTargetLock()
{
    //IF IsCameraLockedToTarget()

	{
        //SET CameraTarget to nullptr

		//GetController()->SetControlRotation(FollowCamera->GetForwardVector().Rotation()); //REMOVE
        //SET bEnableCameraRotationLag to false

		//GetCharacterMovement()->bOrientRotationToMovement = true; //REMOVE
	}
    //ENDIF
}

//TODO 3: Return the Locked Target
UDSTargetComponent* UDSLockArmComponent::GetLockTarget()
{
    //DECLARE a TArray of Type UDSTargetComponent* called AvailableTargets and Assign it the return value of GetTargetComponents()

    //IF the AvailableTargets is empty

        //RETURN nullptr

    //ENDIF

	// Get the target with the smallest angle difference from the camera forward vector (This is a comment)
    //DECLARE a float called ClosestDotToCenter and assign it to 0

    
    //DECLARE a UDSTargetComponent called TargetComponent and initialize it with nullptr as a default value


    /*
        The dot product can be used to find the length of a vector or the angle between two vectors.
        The cross product is used to find a vector which is perpendicular to the plane spanned by two vectors.
    */
    //FOR i = 0, i < AvailableTargets.Num, i++

	{
        //DECLARE a float called Dot, and assign it to the return value of the FVector::DotProduct(GetForwardVector(), (AvailableTargets[i]->GetComponentLocation() - GetComponentLocation()).GetSafeNormal())

        //IF the Dot product is GREATER than the ClosestDotToCenter

		{
            //SET the ClosestDotToCenter to Dot

            //SET the TargetComponent to AvailableTargets[i]

		}
        //ENDIF
	}
    //ENDFOR

    //RETURN TargetComponent

}

//TODO 4: Switch to Locked targets (TArray documentation)
void UDSLockArmComponent::SwitchTarget(EDirection SwitchDirection)
{
    //IF the camera is not locked to a target then return

	//ENDIF

    //DECLARE a TArray of type UDSTargetComponent* called AvailableTargets and assign it to the return value of GetTargetComponents()
	

    // Must have an existing camera target and 1 additional target (This is a comment)
    //IF The number of AvailableTargets is LESS than 2, then return, Must have an existing camera target and 1 additional target
	
	//ENDIF

    //Calculate the Current Target Direction (This is a comment)
    //DECLARE a FVector called CurrentTargetDir and calculate its direction --> (CameraTarget->GetComponentLocation() - GetComponentLocation()).GetSafeNormal() //These will not be provided in ICA's


    //DECLARE a TArray of type UDSTargetComponent* called ViableTargets


    //FOREACH Target in AvailableTargets

	{
		//  Don't consider current target as a switch target (This is a comment)
        //IF the Target is EQUAL to CameraTarget

           //continue
        //ENDIF

        //Calculate the Target Direction (This is a comment)
        //DECLARE a FVector called TargetDir and Assign it to (Target->GetComponentLocation() - GetComponentLocation()).GetSafeNormal(); //These will not be provided in ICA's

        
        //Obtain the Cross Product between Current Target Direction and the Target Direction (This is a comment)
        //DECLARE a FVector called Cross and Assign it to FVector::CrossProduct(CurrentTargetDir, TargetDir)


        //SwitchDirection is EQUAL to EDirection::Right AND Crozz.Z is GREATER than 0 (This is a comment)
        //IF the SwitchDirection is EQUAL to EDirection::Left AND Crozz.Z is LESS than 0 OR 
		if ((SwitchDirection == EDirection::Left && Cross.Z < 0.f)	// Negative Z indicates left
			|| (SwitchDirection == EDirection::Right && Cross.Z > 0.f))	// Positive Z indicates right
		{
            //ADD the Target to the ViableTargets as a Unique Element i.e look up TArray documentation

		}
        //ENDIF
	}
    //END FOREACH

    //IF There are NO ViableTargets, return 

    //ENDIF
	/*
	Select the target with the smallest angle difference to the current target (This is a comment)
	*/
    //DECLARE a int32 called BBestDotIdx and assign it to 0

    //FOR i = 1, i < Number of ViableTargets, i++

	{
        //DECLARE a float called BestDot and assign it to the Dot Product between the CurrentTargetDir and the Direction of the ViableTarget at the BestDotIdx --> FVector::DotProduct(CurrentTargetDir, (ViableTargets[BestDotIdx]->GetComponentLocation() - GetComponentLocation()).GetSafeNormal());

        //DECLARE a float called TestDot and assign it to the DOt Product between the CurrentTargetDir and the Direction of the ViableTarget at i --> 		FVector::DotProduct(CurrentTargetDir, (ViableTargets[i]->GetComponentLocation() - GetComponentLocation()).GetSafeNormal());


		// Higher dot product indicates this target vector has a smaller angle than the previous best (This is a comment)
        //IF TestDot is GREATER than BestDot

            //SET BestDotIdx to i

        //ENDIF
	}
    //ENDFOR

    //CALL LockToTarget( ViableTargets[BestDotIdx] )

}

//TODO 2: Search and return targetable components
TArray<UDSTargetComponent*> UDSLockArmComponent::GetTargetComponents()
{
    //DECLARE a TArray of Type UPrimitiveComponent* called TargetPrims

    //DEClARE a TArray of type TEnumAsByte<EObjectTypeQuery> called ObjectTypes and add one element {EObjectTypeQuery::ObjectTypeQuery2}


	// Overlap check for targetable component
    //CALL UKismetSystemLibrary::SphereOverlapComponents(Owner, ComponentLocation, MaxTargetLockDistance, TArrayOfObjectTypes, UDSTargetComponent's Class, TArrayOfActorsToIgnore, OutComponents)


    //DECLARE a TArray of type UDSTargetComponent* called TargetComps

    //FOREACH component in TagrgetPrims
	{
        //ADD the component to TargetComps

	}
    //END FOREACH

    //RETURN TargetComps

}

//TODO 7: Return true is Camera is locked to a target otherwise false. Don't use a if statement 
bool UDSLockArmComponent::IsCameraLockedToTarget()
{
    //RETURN CameraTarget NOT EQUAL TO nullptr
    //return false; //this is here temporarily so it compiles, should be replace by actual working code

}
