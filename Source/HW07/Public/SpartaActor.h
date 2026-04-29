
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SpartaActor.generated.h"

class UCapsuleComponent;
class USkeletalMeshComponent;
class USpringArmComponent;
class UCameraComponent;
struct FInputActionValue;
class UInputMappingContext;
class UInputAction;

UCLASS()
class HW07_API ASpartaActor : public APawn
{
	GENERATED_BODY()

public:
	ASpartaActor();

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	// 입력 액션 바인딩용 함수
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

protected:
	// --- 필수 컴포넌트 ---
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sparta|Components")
	UCapsuleComponent* CapsuleComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sparta|Components")
	USkeletalMeshComponent* MeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sparta|Components")
	USpringArmComponent* SpringArmComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sparta|Components")
	UCameraComponent* CameraComp;

	// --- Enhanced Input ---
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sparta|Input")
	UInputMappingContext* DefaultContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sparta|Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sparta|Input")
	UInputAction* LookAction;

	// --- 이동 및 회전 변수 ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sparta|Movement")
	float MoveSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sparta|Movement")
	float RotationSpeed;
};
