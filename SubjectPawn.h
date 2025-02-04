#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SubjectPawn.generated.h"

class UCapsuleComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class USpringArmComponent;
class UArrowComponent;
// Enhanced Input에서 액션 값을 받을 때 사용하는 구조체
struct FInputActionValue;

UCLASS()
class SPARTAPROJECT_API ASubjectPawn : public APawn
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterSet")
	float MovementSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterSet")
	float RotationSpeed;
	ASubjectPawn();
	virtual void Tick(float DeltaTime) override;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UCapsuleComponent* CollisionBox;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    USkeletalMeshComponent* SkeletalMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
    USpringArmComponent* SpringArm;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UArrowComponent* ArrowComponent;


	// IA_Move와 IA_Jump 등을 처리할 함수 원형
	// // Enhanced Input에서 액션 값은 FInputActionValue로 전달됩니다.
	UFUNCTION()
	void Move(const FInputActionValue& value);
	UFUNCTION()
	void StopMove(const FInputActionValue& value);
	UFUNCTION()
	void Look(const FInputActionValue& value);
	UFUNCTION()
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	

private:
	FVector2D MovementInput;
	FVector2D RotationInput;
	
};
