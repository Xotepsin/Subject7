#include "SubjectPawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/ArrowComponent.h"
#include "EnhancedInputComponent.h"
#include "SpartaPlayerController.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"


ASubjectPawn::ASubjectPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	// �浹 ������Ʈ ���� �� ��Ʈ ����
	CollisionBox = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionBox"));
	RootComponent = CollisionBox;

	// ȭ��ǥ ������Ʈ �߰�
	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	ArrowComponent->SetupAttachment(RootComponent);
	ArrowComponent->ArrowSize = 2.0f;

	// SkeletalMeshComponent ���� �� ����
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(RootComponent);

	// SpringArm ���� �� ����
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 300.f;
	SpringArm->bUsePawnControlRotation = true; // SpringArm�� ī�޶� ȸ�� ó��

	// Camera ���� �� ����
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	Camera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;  // ĳ���Ͱ� ��Ʈ�ѷ� ȸ���� ������ ����

	MovementSpeed = 100.0f;
	RotationSpeed = 100.0f;

}



void ASubjectPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!MovementInput.IsZero())
	{
		MovementInput.Normalize();

		// ī�޶� ������ ���� �� ���� ���� ���
		FRotator CameraRotation = Camera->GetComponentRotation();

		// Pitch(ī�޶� ���� ����) ���� -> �̵� ���� �ְ� ����
		CameraRotation.Pitch = 0.0f;
		CameraRotation.Roll = 0.0f;

		FVector Forward = FRotationMatrix(CameraRotation).GetUnitAxis(EAxis::X);
		FVector Right = FRotationMatrix(CameraRotation).GetUnitAxis(EAxis::Y);

		// �Է¿� ���� �̵� ���� ��� (X�� �¿�, Y�� ���ķ� ����)
		FVector Movement = (Forward * MovementInput.X) + (Right * MovementInput.Y);

		// ���� ���� �̵�
		AddActorWorldOffset(Movement * MovementSpeed * DeltaTime, true);
	}
}

void ASubjectPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Enhanced InputComponent�� ĳ����
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// IA�� �������� ���� ���� ���� ���� Controller�� ASpartaPlayerController�� ĳ����
		if (ASpartaPlayerController* PlayerController = Cast<ASpartaPlayerController>(GetController()))
		{
			EnhancedInput->BindAction(
				PlayerController->MoveAction,
				ETriggerEvent::Triggered,
				this,
				&ASubjectPawn::Move
			);

			EnhancedInput->BindAction(
				PlayerController->MoveAction,
				ETriggerEvent::Completed,
				this,
				&ASubjectPawn::StopMove
			);

			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction(
					PlayerController->LookAction,
					ETriggerEvent::Triggered,
					this,
					&ASubjectPawn::Look
				);
			}
		}
	}

}
void ASubjectPawn::Move(const FInputActionValue& value)
{
	MovementInput = value.Get<FVector2D>();
}

void ASubjectPawn::StopMove(const FInputActionValue& value)
{
	MovementInput = FVector2D::ZeroVector;  // �Է� �ʱ�ȭ
}

void ASubjectPawn::Look(const FInputActionValue& value)
{
	FVector2D LookInput = value.Get<FVector2D>();
	if (!LookInput.IsNearlyZero())
	{
		FRotator ActorRotation = GetActorRotation();
		ActorRotation.Yaw += LookInput.X;
		SetActorRotation(ActorRotation);

		FRotator ControlRotation = GetControlRotation();
		ControlRotation.Yaw += LookInput.X;
		ControlRotation.Pitch = FMath::Clamp(ControlRotation.Pitch - LookInput.Y, -80.0f, 80.0f); // Pitch ����
		Controller->SetControlRotation(ControlRotation);
	}
}

