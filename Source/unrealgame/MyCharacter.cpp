// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include <Camera/CameraComponent.h>
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MyAnimInstance.h"
#include "MyUserWidget.h"
#include "Blueprint/UserWidget.h"

// Sets default values
AMyCharacter::AMyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bCanJump = true;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/ControlRig/Characters/Mannequins/Meshes/SKM_Manny_Simple.SKM_Manny_Simple'"));
	if (TempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(TempMesh.Object);
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimBPClass(TEXT("/Script/Engine.AnimBlueprint'/Game/Animations/ABP_MyCharacter.ABP_MyCharacter_C'"));
	if (AnimBPClass.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(AnimBPClass.Class);
	}

	springArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	springArmComp->SetupAttachment(RootComponent);
	springArmComp->SetRelativeLocationAndRotation(FVector(0, 0, 90), FRotator(0,0,0));
	springArmComp->TargetArmLength = 450;
	springArmComp->bUsePawnControlRotation = false;
	springArmComp->bInheritYaw = false;
	cameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	cameraComp->SetupAttachment(springArmComp);
	cameraComp->bUsePawnControlRotation = false;

	static ConstructorHelpers::FObjectFinder<UInputMappingContext>DEFAULT_CONTEXT(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/Actions/IMC_Action.IMC_Action'"));
	static ConstructorHelpers::FObjectFinder<UInputAction>IA_MOVE(TEXT("/Script/EnhancedInput.InputAction'/Game/Actions/IA_Move.IA_Move'"));
	static ConstructorHelpers::FObjectFinder<UInputAction>IA_RUN(TEXT("/Script/EnhancedInput.InputAction'/Game/Actions/IA_Run.IA_Run'"));
	static ConstructorHelpers::FObjectFinder<UInputAction>IA_JUMP(TEXT("/Script/EnhancedInput.InputAction'/Game/Actions/IA_Jump.IA_Jump'"));
	static ConstructorHelpers::FObjectFinder<UInputAction>IA_LOOK(TEXT("/Script/EnhancedInput.InputAction'/Game/Actions/IA_Look.IA_Look'"));
	static ConstructorHelpers::FClassFinder<UMyUserWidget> WidgetClass(TEXT("/Game/Widgets/BP_HMBar"));
	
	if (WidgetClass.Succeeded())
	{
		MyUserWidgetClass = WidgetClass.Class;
		UE_LOG(LogTemp, Log, TEXT("HPMPWidgetClass is successfully set in constructor."));
	}

	if (DEFAULT_CONTEXT.Succeeded())
	{
		DefaultMappingContext = DEFAULT_CONTEXT.Object;
	}
	
	if (IA_MOVE.Succeeded())
	{
		MoveAction = IA_MOVE.Object;
	}

	if (IA_RUN.Succeeded())
	{
		RunAction = IA_RUN.Object;
	}

	if (IA_JUMP.Succeeded())
	{
		JumpAction = IA_JUMP.Object;
	}

	if (IA_LOOK.Succeeded())
	{
		LookAction = IA_LOOK.Object;
	}
	// �⺻ �̼� ����
	GetCharacterMovement()->MaxWalkSpeed = 400.0f;

	// ���� ���� ��ȯ ����
	GetCharacterMovement()->AirControl = 1.0f;
	
	// ���� ���� ����
	GetCharacterMovement()->JumpZVelocity = 500.0f;

	// ĳ���� ȸ��
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->RotationRate = FRotator(0.0f, 21600.0f, 0.0f);

	// ĳ���� HP MP ����
	CurrentHP = 100.0f;
	MaxHP = 100.0f;
	CurrentMP = 50.0f;
	MaxMP = 50.0f;
	StaminaDrainRate = 10.0f;
	StaminaRecoveryRate = 5.0f;

	bIsRun = false;
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();

	// // Mesh�� �⺻ ��ġ �� ȸ�� ���� (�ʿ� �� ����)
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -89.649996f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

	// ���� ���� �� ȭ�鿡 �߰�
	if (MyUserWidgetClass)
	{
		MyUserWidgetInstance = CreateWidget<UMyUserWidget>(GetWorld(), MyUserWidgetClass);
		if (MyUserWidgetInstance)
		{
			MyUserWidgetInstance->AddToViewport();
		}
	}

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* SubSystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			SubSystem->AddMappingContext(DefaultMappingContext, 0);
		}
		// PlayerCameraManager�� ����Ͽ� Pitch ���� ����
		if (PlayerController->PlayerCameraManager)
		{
			PlayerController->PlayerCameraManager->ViewPitchMin = -80.0f; // �ּ� Pitch
			PlayerController->PlayerCameraManager->ViewPitchMax = 80.0f;  // �ִ� Pitch
		}
	}
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// HP/MP ���� ������ ����
	if (MyUserWidgetInstance)
	{
		MyUserWidgetInstance->UpdateHP(CurrentHP, MaxHP);
		MyUserWidgetInstance->UpdateMP(CurrentMP, MaxMP);
	}

	HandleStamina(DeltaTime);
}

