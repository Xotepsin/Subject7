# 7번 필수 과제
---

## Pawn클래스 생성
```cpp
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
```
`Pawn.h`
```cpp
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
```
`Pawn.cpp`

우선 `CapsuleComponent`, `SkeletalMeshComponent`, `SpringArm`, `CameraComponent`를 Pawn에 추가해주고, 팔 길이를 설정하고 `Collision`을 `RootComponent`로 설정하고 `RootComponent`에 `SetupAttachment`로 `SkeletalMeshComponent`,`SpringArm`,`Camera`를 달아줬습니다.

그 후 Pawn클래스를 블리프린트로 생성하고 캡슐크기와 위치를 좀 수정해주었습니다. 

![](https://velog.velcdn.com/images/xotepsin/post/618f5a5d-875c-4b45-b499-0865e6794387/image.png)


## IA, IMC생성
---
![](https://velog.velcdn.com/images/xotepsin/post/dc4d5786-3125-4027-868c-1bd9183ac533/image.png)![](https://velog.velcdn.com/images/xotepsin/post/642cb199-6f2d-4139-97fa-6e785420f7d5/image.png)


![](https://velog.velcdn.com/images/xotepsin/post/b4e2c813-f373-415c-ba06-31ce7ad4ace0/image.png)


![](https://velog.velcdn.com/images/xotepsin/post/1e5433f9-cfb0-43e7-a1be-142aad65f5ce/image.png)
위와 같이 `IA_MOVE`,`IA_LOOK`의 이름으로 입력액션을 만들고 둘다 값타입은 Axis2D(Vector2D)로 설정해주었습니다.

```cpp
class UInputMappingContext;
class UInputAction;

UCLASS()
class SPARTAPROJECT_API ASpartaPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ASpartaPlayerController();
	UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
protected:
	virtual void BeginPlay() override;
};
```
`PlayerController.h`
```cpp
#include "SpartaPlayerController.h"
#include "EnhancedInputSubsystems.h"

ASpartaPlayerController::ASpartaPlayerController()
	: InputMappingContext(nullptr),
	MoveAction(nullptr),
	JumpAction(nullptr),
	LookAction(nullptr),
	SprintAction(nullptr)
{

}

void ASpartaPlayerController::BeginPlay()
{
	Super::BeginPlay();

	//LocalPlayer는 그 플레이어의 입력이나 화면뷰를 관리하는 객체이다. -> 입력을 관리하는 객체이다.(플레이어마다 붙어있음)
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		// Local Player에서 EnhancedInputLocalPlayerSubsystem을 획득
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContext)
			{
				// Subsystem을 통해 우리가 할당한 IMC를 활성화
				// 우선순위(Priority)는 0이 가장 높은 우선순위
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}
	
}
```
`PlayerController.cpp`

![](https://velog.velcdn.com/images/xotepsin/post/8770a731-9a0c-401b-9902-ac5254d642b3/image.png)
![](https://velog.velcdn.com/images/xotepsin/post/6ebfae3e-1592-4b8b-be52-07ebaba14ecd/image.png)
![](https://velog.velcdn.com/images/xotepsin/post/d1150aa1-de1c-426c-a5bb-bcc668c56571/image.png)
![](https://velog.velcdn.com/images/xotepsin/post/e367f275-374f-447b-9c9a-68b6ea1ce05b/image.png)
그후 IMC로 IA들을 모두 매핑해 주었습니다.

## Pawn클래스에 동작 바인딩
---
```cpp
protected:
	UFUNCTION()
	void Move(const FInputActionValue& value);
	UFUNCTION()
	void StopMove(const FInputActionValue& value);
	UFUNCTION()
	void Look(const FInputActionValue& value);
private:
	FVector2D MovementInput;
	FVector2D RotationInput;
```
`Pawn.h`
헤더파일에 우선 필요한 함수들과 변수들을 선언해둡니다.
```cpp
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
```
`Pawn.cpp`
그 후 `MovementInput`은 Tick()함수에서 구현하기위해 `FInputActionValue`로 받아만와두고 `Look`함수는 구현해둡니다.
#### `FVector2D LookInput = value.Get<FVector2D>();`
입력된 Look 값을 받아 2D 벡터로 저장합니다.
- `LookInput.X`: Yaw(좌우 회전),
- `LookInput.Y`: Pitch(상하 회전) 값에 대응됩니다.
#### `if (!LookInput.IsNearlyZero())`
- 입력 값이 거의 0이 아닌 경우에만 회전을 처리합니다.
- 불필요한 계산을 피하기 위해 입력이 없을 때 코드를 실행하지 않도록 합니다.
#### Actor의 Yaw 회전
```cpp
FRotator ActorRotation = GetActorRotation();
ActorRotation.Yaw += LookInput.X;
SetActorRotation(ActorRotation);
```
- Pawn 자체(액터)의 회전을 설정합니다.
- 입력된 LookInput.X 값을 사용해 Yaw(좌우 방향) 회전을 업데이트합니다.
- `SetActorRotation()`으로 변경된 회전 값을 설정합니다.
#### Controller의 카메라 회전 (Yaw + Pitch)
```cpp
FRotator ControlRotation = GetControlRotation();
ControlRotation.Yaw += LookInput.X;
ControlRotation.Pitch = FMath::Clamp(ControlRotation.Pitch - LookInput.Y, -80.0f, 80.0f); 
Controller->SetControlRotation(ControlRotation);
```
- 컨트롤러의 카메라 회전을 업데이트합니다.
- Yaw는 좌우 회전을 단순히 더합니다.
- Pitch는 상하 회전인데 제한(-80.0f ~ 80.0f)을 둬서 카메라가 지나치게 위나 아래로 회전하지 않도록 합니다.
  - FMath::Clamp()로 제한을 적용합니다.
- Controller->SetControlRotation()으로 새로운 회전을 적용합니다.

```cpp
			EnhancedInput->BindAction(
				PlayerController->MoveAction,
				ETriggerEvent::Completed,
				this,
				&ASubjectPawn::StopMove
			);
```
Move를 Tick()으로 구현하다보니 이게 입력을 한번받으면 쭉 이동해서 `ETriggerEvent::Completed`로 `StopMove`함수를 구현했습니다.

```cpp
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
```
#### if (!MovementInput.IsZero())
- 입력(`MovementInput`)이 0이 아닌 경우에만 이동을 처리합니다.
#### MovementInput.Normalize();
- 입력 벡터를 정규화합니다.
- 입력 벡터의 길이를 1로 만들어 방향 정보만 유지하면서 크기(속도)는 이후 계산에 의해 적용되도록 합니다.
#### 카메라 기준으로 이동 방향 계산
```cpp
FRotator CameraRotation = Camera->GetComponentRotation();
CameraRotation.Pitch = 0.0f;
CameraRotation.Roll = 0.0f;
```
- 카메라의 회전값(FRotator)을 가져옵니다.
- 이동 방향 왜곡을 방지하기 위해 **Pitch(상하 각도)**와 Roll(기울기) 값을 0으로 설정합니다.
  - 상하 카메라 각도가 이동 방향에 영향을 주지 않도록 무시합니다.
  
#### 카메라 기준 전방 및 우측 벡터 계산
```cpp
FVector Forward = FRotationMatrix(CameraRotation).GetUnitAxis(EAxis::X);
FVector Right = FRotationMatrix(CameraRotation).GetUnitAxis(EAxis::Y);
```
- `FRotationMatrix`를 사용해 회전 정보를 기반으로 전방(X 축) 및 우측(Y 축) 벡터를 구합니다.
`EAxis::X`: 전방 벡터
`EAxis::Y`: 우측 벡터

#### 입력에 따른 이동 벡터 계산
```cpp
FVector Movement = (Forward * MovementInput.X) + (Right * MovementInput.Y);
```
- 입력 벡터를 사용해 이동 방향을 계산합니다.
  - MovementInput.X: 좌우 이동
  - MovementInput.Y: 전후 이동
- 전방과 우측 벡터를 각각 입력 값에 곱해 합산합니다.
#### 월드 공간에서 이동 처리
```cpp
AddActorWorldOffset(Movement * MovementSpeed * DeltaTime, true);
```
- 이동 벡터에 속도(`MovementSpeed`)와 시간(`DeltaTime`)을 곱해 프레임 보정된 이동 값을 구합니다.
- `AddActorWorldOffset()`으로 월드 좌표에서 Pawn을 이동시킵니다.
- `true`: 충돌 감지 활성화 (다른 액터와 충돌 시 이동 중단)

그러고 나서 월드세팅에서
![](https://velog.velcdn.com/images/xotepsin/post/63c258ee-2242-4ce3-893f-b3f895a7267b/image.png)
`DefaulPawnClass`를 `BP_SubjectPawn`으로 설정하고 실행했습니다.
# 시연영상
---
