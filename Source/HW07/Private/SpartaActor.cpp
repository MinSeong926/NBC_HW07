
#include "SpartaActor.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

ASpartaActor::ASpartaActor()
{
	PrimaryActorTick.bCanEverTick = false;

	// 속도 초기화
	MoveSpeed = 600.0f;
	RotationSpeed = 100.0f;

	// 1. 루트 충돌체 (CapsuleComponent) 생성 및 설정
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	SetRootComponent(CapsuleComp);
	CapsuleComp->SetSimulatePhysics(false); // 물리 비활성화 (필수 조건)

	CapsuleComp->InitCapsuleSize(42.f, 96.0f); // 캡슐 크기 설정

	// 2. 스켈레탈 메시 생성 및 설정
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);
	MeshComp->SetSimulatePhysics(false); // 물리 비활성화 (필수 조건)

	// 메시 위치 조정
	MeshComp->SetRelativeLocation(FVector(0.f, 0.f, -96.0f));
	MeshComp->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

	// 3. 스프링 암 생성 및 설정
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 400.0f;
	SpringArmComp->bUsePawnControlRotation = false;

	// 4. 카메라 생성 및 설정
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);
}

void ASpartaActor::BeginPlay()
{
	Super::BeginPlay();

	// Enhanced Input Mapping Context 등록
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (DefaultContext)
			{
				Subsystem->AddMappingContext(DefaultContext, 0);
			}
		}
	}
}

void ASpartaActor::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Move와 Look 액션 바인딩
		if (MoveAction)
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASpartaActor::Move);
		}

		if (LookAction)
		{
			EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASpartaActor::Look);
		}
	}
}

// 이동 구현 (WASD)
void ASpartaActor::Move(const FInputActionValue& Value)
{
	FVector2D MoveVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// DeltaTime을 사용하여 프레임 독립적인 이동 계산
		float DeltaTime = GetWorld()->GetDeltaSeconds();

		FVector DeltaLocation = FVector(MoveVector.X, MoveVector.Y, 0.0f) * MoveSpeed * DeltaTime;

		AddActorLocalOffset(DeltaLocation, true);
	}
}

// 회전 구현 (마우스)
void ASpartaActor::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		float DeltaTime = GetWorld()->GetDeltaSeconds();

		float YawInput = LookAxisVector.X * RotationSpeed * DeltaTime;
		float PitchInput = LookAxisVector.Y * RotationSpeed * DeltaTime;

		AddActorLocalRotation(FRotator(0.0f, YawInput, 0.0f));

		SpringArmComp->AddLocalRotation(FRotator(PitchInput, 0.0f, 0.0f));
	}
}