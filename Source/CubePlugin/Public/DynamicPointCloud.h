// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LidarPointCloud.h"
#include "LidarPointCloudActor.h"
#include "LidarPointCloudComponent.h"
#include "DynamicPointCloud.generated.h"

/**
 * 
 */
UCLASS()
class CUBEPLUGIN_API ADynamicPointCloud : public AGameModeBase
{
	GENERATED_BODY()

public:
	ADynamicPointCloud();
	virtual ~ADynamicPointCloud() = default;



	
};
