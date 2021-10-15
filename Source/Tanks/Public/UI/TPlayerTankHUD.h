// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "TPlayerTankHUD.generated.h"

class UTPlayerTankMainWidget;
/**
 * 
 */
UCLASS()
class TANKS_API ATPlayerTankHUD : public AHUD
{
	GENERATED_BODY()

/**
 * Members
 */

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UTPlayerTankMainWidget> TankPlayerMainWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UTPlayerTankMainWidget* TankPlayerMainWidget;



/**
 * Members
 */
protected:

	virtual void BeginPlay() override;

public:

	UFUNCTION(BlueprintCallable)
	UTPlayerTankMainWidget* GetTankPlayerMainWidget() const;

	
};
