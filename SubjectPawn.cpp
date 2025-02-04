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

	// 충돌 컴포넌트 생성 및 루트 설정
	CollisionBox = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionBox"));
	RootComponent = CollisionBox;

	// 화살표 컴포넌트 추가
	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	ArrowComponent->SetupAttachment(RootComponent);
	ArrowComponent->ArrowSize = 2.0f;

	// SkeletalMeshComponent 생성 및 설정
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(RootComponent);

	// SpringArm 생성 및 설정
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 300.f;
	SpringArm->bUsePawnControlRotation = true; // SpringArm이 카메라 회전 처리

	// Camera 생성 및 설정
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	Camera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;  // 캐릭터가 컨트롤러 회전을 따라가지 않음

	MovementSpeed = 100.0f;
	RotationSpeed = 100.0f;

}



void ASubjectPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!MovementInput.IsZero())
	{
		MovementInput.Normalize();

		// 카메라 기준의 전방 및 우측 벡터 계산
		FRotator CameraRotation = Camera->GetComponentRotation();

		// Pitch(카메라 상하 각도) 무시 -> 이동 방향 왜곡 방지
		CameraRotation.Pitch = 0.0f;
		CameraRotation.Roll = 0.0f;

		FVector Forward = FRotationMatrix(CameraRotation).GetUnitAxis(EAxis::X);
		FVector Right = FRotationMatrix(CameraRotation).GetUnitAxis(EAxis::Y);

		// 입력에 따라 이동 벡터 계산 (X가 좌우, Y가 전후로 매핑)
		FVector Movement = (Forward * MovementInput.X) + (Right * MovementInput.Y);

		// 월드 공간 이동
		AddActorWorldOffset(Movement * MovementSpeed * DeltaTime, true);
	}
}

void ASubjectPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Enhanced InputComponent로 캐스팅
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// IA를 가져오기 위해 현재 소유 중인 Controller를 ASpartaPlayerController로 캐스팅
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
	MovementInput = FVector2D::ZeroVector;  // 입력 초기화
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
		ControlRotation.Pitch = FMath::Clamp(ControlRotation.Pitch - LookInput.Y, -80.0f, 80.0f); // Pitch 제한
		Controller->SetControlRotation(ControlRotation);
	}
}

