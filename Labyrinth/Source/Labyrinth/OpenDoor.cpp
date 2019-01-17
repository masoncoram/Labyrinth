// Copyright Mason Coram and Brad Marx 2019

#include "OpenDoor.h"

// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();

	ActorThatOpens = GetWorld()->GetFirstPlayerController()->GetPawn();

	// Find the owning Actor
	DoorOwner = GetOwner();

}

void UOpenDoor::OpenDoor()
{
	// Set the door rotation
	DoorOwner->SetActorRotation(FRotator(0.0f, OpenAngle, 0.0f));
}

void UOpenDoor::CloseDoor()
{
	// Set the door rotation
	DoorOwner->SetActorRotation(FRotator(0.0f, CloseAngle, 0.0f));
}


// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	// If the ActorThatOpens is in the volume
	// then open the door
	if (IsOpenTrigger && PressurePlate->IsOverlappingActor(ActorThatOpens)) {
		OpenDoor();
		LastDoorOpenTime = GetWorld()->GetTimeSeconds();
	}
	if (IsCloseTrigger && PressurePlate->IsOverlappingActor(ActorThatOpens)) {
		CloseDoor();
	}

	// Check if it's time to close the door
	if (LastDoorOpenTime + DoorCloseDelay < GetWorld()->GetTimeSeconds()) {
		CloseDoor();
	}

}

