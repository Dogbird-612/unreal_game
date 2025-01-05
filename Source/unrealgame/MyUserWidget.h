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
    // HP �� MP Progress Bar�� �����ϱ� ���� UPROPERTY
    UPROPERTY(meta = (BindWidget))
    class UProgressBar* HPBar;

    UPROPERTY(meta = (BindWidget))
    class UProgressBar* MPBar;

    // HP�� MP ���� ������Ʈ�ϴ� �Լ�
    UFUNCTION(BlueprintCallable)
    void UpdateHP(float CurrentHP, float MaxHP);

    UFUNCTION(BlueprintCallable)
    void UpdateMP(float CurrentMP, float MaxMP);
};
