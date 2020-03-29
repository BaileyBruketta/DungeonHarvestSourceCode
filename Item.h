// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class DUNGEONHARVEST_API Item
{
public:
	Item();
	~Item();

	FString itemID;
	FString itemName;
	int quantity;
	int goldValue;
};
