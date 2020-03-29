// Fill out your copyright notice in the Description page of Project Settings.

#include "Camera/CameraComponent.h"
#include "TimerManager.h"
#include "Engine.h"
#include "math/UnrealMathUtility.h"
#include "EnemyBase.h"
#include "GameController.h"

// Sets default values
AGameController::AGameController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	

}

// Called when the game starts or when spawned
void AGameController::BeginPlay()
{
	Super::BeginPlay();

	//set internal acknowledgement of player direction
	playerDirection = "north";

	//This is how we get the coordinates for the dungeon tiles.
	//Tile right: add to Y, tile up: add to X;
	DungeonRoot = FVector(0, -5000, 10);	
	FVector trueRoot = FVector(-250, -5250, 10);
	dungeonTileVectors.Init(DungeonRoot, 625);
	borderTileVectors.Init(trueRoot, 600);
	int i = 0;
	for (i; i < 25; i++) 
	{
		int x=0;
		for (x; x < 25; x++) 
		{
			FVector newTileCoord = DungeonRoot;
			newTileCoord.Y += (50 * x);
			newTileCoord.X += (50 * i);
			int TileReference = x + (25 * i);
			dungeonTileVectors[TileReference] = newTileCoord;
		}
	}
	//initialize the aray that holds pointers to tiles
	dungeonTileActors.Init(player, 625);
	borderTileActors.Init(player, 600);
	//initialize the boolean which knows whether a tile is to be wall or floor 
	isWall.Init(true, 625);
	isRoom.Init(false, 625);
	enemyLoca.Init(DungeonRoot, 50);
	enemyList.Init(hack, 50);
	enemyPresent.Init(false, 50);
	isOccupied.Init(false, 625);
	SetBorderVertices();
	//test
	GenerateFloor();
	
}
//Calculates values for the border of the dungeon map
void AGameController::SetBorderVertices()
{
	FVector boardRoot = FVector(-250,-5250,0);
	int x = 0;
	while(x<31)
	{
		int i = 0;
		while (x < 36)
		{
			if (x < 5) 
			{
				int plchdlr = 0;
				for (plchdlr; plchdlr < 35; plchdlr++)
				{
					
					//int z = i + (x * 30);
					borderTileVectors[i].Y += 50 * plchdlr;
					borderTileVectors[i].X += 50 * x;
					i += 1;
				}
				//x += 1;
			}
		
			else if (x > 4)
			{
				if (x < 30)
				{
					int plchldr = 0;
					for (plchldr; plchldr < 10; plchldr++)
					{
						int rowValue = i;
						rowValue -= 175;
						while (rowValue > 9)
						{
							rowValue -= 10;
						}

						if (rowValue < 5)
						{
							borderTileVectors[i].Y += 50 * plchldr;
							borderTileVectors[i].X += 50 * x;
						}
						else if (rowValue > 4)
						{
							borderTileVectors[i].Y += (50 * plchldr) + 1250;
							borderTileVectors[i].X += 50 * x;
						}
						i += 1;
					}
				}


				else if (x > 29)
				{
					if (x < 34)
					{
						int plchdlr = 0;
						for (plchdlr; plchdlr < 35; plchdlr++)
						{
							//int z = i + (x * 30);
							borderTileVectors[i].Y += 50 * plchdlr;
							borderTileVectors[i].X += 50 * x;
							i += 1;
						}
					}

				}
			}

			x += 1;
		}
	}
	//GenerateFloor();
}

// Called every frame
void AGameController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

// *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** THIS IS HOW MOVEMENT WORKS *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** ***
//input from player via hud buttons -> hud casts to GameController instance and calls Turn with an int argument-> Turn calls Movement with an int arg -> movement checks int for direction.........
//.......movement checks if player is facing corresponding direction. if not, it turns the player. if so, movement calls a PlayerMoveN/S/W/E, and sets a timer to call it again -> PMNSWE calls "MoveFunction".....
//.......which moves the player; player moves in 2 separate movements, spaced apart by .05 seconds. this simulates walking. 
// *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *

