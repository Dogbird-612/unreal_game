// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "InputActionValue.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyCharacter.generated.h"

UCLASS()
class UNREALGAME_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

private:
	// IMC
	UPROPERTY(editanywhere, category = input, meta = (allowprivateaccess = true))
	class UInputMappingContext* DefaultMappingContext;
	// IA_Move
	UPROPERTY(editanywhere, category = input, meta = (allowprivateaccess = true))
	class UInputAction* MoveAction;
	//IA_Run
	UPROPERTY(editanywhere, category = input, meta = (allowprivateaccess = true))
	class UInputAction* RunAction;
	// IA_Jump
	UPROPERTY(editanywhere, category = Input)
	class UInputAction* JumpAction;
	// IA_Look
	UPROPERTY(editanywhere, category = input, meta = (allowprivateaccess = true))
	class UInputAction* LookAction;

	bool bCanJump; // 점프 가능 여부

	FTimerHandle JumpCooldownTimer; // 점프 쿨다운을 위한 타이머

public:
	// Sets default values for this character's properties
	AMyCharacter();

public:
	// SpingArm
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* springArmComp;

	// Camera
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* cameraComp;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 위젯 클래스와 인스턴스
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UMyUserWidget> MyUserWidgetClass;

	UPROPERTY()
	UMyUserWidget* MyUserWidgetInstance;

	// HP 및 MP 상태
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stats")
	float CurrentHP;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxHP;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stats")
	float CurrentMP;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxMP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float StaminaDrainRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float StaminaRecoveryRate;

	// 달리기 상태
	bool bIsRun;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Move functions
	void Move(const FInputActionValue& value);

	//Start Run functions
	void StartRun();

	//Stop Run functions
	void StopRun();

	//Jump functions
	void Jump() override;

	//Reset Jump functions
	void ResetJump();

	//Look functions
	void Look(const FInputActionValue& value);

	void HandleStamina(float DeltaTime);
};
