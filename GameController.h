// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyBase.h"
#include "Item.h"
#include "GameController.generated.h"


UCLASS()
class DUNGEONHARVEST_API AGameController : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGameController();

	UFUNCTION(BlueprintCallable, Category = "Player")
		void Turn(int inputx);

	void Movement(int mov);
	void MoveFunction(int direction, AActor* gamepiece);

	void PlayerMoveNorth();
	void PlayerMoveSouth();
	void PlayerMoveWest();
	void PlayerMoveEast();

	void updateCamera();

	FString playerDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
		AActor* player;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
		AActor* camera;

	FTimerHandle movementTimer;

	int playerGold;
	int playerHealthMax;
	int playerHealthCurrent;
	int playerStomachMax;
	int playerStomachCurrent;

	TArray<Item> spawnedItems;

	FVector DungeonRoot;
	TArray<FVector> dungeonTileVectors;
	TArray<FVector> borderTileVectors;
	TArray<AActor*> dungeonTileActors;
	TArray<AActor*> borderTileActors;
	TArray<bool> isWall;
	TArray<bool> isRoom;
	//this is used to generate the room/wall/corridor schema 
	void GenerateFloor();
	//this is used to populate the floor of a dungeon
	void PopulateFloor(int dungeonNumber, int dungeonFloor);
	void TeleportToDungeon();

	void SetBorderVertices();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "mapbuilding")
		TSubclassOf<class AActor> wallToUse;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "mapbuilding")
		TSubclassOf<class AActor> wall2;

	AActor* floorToUse;
	AActor* foliagePrim;
	AActor* foliageSeco;

	void SetEnemyStats(int dunj, int floor);
	TArray<AEnemyBase* > enemyList;
	TArray<FVector> enemyLoca;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "enemy")
		TSubclassOf<class AEnemyBase> enemyClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "enemy")
		AEnemyBase* hack;
	TArray<bool> enemyPresent;

	//this is essential for seeing if a player can walk in a specified direction, if things can spawn in, etc
	TArray<bool> isOccupied;
	FVector currentplayerLocation;
	int CurPlaLocInt;

	bool canMove(int mapLoc, int direction);



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
