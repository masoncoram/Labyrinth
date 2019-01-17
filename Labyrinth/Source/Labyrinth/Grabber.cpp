// Copyright Mason Coram and Brad Marx 2019

#include "Grabber.h"

#define OUT

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	// ...
	UE_LOG(LogTemp, Warning, TEXT("Grabber reporting for duty!"));

	FindPhysicsHandle();
	SetupInputComponent();
	
}

void UGrabber::FindPhysicsHandle()
{
	/// Look for attached Physics handle
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandle) {
		/// Physics handle is found

	}
	else {
		/// Not found physics handle
		UE_LOG(LogTemp, Error, TEXT("Owner %s does not have physics handle"), *(GetOwner()->GetName()));
	}
}

void UGrabber::SetupInputComponent()
{
	/// Look for Input Component (only appears at run time)
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent) {
		UE_LOG(LogTemp, Warning, TEXT("Input component found"));
		/// Bind the input axis
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
	else {
		/// Not found physics handle
		UE_LOG(LogTemp, Error, TEXT("Owner %s does not have input component"), *(GetOwner()->GetName()));
	}
}



void UGrabber::Grab() {
	UE_LOG(LogTemp, Warning, TEXT("Grab pressed"));

	// LINE TRACE and see if we reach any actors with physics body collision channel set
	FHitResult HitResult = GetFirstPhysicsBodyInReach();
	UPrimitiveComponent* ComponentToGrab = HitResult.GetComponent();
	AActor* ActorHit = HitResult.GetActor();

	// If we hit something then attach a physics handle
		// attach a physics handle
	if (ActorHit) {
		UE_LOG(LogTemp, Warning, TEXT("Attaching physics handle"));
		PhysicsHandle->GrabComponent(
			ComponentToGrab,
			NAME_None,
			ComponentToGrab->GetOwner()->GetActorLocation(),
			true // constrain rotation
		);
	}
}
void UGrabber::Release() {
	UE_LOG(LogTemp, Warning, TEXT("Grab released"));

	// release physics handle
	PhysicsHandle->ReleaseComponent();
}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Get player viewpoint this tick
	FVector playerLocation;
	FRotator playerRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT playerLocation,
		OUT playerRotation
	);
	FString locationString = playerLocation.ToString();
	FString rotationString = playerRotation.ToString();

	FVector LineTraceEnd = playerLocation + playerRotation.Vector() * Reach;

	// if the physics handle grabbed a component
		// move the object that we're holding
	if (PhysicsHandle->GrabbedComponent) {
		//UE_LOG(LogTemp, Warning, TEXT("Setting Target Location"));
		PhysicsHandle->SetTargetLocation(LineTraceEnd);
	}

}

const FHitResult UGrabber::GetFirstPhysicsBodyInReach()
{
	// Get player viewpoint this tick
	FVector playerLocation;
	FRotator playerRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT playerLocation,
		OUT playerRotation
	);
	FString locationString = playerLocation.ToString();
	FString rotationString = playerRotation.ToString();

	FVector LineTraceEnd = playerLocation + playerRotation.Vector() * Reach;

	// Draw a red trace in the world to visualize
	/*DrawDebugLine(
		GetWorld(),
		playerLocation,
		LineTraceEnd,
		FColor(255, 0, 0),
		false,
		0,
		0,
		10.f
	);*/

	/// Setup query parameters
	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());
	// Line-trace (Ray-Cast) out to reach distance
	FHitResult hitObject;
	GetWorld()->LineTraceSingleByObjectType(
		OUT hitObject,
		playerLocation,
		LineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParameters
	);

	// See what we hit
	AActor* actorHit = hitObject.GetActor();
	if (actorHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("Line trace hit: %s"), *(actorHit->GetName()));
	}
	return hitObject;
}
