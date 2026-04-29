
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "NPC.generated.h"

class UBoxComponent;
class USkeletalMeshComponent;
class USpringArmComponent;
class UCameraComponent;
struct FInputActionValue;
class UInputMappingContext;
class UInputAction;

UCLASS()
class HW07_API ANPC : public APawn
{
	GENERATED_BODY()

public:
	ANPC();

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent);

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

private:
	// 이동, 회전 입력 처리 함수
	void Move(const FInputActionValue& InValue);
	void Look(const FInputActionValue& InValue);
	void UpDown(const FInputActionValue& InValue); // 상하 이동 (Space/Shift)
	void Roll(const FInputActionValue& InValue);   // 롤 회전 (Q/E 또는 휠)

	// 중력 및 지면 체크 함수
	void ApplyGravityAndCheckGround(float DeltaTime);

	// 상태 변수
	FVector CurrentVelocity;
	bool bIsGrounded;

protected:
	// 컴포넌트
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC|Components")
	UBoxComponent* Body;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC|Components")
	USkeletalMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC|Components")
	USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC|Components")
	UCameraComponent* Camera;

	// 입력 액션
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC|Input")
	UInputMappingContext* IMC;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC|Input")
	UInputAction* IAMove;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC|Input")
	UInputAction* IALook;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC|Input")
	UInputAction* IAUpDown; // 추가: Z축 로컬 이동

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC|Input")
	UInputAction* IARoll;   // 추가: Roll 회전

	// 이동 및 물리 설정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC|Movement")
	float MoveSpeed = 600.f; // 지상 이동 속도

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC|Movement")
	float RotationSpeed = 100.f; // 회전 속도 민감도

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC|Movement")
	float AirControlMultiplier = 0.4f; // 공중 상태 속도 제한 (40%)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC|Physics")
	float Gravity = -980.f; // 중력 가속도

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC|Physics")
	float TraceDistance = 60.f; // 바닥 충돌 감지 거리 (Box 크기에 맞게 조절)
};
