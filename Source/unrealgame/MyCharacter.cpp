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
	// 기본 이속 설정
	GetCharacterMovement()->MaxWalkSpeed = 400.0f;

	// 공중 방향 전환 설정
	GetCharacterMovement()->AirControl = 1.0f;
	
	// 점프 높이 설정
	GetCharacterMovement()->JumpZVelocity = 500.0f;

	// 캐릭터 회전
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->RotationRate = FRotator(0.0f, 21600.0f, 0.0f);

	// 캐릭터 HP MP 설정
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

	// // Mesh의 기본 위치 및 회전 설정 (필요 시 수정)
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -89.649996f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

	// 위젯 생성 및 화면에 추가
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
		// PlayerCameraManager를 사용하여 Pitch 제한 설정
		if (PlayerController->PlayerCameraManager)
		{
			PlayerController->PlayerCameraManager->ViewPitchMin = -80.0f; // 최소 Pitch
			PlayerController->PlayerCameraManager->ViewPitchMax = 80.0f;  // 최대 Pitch
		}
	}
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// HP/MP 값을 위젯에 전달
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

// 이동 함수
void AMyCharacter::Move(const FInputActionValue& Value)
{
	// 이동에 사용될 방향과 속도를 나타냄
	const FVector2D MovementVector = Value.Get<FVector2D>();
	if (springArmComp && MovementVector.SizeSquared() > 0.0f) // 입력 값이 유효한 경우
	{
		// SpringArm의 상대적 회전 값 가져오기
		FRotator CameraRotation = springArmComp->GetRelativeRotation();
		CameraRotation.Pitch = 0.0f; // Pitch 제거 (이동 방향에는 영향을 주지 않음)
		CameraRotation.Roll = 0.0f;  // Roll 제거 (이동 방향에는 영향을 주지 않음)

		// 카메라가 바라보는 방향을 기준으로 전방 벡터 계산
		const FVector ForwardDirection = FRotationMatrix(CameraRotation).GetUnitAxis(EAxis::X);

		// 카메라가 바라보는 방향을 기준으로 오른쪽 벡터 계산
		const FVector RightDirection = FRotationMatrix(CameraRotation).GetUnitAxis(EAxis::Y);

		// 입력값과 방향 벡터를 조합하여 이동 방향 계산
		FVector DesiredMovementDirection = (ForwardDirection * MovementVector.Y) + (RightDirection * MovementVector.X);
		DesiredMovementDirection.Z = 0.0f;

        // 입력 값을 사용하여 이동
		AddMovementInput(DesiredMovementDirection);
	}
}

// 달리기 함수
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

// 점프 함수
void AMyCharacter::Jump()
{
	if (bCanJump && !GetCharacterMovement()->IsFalling()) // 공중에 있지 않고 점프 가능할 때만
	{
		Super::Jump();
		bCanJump = false; // 점프를 한 번 했으므로 비활성화
		GetWorld()->GetTimerManager().SetTimer(JumpCooldownTimer, this, &AMyCharacter::ResetJump, 1.1f, false); // 1.1초 쿨다운
	}
}

void AMyCharacter::ResetJump()
{
	bCanJump = true; // 쿨다운 후 점프 가능 상태로 복구
}

void AMyCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>(); // 마우스/컨트롤러 입력 값 가져오기

	if (springArmComp)
	{
		// 현재 SpringArm의 회전값 가져오기
		FRotator CurrentRotation = springArmComp->GetRelativeRotation();

		// 마우스 입력 값을 기반으로 회전값 갱신
		CurrentRotation.Yaw += LookAxisVector.X; // 좌우(Yaw) 회전
		CurrentRotation.Pitch = FMath::Clamp(CurrentRotation.Pitch - LookAxisVector.Y, -20.0f, 40.0f); // 상하(Pitch) 회전 및 제한

		// 갱신된 회전값을 SpringArm에 적용
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
			StopRun(); // 스태미나가 0이 되면 자동으로 멈춤
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