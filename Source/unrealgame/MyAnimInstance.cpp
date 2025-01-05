// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MyCharacter.h"

void UMyAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    // �ִϸ��̼� �ʱ�ȭ �۾�
    if (ACharacter* Character = Cast<ACharacter>(TryGetPawnOwner()))
    {
        // ĳ������ �ʱ�ȭ �۾�
        MyCharacter = Cast<AMyCharacter>(Character);
    }
}

void UMyAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
    Super::NativeUpdateAnimation(DeltaTime);

    
    if (MyCharacter)
    {
        Speed = MyCharacter->GetVelocity().Size();
        bIsInAir = MyCharacter->GetCharacterMovement()->IsFalling();
        bIsJumping = MyCharacter->bPressedJump && bIsInAir;
    }
}