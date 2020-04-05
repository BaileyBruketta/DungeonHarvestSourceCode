// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyBase.generated.h"

UCLASS()
class DUNGEONHARVEST_API AEnemyBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyBase();


	int maxHealth;
	int curHealth;
	int attack;
	int defense;
	int speed;
	int level;
	int ID;
	int currentLocationInteger;
	void SetCurrentLocationInteger(int set);
	int GetCurrentLocationInteger();
	FString nameQ;

	void spawnStats(int levelz, int IDz);





protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
