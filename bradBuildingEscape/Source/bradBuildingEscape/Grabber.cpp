// Fill out your copyright notice in the Description page of Project Settings.

#include "Grabber.h"
#include "Engine/World.h"
#include "RunTime/Engine/Public/DrawDebugHelpers.h"
#include "Gameframework/Actor.h"

#define OUT


// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

/// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	FindPhysicsHandleComponent();

	SetupInputComponent();
}

/// Look for attached physics Handle
void UGrabber::FindPhysicsHandleComponent() 
{
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandle)
	{
		UE_LOG(LogTemp, Warning, TEXT("Physics Handle Component Found!"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No PhysicsHandle found in %s!"), *GetOwner()->GetName());
	}
}

/// Look for attached input component (only appears at runtime)
void UGrabber::SetupInputComponent()
{
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent)
	{
		/// bind the input axis
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::GrabItem);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::ReleaseItem);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No InputComponent found in %s!"), *GetOwner()->GetName());
	}
}

void UGrabber::GrabItem()
{
	// Line trace and see if we reach any actors with physics bodyt collision channel set
	auto HitResult = GetFirstPhysicsBodyInReach();
	auto ComponentToGrab = HitResult.GetComponent();  // here this retrieves the mesh component
	auto ActorHit = HitResult.GetActor();

	// If we hit something then attach a physics handle
	if (ActorHit)
	{
		if (!PhysicsHandle) { return; }
		// attach physics handle
		PhysicsHandle->GrabComponent(
			ComponentToGrab,
			NAME_None, // no bones needed
			ComponentToGrab->GetOwner()->GetActorLocation(),
			true // bool allow rotation
		);
	}
}

void UGrabber::ReleaseItem()
{
	if (!PhysicsHandle) { return; }
	//TODO release physics handle
	PhysicsHandle->ReleaseComponent();
}


/// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!PhysicsHandle) { return; }
	// If the physics handle is attached
	if (PhysicsHandle->GrabbedComponent)
	{
		// move the object that we're holding
		PhysicsHandle->SetTargetLocation(GetLineTraceEnd());

	}
}

FVector UGrabber::GetLineTraceStart()
{
	/// get player viewpoint
	FVector  PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
	);

	return PlayerViewPointLocation;
}

FVector UGrabber::GetLineTraceEnd()
{
	/// get player viewpoint
	FVector  PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
	);

	return PlayerViewPointLocation + (PlayerViewPointRotation.Vector() * Reach);// FVector(Reach, 0.0f, 00.0f);
}


const FHitResult UGrabber::GetFirstPhysicsBodyInReach()
{
	/*
	/// draw red debug line
	DrawDebugLine(
		GetWorld(),
		PlayerViewPointLocation,
		LineTraceEnd,
		FColor(255, 0, 0),
		false,
		0.0f,
		0.0f,
		10.0f
	);
	*/
	/// Ray-Casting out to reach distance
	FHitResult Hit;
	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner()); // Setup query Parameters
	GetWorld()->LineTraceSingleByObjectType(
		OUT Hit,
		GetLineTraceStart(),
		GetLineTraceEnd(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParameters
	);
	// see what we hit
	AActor* ActorHit = Hit.GetActor();
	if (ActorHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("Line Trace Hit: %s!"), *(ActorHit->GetName()));
	}

	return Hit;
}