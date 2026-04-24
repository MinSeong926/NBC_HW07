
#include "NPC.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshcomponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

ANPC::ANPC()
{
	PrimaryActorTick.bCanEverTick = true;
	Body = CreateDefaultSubobject<UBoxComponent>(TEXT("Body"));

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	ConstructorHelpers::FObjectFinder<USkeletalMesh> FoundedMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/Mannequins/Meshes/SKM_Manny.SKM_Manny'"));
	
	if (FoundedMesh.Succeeded() == true) {
		Mesh->SetSkeletalMeshAsset(FoundedMesh.Object);
	}
	Mesh->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
	Mesh->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

	// 스프링 암 생성
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	// 카메라 생성
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

	SetRootComponent(Body);
	Mesh->SetupAttachment(GetRootComponent());
	SpringArm->SetupAttachment(GetRootComponent());
	Camera->SetupAttachment(SpringArm);

	Body->SetSimulatePhysics(false);
	Body->SetCollisionProfileName(TEXT("OverlapAll"));
}

void ANPC::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("UE4UpDown"), this, &ThisClass::UE4UpDown);
	PlayerInputComponent->BindAxis(TEXT("UE4LeftRight"), this, &ThisClass::UE4LeftRight);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		EnhancedInput->BindAction(IAMove, ETriggerEvent::Triggered, this, &ThisClass::Move);
		EnhancedInput->BindAction(IALook, ETriggerEvent::Triggered, this, &ThisClass::Look);
	}
}

void ANPC::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PC = GetController<APlayerController>();
	if (IsValid(PC) == true) {
		if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer()) {
			if (UEnhancedInputLocalPlayerSubsystem* SubSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>()) {
				if (IMC) {
					SubSystem->AddMappingContext(IMC, 0);
				}
			}
		}
	}
}

void ANPC::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	FVector StartLocation = GetActorLocation();
	StartLocation += FVector(0.0f, 0.0f, 100.f); // 높이를 조금 올려줌

	FVector EndLocationWithLocalForwardVector = StartLocation + (GetActorForwardVector() * 100.0f);
	FVector EndLocationWithLocalRightVector = StartLocation + (GetActorRightVector() * 100.0f);
	FVector EndLocationWithLocaldForwardVector = StartLocation + (FVector::ForwardVector * 100.0f);
	FVector EndLocationWithLocaldRightVctor = StartLocation + (FVector::RightVector * 100.0f);

	DrawDebugDirectionalArrow(GetWorld(), StartLocation, EndLocationWithLocalForwardVector, 50.0f, FColor::Red, false, 0.f, 0, 2.0f);
	DrawDebugDirectionalArrow(GetWorld(), StartLocation, EndLocationWithLocalRightVector, 50.0f, FColor::Red, false, 0.f, 0, 2.0f);
	DrawDebugDirectionalArrow(GetWorld(), StartLocation, EndLocationWithLocaldForwardVector, 50.0f, FColor::Green, false, 0.f, 0, 2.0f);
	DrawDebugDirectionalArrow(GetWorld(), StartLocation, EndLocationWithLocaldRightVctor, 50.0f, FColor::Green, false, 0.f, 0, 2.0f);
}

void ANPC::UE4UpDown(float InAxisValue) {
	// UE_LOG(LogTemp, Log, TEXT("ANPC::UE4UpDown(%f)"), InAxisValue);
}

void ANPC::UE4LeftRight(float InAxisValue) {
	// UE_LOG(LogTemp, Log, TEXT("ANPC::UE4LeftRight(%f)"), InAxisValue);
}

void ANPC::Move(const FInputActionValue& InValue)
{
	FVector2D MoveInput = InValue.Get<FVector2D>();
	UE_LOG(LogTemp, Log, TEXT("MoveInput: (%.1f, %.1f"), MoveInput.X, MoveInput.Y);

	// AddActorLocalOffset(FVector(MoveInput.X, MoveInput.Y, 0.f));
	AddActorLocalOffset(FVector(MoveInput.X * 200 * GetWorld()->GetDeltaSeconds(), MoveInput.Y, 0.f));
}

void ANPC::Look(const FInputActionValue& InValue)
{
	FVector2D LookInput = InValue.Get<FVector2D>();
	UE_LOG(LogTemp, Log, TEXT("LookInput: (%.1f, %.1f"), LookInput.X, LookInput.Y);

	AddActorLocalRotation(FRotator(0.f, LookInput.X, 0.f));

	FRotator CurrentSpringArmRotation = SpringArm->GetRelativeRotation();
	CurrentSpringArmRotation += FRotator(-LookInput.Y * 200.f * GetWorld()->GetDeltaSeconds(), 0.f, 0.f);
	SpringArm->SetRelativeRotation(CurrentSpringArmRotation);
}