//catches input from player 
void AGameController::Turn(int inputx)
{
	int turn = inputx;
	//up input received
	if      (turn == 0) { Movement(0); }
	//down input recieved
	else if (turn == 1) { Movement(1); }
	//left input recieved
	else if (turn == 2) { Movement(2); }
	//right input recieved
	else if (turn == 3) { Movement(3); }

}
//this is used by multiple functions, and is a way to check if the tile the player or enemy "wishes" to move to is Open for moving
bool AGameController::canMove(int mapLoc, int direction)
{
	int spaceToCheck;
	//0 = north, 1 = south, 2 = west, 3 = east
	if (direction == 0)	     {spaceToCheck = mapLoc + 25;	}
	else if (direction == 1) {spaceToCheck = mapLoc - 25;	}
	else if (direction == 2) {spaceToCheck = mapLoc - 1;	}
	else if (direction == 3) {spaceToCheck = mapLoc + 1;	}

	if (spaceToCheck < 625) 
	{
		if (isOccupied[spaceToCheck] == false) { if (isWall[spaceToCheck] == false) { return true; } else return false; }
		else return false;
	}
	else return false;
}
//manages player movement
//TODO: turn player proper direction
void AGameController::Movement(int mov) 
{
	int moveArg = mov;
	if (moveArg == 0)
	{
		if (playerDirection == "north") 
		{			
			if (canMove(CurPlaLocInt, 0) == true) 
			{
				MoveFunction(0, player);
				GetWorld()->GetTimerManager().SetTimer(movementTimer, this, &AGameController::PlayerMoveNorth, .05f, false);
				isOccupied[CurPlaLocInt] = false;
				CurPlaLocInt += 25;
			}
		}
		else if (playerDirection != "north") 
		{
			playerDirection = "north";
		}
	}
	else if (moveArg == 1)
	{
		if (playerDirection == "south")
		{
			if (canMove(CurPlaLocInt, 1) == true) 
			{
				MoveFunction(1, player);
				GetWorld()->GetTimerManager().SetTimer(movementTimer, this, &AGameController::PlayerMoveSouth, .05f, false);
				isOccupied[CurPlaLocInt] = false;
				CurPlaLocInt -= 25;
			}
		}
		else if (playerDirection != "south")
		{
			playerDirection = "south";
		}
	}
	else if (moveArg == 2)
	{
		if (playerDirection == "west") 
		{
			if (canMove(CurPlaLocInt, 2) == true) 
			{
				MoveFunction(2, player);
				GetWorld()->GetTimerManager().SetTimer(movementTimer, this, &AGameController::PlayerMoveWest, .05f, false);
				isOccupied[CurPlaLocInt] = false;
				CurPlaLocInt -= 1;
			}
		}
		else if (playerDirection != "west") 
		{
			playerDirection = "west";
		}
	}
	else if (moveArg == 3)
	{
		if (playerDirection == "east")
		{
			if (canMove(CurPlaLocInt, 3) == true) 
			{
				MoveFunction(3, player);
				GetWorld()->GetTimerManager().SetTimer(movementTimer, this, &AGameController::PlayerMoveEast, .05f, false);
				isOccupied[CurPlaLocInt] = false;
				CurPlaLocInt += 1;
			}
		}
		else if (playerDirection != "east")
		{
			playerDirection = "east";
		}
	}

}
//Used by movement timers to direct player movement. this accounts for the second "step", or is called 3 seconds after a first movement, so that the player has its movement divided into a 2 part sequence
void AGameController::PlayerMoveNorth() { MoveFunction(0, player); }
void AGameController::PlayerMoveSouth() { MoveFunction(1, player); }
void AGameController::PlayerMoveWest() { MoveFunction(2, player); }
void AGameController::PlayerMoveEast() { MoveFunction(3, player); }

//catches direction and gamepiece for movement
//TODO: turn actor direction they are moving
//TODO: REmove duplicate code
void AGameController::MoveFunction(int direction, AActor* gamepiece)
{
	//north
	if (direction == 0) 
	{
		FVector loc    = gamepiece->GetActorLocation();
		FVector newLoc = loc;
		newLoc.X       = newLoc.X + 25;
		gamepiece->SetActorLocation(newLoc);
		
	}
	//south
	if (direction == 1)
	{
		FVector loc    = gamepiece->GetActorLocation();
		FVector newLoc = loc;
		newLoc.X       = newLoc.X - 25;
		gamepiece->SetActorLocation(newLoc);
	}
	//west
	if (direction == 2)
	{
		FVector loc    = gamepiece->GetActorLocation();
		FVector newLoc = loc;
		newLoc.Y       = newLoc.Y - 25;
		gamepiece->SetActorLocation(newLoc);
	}
	//east
	if (direction == 3) 
	{
		FVector loc    = gamepiece->GetActorLocation();
		FVector newLoc = loc;
		newLoc.Y       = newLoc.Y + 25;
		gamepiece->SetActorLocation(newLoc);
	}
	
	updateCamera();

}

