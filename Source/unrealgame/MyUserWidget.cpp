// Fill out your copyright notice in the Description page of Project Settings.


#include "MyUserWidget.h"
#include "Components/ProgressBar.h"

void UMyUserWidget::UpdateHP(float CurrentHP, float MaxHP)
{
    if (HPBar)
    {
        float HPPercent = CurrentHP / MaxHP;
        HPBar->SetPercent(HPPercent);
    }
}

void UMyUserWidget::UpdateMP(float CurrentMP, float MaxMP)
{
    if (MPBar)
    {
        float MPPercent = CurrentMP / MaxMP;
        MPBar->SetPercent(MPPercent);
    }
}