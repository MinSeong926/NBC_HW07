
#include "NPC.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshcomponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "DrawDebugHelpers.h"

ANPC::ANPC()
{
	PrimaryActorTick.bCanEverTick = true;

	Body = CreateDefaultSubobject<UBoxComponent>(TEXT("Body"));
	SetRootComponent(Body);
	Body->SetSimulatePhysics(false); // 물리 엔진 비활성화 (직접 제어)
	Body->SetCollisionProfileName(TEXT("Pawn"));

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	ConstructorHelpers::FObjectFinder<USkeletalMesh> FoundedMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Cinematic_Dragonfly/Meshes/SK_Dragonfly.SK_Dragonfly'"));
	if (FoundedMesh.Succeeded()) {
		Mesh->SetSkeletalMeshAsset(FoundedMesh.Object);
	}
	Mesh->SetupAttachment(GetRootComponent());
	Mesh->SetRelativeLocation(FVector(0.f, 0.f, -50.f)); // Box 크기에 맞춰 Z축 조절 필요
	Mesh->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 400.f;
	SpringArm->bUsePawnControlRotation = false; // 6자유도를 위해 Pawn 자체 회전을 따름

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	CurrentVelocity = FVector::ZeroVector;
	bIsGrounded = false;
}

void ANPC::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PC = Cast<APlayerController>(GetController())) {
		if (UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer())) {
			if (IMC) {
				SubSystem->AddMappingContext(IMC, 0);
			}
		}
	}
}

void ANPC::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		// 필수 구현 기능
		if (IAMove) EnhancedInput->BindAction(IAMove, ETriggerEvent::Triggered, this, &ANPC::Move);
		if (IALook) EnhancedInput->BindAction(IALook, ETriggerEvent::Triggered, this, &ANPC::Look);

		// 도전 구현 기능 (상하 이동, 롤 회전)
		if (IAUpDown) EnhancedInput->BindAction(IAUpDown, ETriggerEvent::Triggered, this, &ANPC::UpDown);
		if (IARoll) EnhancedInput->BindAction(IARoll, ETriggerEvent::Triggered, this, &ANPC::Roll);
	}
}

void ANPC::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// 2. 중력 및 낙하 시스템 적용
	ApplyGravityAndCheckGround(DeltaSeconds);
}

void ANPC::Move(const FInputActionValue& InValue)
{
	FVector2D MoveInput = InValue.Get<FVector2D>();

	// 3. 에어 컨트롤 분기 (공중일 때 속도 제한)
	float CurrentSpeed = bIsGrounded ? MoveSpeed : (MoveSpeed * AirControlMultiplier);

	// 1. Local 기반 이동 (Forward/Right)
	FVector DeltaLocation = FVector(MoveInput.X, MoveInput.Y, 0.f) * CurrentSpeed * GetWorld()->GetDeltaSeconds();

	// bSweep을 true로 설정하여 이동 시 벽과 충돌하도록 처리
	AddActorLocalOffset(DeltaLocation, true);
}

void ANPC::UpDown(const FInputActionValue& InValue)
{
	float UpDownInput = InValue.Get<float>();
	float CurrentSpeed = bIsGrounded ? MoveSpeed : (MoveSpeed * AirControlMultiplier);

	// Local 기반 Z축 상하 이동
	FVector DeltaLocation = FVector(0.f, 0.f, UpDownInput) * CurrentSpeed * GetWorld()->GetDeltaSeconds();
	AddActorLocalOffset(DeltaLocation, true);

	// 비행체처럼 올라가기
	CurrentVelocity.Z = 0.f;
}

void ANPC::Look(const FInputActionValue& InValue)
{
	FVector2D LookInput = InValue.Get<FVector2D>();

	// 6자유도를 위해 폰(Actor) 자체에 Pitch(Y)와 Yaw(X) 회전 적용
	//float PitchInput = LookInput.Y * RotationSpeed * GetWorld()->GetDeltaSeconds();
	//float YawInput = LookInput.X * RotationSpeed * GetWorld()->GetDeltaSeconds();

	//AddActorLocalRotation(FRotator(PitchInput, 0.f, 0.f));
	//AddActorLocalRotation(FRotator(0.f, YawInput, 0.f));

	// 좌우 회전
	AddControllerYawInput(LookInput.X);
	// 상하 회전
	AddControllerPitchInput(LookInput.Y);
}

void ANPC::Roll(const FInputActionValue& InValue)
{
	float RollInput = InValue.Get<float>();
	float RollAmount = RollInput * RotationSpeed * GetWorld()->GetDeltaSeconds();

	// Roll 회전 적용
	AddActorLocalRotation(FRotator(0.f, 0.f, RollAmount));
}

void ANPC::ApplyGravityAndCheckGround(float DeltaTime)
{
	FVector StartLocation = GetActorLocation();
	// Z축 아래 방향으로 LineTrace (World 축 기준 낙하)
	FVector EndLocation = StartLocation + FVector(0.f, 0.f, -TraceDistance);

	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	// 바닥 충돌 체크
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, CollisionParams);

	if (bHit)
	{
		// 지면에 닿은 상태
		bIsGrounded = true;
		CurrentVelocity.Z = 0.f; // Z축 낙하 속도 초기화
	}
	else
	{
		// 공중에 있는 상태
		bIsGrounded = false;
		CurrentVelocity.Z += Gravity * DeltaTime; // 매 프레임 중력 가속도 누적

		// World 좌표계 기준으로 낙하 이동 적용
		AddActorWorldOffset(CurrentVelocity * DeltaTime, true);
	}

	// 디버그 라인 출력 (필요 시 주석 처리)
	//FColor LineColor = bIsGrounded ? FColor::Green : FColor::Red;
	//DrawDebugLine(GetWorld(), StartLocation, EndLocation, LineColor, false, -1.f, 0, 2.f);
}