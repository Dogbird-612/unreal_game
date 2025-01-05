// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class UNREALGAME_API UMyUserWidget : public UUserWidget
{
	GENERATED_BODY()


public:
    // HP 및 MP Progress Bar에 접근하기 위한 UPROPERTY
    UPROPERTY(meta = (BindWidget))
    class UProgressBar* HPBar;

    UPROPERTY(meta = (BindWidget))
    class UProgressBar* MPBar;

    // HP와 MP 값을 업데이트하는 함수
    UFUNCTION(BlueprintCallable)
    void UpdateHP(float CurrentHP, float MaxHP);

    UFUNCTION(BlueprintCallable)
    void UpdateMP(float CurrentMP, float MaxMP);
};
