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

	//LocalPlayer�� �� �÷��̾��� �Է��̳� ȭ��並 �����ϴ� ��ü�̴�. -> �Է��� �����ϴ� ��ü�̴�.(�÷��̾�� �پ�����)
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		// Local Player���� EnhancedInputLocalPlayerSubsystem�� ȹ��
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContext)
			{
				// Subsystem�� ���� �츮�� �Ҵ��� IMC�� Ȱ��ȭ
				// �켱����(Priority)�� 0�� ���� ���� �켱����
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}
	
}