void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);


	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyCharacter::Move);
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Started, this, &AMyCharacter::StartRun);
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &AMyCharacter::StopRun);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AMyCharacter::Jump);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyCharacter::Look);
	}
}

// �̵� �Լ�
void AMyCharacter::Move(const FInputActionValue& Value)
{
	// �̵��� ���� ����� �ӵ��� ��Ÿ��
	const FVector2D MovementVector = Value.Get<FVector2D>();
	if (springArmComp && MovementVector.SizeSquared() > 0.0f) // �Է� ���� ��ȿ�� ���
	{
		// SpringArm�� ����� ȸ�� �� ��������
		FRotator CameraRotation = springArmComp->GetRelativeRotation();
		CameraRotation.Pitch = 0.0f; // Pitch ���� (�̵� ���⿡�� ������ ���� ����)
		CameraRotation.Roll = 0.0f;  // Roll ���� (�̵� ���⿡�� ������ ���� ����)

		// ī�޶� �ٶ󺸴� ������ �������� ���� ���� ���
		const FVector ForwardDirection = FRotationMatrix(CameraRotation).GetUnitAxis(EAxis::X);

		// ī�޶� �ٶ󺸴� ������ �������� ������ ���� ���
		const FVector RightDirection = FRotationMatrix(CameraRotation).GetUnitAxis(EAxis::Y);

		// �Է°��� ���� ���͸� �����Ͽ� �̵� ���� ���
		FVector DesiredMovementDirection = (ForwardDirection * MovementVector.Y) + (RightDirection * MovementVector.X);
		DesiredMovementDirection.Z = 0.0f;

        // �Է� ���� ����Ͽ� �̵�
		AddMovementInput(DesiredMovementDirection);
	}
}

// �޸��� �Լ�
void AMyCharacter::StartRun()
{
	bIsRun = true;
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
}

void AMyCharacter::StopRun()
{
	bIsRun = false;
	GetCharacterMovement()->MaxWalkSpeed = 400.0f;
}

// ���� �Լ�
void AMyCharacter::Jump()
{
	if (bCanJump && !GetCharacterMovement()->IsFalling()) // ���߿� ���� �ʰ� ���� ������ ����
	{
		Super::Jump();
		bCanJump = false; // ������ �� �� �����Ƿ� ��Ȱ��ȭ
		GetWorld()->GetTimerManager().SetTimer(JumpCooldownTimer, this, &AMyCharacter::ResetJump, 1.1f, false); // 1.1�� ��ٿ�
	}
}

void AMyCharacter::ResetJump()
{
	bCanJump = true; // ��ٿ� �� ���� ���� ���·� ����
}

void AMyCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>(); // ���콺/��Ʈ�ѷ� �Է� �� ��������

	if (springArmComp)
	{
		// ���� SpringArm�� ȸ���� ��������
		FRotator CurrentRotation = springArmComp->GetRelativeRotation();

		// ���콺 �Է� ���� ������� ȸ���� ����
		CurrentRotation.Yaw += LookAxisVector.X; // �¿�(Yaw) ȸ��
		CurrentRotation.Pitch = FMath::Clamp(CurrentRotation.Pitch - LookAxisVector.Y, -20.0f, 40.0f); // ����(Pitch) ȸ�� �� ����

		// ���ŵ� ȸ������ SpringArm�� ����
		springArmComp->SetRelativeRotation(CurrentRotation);
	}
}

void AMyCharacter::HandleStamina(float DeltaTime)
{
	if (bIsRun && CurrentMP > 0)
	{
		CurrentMP -= StaminaDrainRate * DeltaTime;
		if (CurrentMP <= 0)
		{
			CurrentMP = 0;
			StopRun(); // ���¹̳��� 0�� �Ǹ� �ڵ����� ����
		}
	}
	else if (!bIsRun && CurrentMP < MaxMP)
	{
		CurrentMP += StaminaRecoveryRate * DeltaTime;
		if (CurrentMP > MaxMP)
		{
			CurrentMP = MaxMP;
		}
	}
}