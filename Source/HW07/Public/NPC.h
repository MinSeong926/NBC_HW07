
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
	void UE4UpDown(float InAxisValue);
	void UE4LeftRight(float InAxisValue);

	void Move(const FInputActionValue& InValue);
	void Look(const FInputActionValue& InValue);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC")
	UBoxComponent* Body;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC")
	USkeletalMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC")
	USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC")
	UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC|Input")
	UInputMappingContext* IMC;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC|Input")
	UInputAction* IAMove;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC|Input")
	UInputAction* IALook;
};