void AGameController::updateCamera() 
{
	FVector camLoc  = camera->GetActorLocation();
	FVector playLoc = player->GetActorLocation();
	camLoc.X        = playLoc.X;
	camLoc.Y        = playLoc.Y;
	camera->SetActorLocation(camLoc);
}
//******************************************************************************************************************************************************************************************************************************
//This deals with the aspects of generating the floors of the dungeons. Rooms, walls, corridors.
//This generates the XY coordinate data for a 3-8 room dungeon, on a 25x25 plane, and includes hallways between rooms by connecting the room "root" of a room to the root of that before it with a one-tile wide hallway  
void AGameController::GenerateFloor()
{
	//this first variable is used to see if we have hit the second room yet, necessary for building hallways 
	int roomWeAreCalculating = 0;
	FVector PreviousRoomRoot = DungeonRoot;
	int PrevRoomRootInt = 0;
	//number of rooms
	int noRooms = FMath::FRandRange(3,8);
	int i = 0;
	//for every room
	for (i; i < noRooms; i++)
	{
		//pick a starting tile, possible room length and width 
		int roomRoot = FMath::FRandRange(0, 624);
		int roomLeng = FMath::FRandRange(3, 8);
		int roomWidt = FMath::FRandRange(3, 8);

		//isolate to single row 
		int rowLoca = roomRoot;
		while (rowLoca > 24) 
		{			rowLoca = rowLoca - 25;		}

		//test for room (length)
		int testrow = rowLoca + roomLeng;
		int numbMov = 0;
		while (testrow > 24) 
		{			numbMov += 1;			testrow = testrow -= 1;		}

		//move roomRoot left so that it may have room to expand to full length
		roomRoot = roomRoot - numbMov;

		//isolate for northbound limit
		int rowHite = roomRoot;
		int romPeak = rowHite + (roomWidt*25);
		int numMoDo = 0;
		while (romPeak > 624) 
		{			numMoDo += 1;			romPeak -= 25;		}

		//move roomRoot down
		roomRoot = roomRoot - (25 * numMoDo);

		//flip dunegon tile bools so that our abstract rooms are represented by a set of booleans linked to tile vectors and actor pointers
		//TODO : add another boolean such that we may mark the rooms as rooms and leave hallways unmarked, that we might fill rooms with appropriate drops and enemies 
		int ii = 0;
		int ll = roomWidt + 1;
		for (ii; ii < ll; ii++) 
		{
			int zz = 0;
			int l = roomLeng + 1;
			for (zz; zz < l; zz++) 
			{
				int Roomtoget = roomRoot + zz + (25 * ii);
				isWall[Roomtoget] = false;
				isRoom[Roomtoget] = true;
			}
		}

		//this makes hallways 
		if (roomWeAreCalculating != 0) 
		{
			int thisRoomRoot = roomRoot;
			int pastRoomRoot = PrevRoomRootInt;
			int heightThis = 0;
			int heightPrev = 0;

			while (PrevRoomRootInt > 24) { heightPrev += 1;	PrevRoomRootInt = PrevRoomRootInt - 25; }
			while (thisRoomRoot > 24)    { heightThis += 1;	thisRoomRoot = thisRoomRoot - 25; }

			int diff = thisRoomRoot - PrevRoomRootInt;
			int corner = roomRoot - diff; //this is a capture of which integer value is the corner of this hallway 

			//This creates hallways from left to right 
			if (diff > 0)      { while (diff > 0) { int hw = roomRoot - diff; isWall[hw] = false; diff += -1; } }
			else if (diff < 0) { while (diff < 0) { int hw = roomRoot - diff; isWall[hw] = false; diff += 1; } }

			//This creates hallways from north to south 
			int diff2 = heightThis - heightPrev;
			if (diff2 > 0) { while (diff2 > 0) { int hw = corner - (25 * diff2); isWall[hw] = false; diff2 -= 1; } }
			if (diff2 < 0) { while (diff2 < 0) { int hw = corner - (25 * diff2); isWall[hw] = false;  diff2 += 1; } }
		}

		PreviousRoomRoot = dungeonTileVectors[roomRoot];
		PrevRoomRootInt = roomRoot;
		roomWeAreCalculating += 1;
	}
	PopulateFloor(0,0);
}
//******************************************************************************************************************************************************************************************************************************
//This takes the abstract data for a dungeon floor plan, and spawns in walls, floor tiles
//TODO : Allow for an argument to be passed, such that we may refer to a faux-database to select individually styled tiles , and another function 
void AGameController::PopulateFloor(int dungeonNumber, int dungeonFloor)
{
	TSubclassOf<class AActor> wallType;
	TSubclassOf<class AActor> FloorType;

	//set appropriate tiles to use

	//WildWood Forest
	if (dungeonNumber == 0) 
	{
		FloorType = wallToUse;
		wallType = wall2;
	}



	//room tiles
	int x = 0;
	for (x; x < 625; x++)
	{
		//if the tile is not a wall, spawn a floor piece
		if (isWall[x] == false) 
		{
			FActorSpawnParameters SpawnParams;
			FVector SpawnPoint = dungeonTileVectors[x];
			FRotator SpawnRot;
			AActor* newTile = GetWorld()->SpawnActor<AActor>(FloorType, SpawnPoint, SpawnRot, SpawnParams);
		}
		//if the tile is a wall, spawn a wall piece
		else if (isWall[x] == true) 
		{
			FActorSpawnParameters SpawnParams;
			FVector SpawnPoint = dungeonTileVectors[x];
			FRotator SpawnRot;
			AActor* newTile = GetWorld()->SpawnActor<AActor>(wallType, SpawnPoint, SpawnRot, SpawnParams);
		}
	}

	//border piece (also wall piece)
	int zy = 0;
	for (zy; zy < 600; zy++) 
	{
		FActorSpawnParameters SpawnParams;
		FVector SpawnPoint = borderTileVectors[zy];
		FRotator SpawnRot;
		SpawnRot.Yaw = 0;
		SpawnRot.Pitch = 0;
		SpawnRot.Roll = 0;
		AActor* newTile = GetWorld()->SpawnActor<AActor>(wallType, SpawnPoint, SpawnRot, SpawnParams);
	}

	SetEnemyStats(dungeonNumber, dungeonFloor);
	
}
//******************************************************************************************************************************************************************************************************************************
//spawns enemies based on dungeon number and dungeon floor
void AGameController::SetEnemyStats(int dunj, int floor)
{
	int NumberOfEnemiesToStart = 0;
	

	TArray<int> enemyID;
	TArray<int> enemyLev;

	//custom settings for enemy type and levels - this dictates which dungeons host which enemies, includes an element of controlled randomization 
	//each dungeon will have different enemies and levels; a simialr stucture exists for choosing the terrain for dungeon floors
	if (dunj == 0) 
	{
		NumberOfEnemiesToStart = FMath::RandRange(1, 4);
		enemyID.Init(0, NumberOfEnemiesToStart);
		enemyLev.Init(0, NumberOfEnemiesToStart);

		int z = 0;
		for (z; z < NumberOfEnemiesToStart; z++) 
		{
			int L = FMath::RandRange(0, 10);
			if (L < 3) 
			{
				enemyID[z] = 1;
				enemyLev[z] = FMath::RandRange(1, 5);
			}
			else
			{
				enemyID[z] = 2;
				enemyLev[z] = FMath::RandRange(1, 5);
			}
		}
	}

	//this takes data generated in previous lines and spawns enemies of corresponding type and level, stashes their reference in an array of enemies with arrays for locations and "present/none"  status
	int i = 0;
	for (i; i < NumberOfEnemiesToStart; i++)
	{
		int startPoint = 0;
		startPoint = FMath::RandRange(0, 624);
		while (isRoom[startPoint] == false) { if (startPoint < 624) { startPoint += 1; } else { startPoint -= 1; } }
		enemyLoca[i] = dungeonTileVectors[startPoint];
		isOccupied[startPoint] = true;
		FActorSpawnParameters SpawnParams;
		FRotator SpawnRot;
		FVector SpawnPoint = enemyLoca[i];
		AEnemyBase* newEnemy = GetWorld()->SpawnActor<AEnemyBase>(enemyClass, SpawnPoint, SpawnRot, SpawnParams);
		enemyList[i] = newEnemy;

		enemyList[i]->spawnStats(enemyLev[i], enemyID[i]);
		enemyPresent[i] = true;

		
	}
	TeleportToDungeon();
}

//This puts the player piece inside the dungeon
void AGameController::TeleportToDungeon()
{
	int entrance = 0;
	entrance = FMath::RandRange(0, 624);
	while (isRoom[entrance] == false) {if (entrance < 624) { entrance += 1; }else { entrance -= 1; }}

	while(isOccupied[entrance] ==true){ while (isRoom[entrance] == false) { if (entrance < 624) { entrance += 1; } else { entrance -= 1; } } }

	currentplayerLocation = dungeonTileVectors[entrance];
	CurPlaLocInt = entrance;
	player->SetActorLocation(currentplayerLocation);
	isOccupied[CurPlaLocInt] = true;
	updateCamera();



}

