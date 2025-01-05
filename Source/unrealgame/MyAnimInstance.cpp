// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MyCharacter.h"

void UMyAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    // 애니메이션 초기화 작업
    if (ACharacter* Character = Cast<ACharacter>(TryGetPawnOwner()))
    {
        // 캐릭터의 초기화 작업
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