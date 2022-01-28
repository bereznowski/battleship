#include <iostream>
#include <string.h>

#define numberOfPlayers 2
#define numberOfShipClasses 4
#define maxNumberOfShipsPerClass 10
#define maxShipLength 5
#define defaultNumberOfCarriers 1
#define defaultNumberOfBattleships 2
#define defaultNumberOfCruisers 3
#define defaultNumberOfDestroyers 4
#define bufferSize 100
#define defaultY1PlayerA 0
#define defaultY2PlayerA 9
#define defaultX1PlayerA 0
#define defaultX2PlayerA 10
#define defaultY1PlayerB 11
#define defaultY2PlayerB 20
#define defaultX1PlayerB 0
#define defaultX2PlayerB 10

using namespace std;

enum Board
{
   emptyField,
   aOccupiesField,
   bOccupiesField,
   destroyedField
};


enum CurrentPlayer
{
   playerA,
   playerB
};


enum ShipClass
{
   carrier,
   battleship,
   cruiser,
   destroyer
};


enum ShipSize
{
   carrierSize = 5,
   battleshipSize = 4,
   cruiserSize = 3,
   destroyerSize = 2
};


struct InitialPosition
{
   int y1, y2, x1, x2;
};


struct Part
{
   int y;
   int x;
   bool destroyed = false;
};


struct Ship
{
   int y;
   int x;
   char direction;
   enum ShipSize size;
   Part** parts;
   bool placedOnBoard = false;
};


struct Player
{
   int* numberOfShips;
   Ship** ships;
   InitialPosition initialPosition;
};


enum Board* createBoard(const int sizeY, const int sizeX);
void allocateMemoryForPlayers(Player** const players);
void setInitialPositionsOfPlayers(Player** const players);
void setFleet(Player** const players, int* const numberOfShipOfClass, const CurrentPlayer currentPlayer);
void executeState(char* const buffer, enum Board* const board, const int sizeY, const int sizeX, Player** const players, enum CurrentPlayer* currentPlayer);
void printBoard(enum Board* const board, const int sizeY, const int sizeX, Player** const players);
void clearBoard(enum Board* const board, const int sizeY, const int sizeX);
void addShipsToBoard(enum Board* const board, const int sizeX, Player** const players);
void assignNextPlayer(enum CurrentPlayer** const currentPlayer);
bool checkPlayerCorrectness(const enum CurrentPlayer currentPlayer, char* const buffer);
void executePlayer(char* const buffer, const int sizeY, const int sizeX, Player** const players, enum CurrentPlayer* const currentPlayer, bool* const errorOccurance);
void placeShip(Player** const players, const enum CurrentPlayer currentPlayer, bool* const errorOccurance);
void changeInitialsToSizeAndClass(char* const shipInitials, enum ShipSize* const shipSize, enum ShipClass* const shipClass);
bool checkIfInPlayerPosition(const int y, const int x, const int partY, const int partX, const char direction, const int shipNumber, char* const shipInitials, Player** const players, const enum CurrentPlayer currentPlayer, bool* const errorOccurance);
bool checkIfAllShipsOfClassAlreadyPresent(const int y, const int x, const char direction, const int shipNumber, char* const shipInitials, const enum ShipClass shipClass, Player** const players, const enum CurrentPlayer currentPlayer, bool* const errorOccurance);
bool checkIfShipAlreadySet(const int y, const int x, const char direction, const int shipNumber, char* const shipInitials, const enum ShipClass shipClass, Player** const players, const enum CurrentPlayer currentPlayer, bool* const errorOccurance);
void shoot(const int sizeY, const int sizeX, Player** const players, bool* const errorOccurance);
bool checkIfShootInBoard(const int y, const int x, const int sizeY, const int sizeX, bool* const errorOccurance);
bool checkIfAllShipsArePlaced(const int y, const int x, Player** const players, bool* const errorOccurance);
bool checkWin(Player** const players, enum CurrentPlayer* const winningPlayer);
void freeAllMemoryForPlayers(Player** const players);


int main()
{
   const int sizeY = 21, sizeX = 10;
   char buffer[bufferSize];
   enum Board* board = createBoard(sizeY, sizeX);
   enum CurrentPlayer currentPlayer = playerA;
   Player** players = (Player**)malloc(sizeof(Player*) * numberOfPlayers);
   bool errorOccurance = false;
   enum CurrentPlayer winningPlayer = currentPlayer;

   allocateMemoryForPlayers(players);
   setInitialPositionsOfPlayers(players);

   for(int i = 0; i < numberOfPlayers; i++) // this loop sets the default fleet size
   {
      int numberOfShipOfClass[numberOfShipClasses] = {defaultNumberOfCarriers, defaultNumberOfBattleships, defaultNumberOfCruisers, defaultNumberOfDestroyers};
      setFleet(players, numberOfShipOfClass, (i == playerA) ? playerA : playerB);
   }
      

   while(cin >> buffer && errorOccurance == false)
   {
      if(strcmp(buffer, "[state]") == 0)
         executeState(buffer, board, sizeY, sizeX, players, &currentPlayer);

      else if(strcmp(buffer, "[playerA]") == 0 || strcmp(buffer, "[playerB]") == 0)
      {
         if(checkPlayerCorrectness(currentPlayer, buffer) == true)
         {
            executePlayer(buffer, sizeY, sizeX, players, &currentPlayer, &errorOccurance);

            if(errorOccurance == true)
               break;
         }
         else
         {
            errorOccurance = true;
            cout << "INVALID OPERATION \"" << buffer << " \": THE OTHER PLAYER EXPECTED" << endl;
            break;
         }

         if(checkWin(players, &winningPlayer) == true)
         {
            cout << ((winningPlayer == playerA) ? 'A' : 'B') << " won";
               break;
         }
      }
   }

   freeAllMemoryForPlayers(players);
   free(players);
   free(board);

   return 0;
}


enum Board* createBoard(const int sizeY, const int sizeX)
{
   enum Board* createdBoard = (enum Board*)malloc((sizeY*sizeX) * sizeof(enum Board));

   for(int i = 0; i < sizeY; i++)
   {
      for(int j = 0; j < sizeX; j++)
      {
         createdBoard[i*sizeX + j] = emptyField;
      }
   }

   return createdBoard;
}


void allocateMemoryForPlayers(Player** const players)
{
   for(int i = 0; i < numberOfPlayers; i++)
   {
      players[i] = (Player*)malloc(sizeof(Player));
      players[i]->numberOfShips = (int*)malloc(sizeof(int) * numberOfShipClasses);
      players[i]->ships = (Ship**)malloc(sizeof(Ship*) * numberOfShipClasses * maxNumberOfShipsPerClass);

      for(int j = 0; j < numberOfShipClasses * maxNumberOfShipsPerClass; j++)
      {
         players[i]->ships[j] = (Ship*)malloc(sizeof(Ship));
         players[i]->ships[j]->parts = (Part**)malloc(sizeof(Part*) * maxShipLength);

         for(int k = 0; k < maxShipLength; k++)
         {
            players[i]->ships[j]->parts[k] = (Part*)malloc(sizeof(Part));
         }
      }
   }
}


void setInitialPositionsOfPlayers(Player** const players)
{
   for(int i = 0; i < numberOfPlayers; i++)
   {
      players[i]->initialPosition.y1 = (i == playerA) ? defaultY1PlayerA : defaultY1PlayerB;
      players[i]->initialPosition.y2 = (i == playerA) ? defaultY2PlayerA : defaultY2PlayerB;
      players[i]->initialPosition.x1 = (i == playerA) ? defaultX1PlayerA : defaultX1PlayerB;
      players[i]->initialPosition.x2 = (i == playerA) ? defaultX2PlayerA : defaultX2PlayerB;
   }
}


void setFleet(Player** const players, int* const numberOfShipOfClass, const CurrentPlayer currentPlayer)
{
   for(int i = 0; i < numberOfShipClasses; i++)
   {
      players[currentPlayer]->numberOfShips[i] = numberOfShipOfClass[i];
      for(int j = 0; j < numberOfShipOfClass[i]; j++)
      {
         switch (i)
         {
         case carrier:
            players[currentPlayer]->ships[i * maxNumberOfShipsPerClass + j]->size = carrierSize;
            break;

         case battleship:
            players[currentPlayer]->ships[i * maxNumberOfShipsPerClass + j]->size = battleshipSize;
            break;

         case cruiser:
            players[currentPlayer]->ships[i * maxNumberOfShipsPerClass + j]->size = cruiserSize;
            break;

         case destroyer:
            players[currentPlayer]->ships[i * maxNumberOfShipsPerClass + j]->size = destroyerSize;
            break;
         
         default:
            break;
         }
      }
   }
}


void executeState(char* const buffer, enum Board* const board, const int sizeY, const int sizeX, Player** const players, enum CurrentPlayer* currentPlayer)
{
   cin >> buffer;

   while(strcmp(buffer, "[state]") != 0)
   {
      if(strcmp(buffer, "PRINT") == 0)
      {
         int printParameter;
         
         cin >> printParameter;
         
         printBoard(board, sizeY, sizeX, players);
      }

      else if(strcmp(buffer, "SET_FLEET") == 0)
      {
         char playerLetter;
         int numberOfShipOfClass[numberOfShipClasses];

         cin >> playerLetter;

         for(int i = 0; i < numberOfShipClasses; i++)
            cin >> numberOfShipOfClass[i];

         setFleet(players, numberOfShipOfClass, (playerLetter == 'A') ? playerA : playerB);
      }

      else if(strcmp(buffer, "NEXT_PLAYER") == 0)
         assignNextPlayer(&currentPlayer);

      cin >> buffer;
   }
}


void printBoard(enum Board* const board, const int sizeY, const int sizeX, Player** const players)
{
   clearBoard(board, sizeY, sizeX);
   addShipsToBoard(board, sizeX, players);

   int occupiedFieldsA = 0, occupiedFieldsB = 0;

   for(int i = 0; i < sizeY; i++)
   {
      for(int j = 0; j < sizeX; j++)
      {
         if (board[i*sizeX + j] == emptyField)
         cout << ' ';

         else if (board[i*sizeX + j] == aOccupiesField)
         {
            cout << '+';
            occupiedFieldsA++;
         }
         
         else if (board[i*sizeX + j] == bOccupiesField)
         {
            cout << '+';
            occupiedFieldsB++;
         }
         else if (board[i*sizeX + j] == destroyedField)
         {
            cout << 'x';
         }
         
      }
      cout << endl;
   }

   cout << "PARTS REMAINING:: A : " << occupiedFieldsA << " B : " << occupiedFieldsB << endl;
}


void clearBoard(enum Board* const board, const int sizeY, const int sizeX)
{
   for(int i = 0; i < sizeY; i++)
   {
      for(int j = 0; j < sizeX; j++)
      {
         board[i*sizeX + j] = emptyField;
      }
   }
}


void addShipsToBoard(enum Board* const board, const int sizeX, Player** const players)
{
   // these loops iterate over all ships for both players and add already placed ships (part by part) to the board with appropriate coordinates
   for(int i = 0; i < numberOfPlayers; i++)
   {
      for(int j = 0; j < numberOfShipClasses; j++)
      {
         int numberOfShipsInClass = players[i]->numberOfShips[j];

         for(int k = 0; k < numberOfShipsInClass; k++)
         {
            if(players[i]->ships[j * maxNumberOfShipsPerClass + k]->placedOnBoard == true)
            {
               enum CurrentPlayer shipOwner = (i == playerA) ? playerA : playerB;
               enum ShipSize shipSize = players[i]->ships[j * maxNumberOfShipsPerClass + k]->size;

               for(int l = 0; l < shipSize; l++)
               {
                  Part* currentPart = players[i]->ships[j * maxNumberOfShipsPerClass + k]->parts[l];
                  int x = currentPart->x;
                  int y = currentPart->y;

                  board[y*sizeX + x] = ((currentPart->destroyed == true) ? destroyedField : (shipOwner == playerA) ? aOccupiesField : bOccupiesField);
               }
            }
         }
      }
   }
}


void assignNextPlayer(enum CurrentPlayer** const currentPlayer)
{
   char playerLetter;

   cin >> playerLetter;

   switch (playerLetter)
   {
   case 'A':
      **currentPlayer = playerA;
      break;
   
   case 'B':
      **currentPlayer = playerB;
      break;
   }
}


bool checkPlayerCorrectness(const enum CurrentPlayer currentPlayer, char* const buffer)
{
   if ((buffer[7] == 'A' && currentPlayer == playerA) || (buffer[7] == 'B' && currentPlayer == playerB))
      return true;
   else
      return false;
}


void executePlayer(char* const buffer, const int sizeY, const int sizeX, Player** const players, enum CurrentPlayer* const currentPlayer, bool* const errorOccurance)
{
   char passedBuffer[bufferSize];
   strcpy(passedBuffer, buffer);

   cin >> buffer;

   while(strcmp(buffer, passedBuffer) != 0)
   {
      if(strcmp(buffer, "PLACE_SHIP") == 0)         
         placeShip(players, *currentPlayer, errorOccurance);
      if(strcmp(buffer, "SHOOT") == 0)
         shoot(sizeY, sizeX, players, errorOccurance);

      if(*errorOccurance == true)
         return;

      cin >> buffer;
   }

   *currentPlayer = (*currentPlayer == playerA) ? playerB : playerA;
}


void placeShip(Player** const players, const enum CurrentPlayer currentPlayer, bool* const errorOccurance)
{
   int y, x, shipNumber;
   char direction, shipInitials[3];
   enum ShipClass shipClass;
   enum ShipSize shipSize;

   cin >> y >> x >> direction >> shipNumber >> shipInitials;

   changeInitialsToSizeAndClass(shipInitials, &shipSize, &shipClass);

   if(checkIfAllShipsOfClassAlreadyPresent(y, x, direction, shipNumber, shipInitials, shipClass, players, currentPlayer, errorOccurance) == true)
      return;
   if(checkIfShipAlreadySet(y, x, direction, shipNumber, shipInitials, shipClass, players, currentPlayer, errorOccurance) == true)
      return;

   // assignment of the coordinates and size for the whole ship
   players[currentPlayer]->ships[shipClass * maxNumberOfShipsPerClass + shipNumber]->y = y;
   players[currentPlayer]->ships[shipClass * maxNumberOfShipsPerClass + shipNumber]->x = x;
   players[currentPlayer]->ships[shipClass * maxNumberOfShipsPerClass + shipNumber]->size = shipSize;

   // this loop assigns the coordinates and the destruction status (not distroyed) to the parts of a newly placed ship
   for(int i = 0; i < shipSize; i++)
   {
      players[currentPlayer]->ships[shipClass * maxNumberOfShipsPerClass + shipNumber]->parts[i]->destroyed = false;

      switch (direction)
      {
      case 'N':
         {
            if(checkIfInPlayerPosition(y, x, y+i, x, direction, shipNumber, shipInitials, players, currentPlayer, errorOccurance) == false)
               return;
            else
            {
               players[currentPlayer]->ships[shipClass * maxNumberOfShipsPerClass + shipNumber]->parts[i]->y = y+i;
               players[currentPlayer]->ships[shipClass * maxNumberOfShipsPerClass + shipNumber]->parts[i]->x = x;
            }
         }
         break;

      case 'S':
         {
            if(checkIfInPlayerPosition(y, x, y-i, x, direction, shipNumber, shipInitials, players, currentPlayer, errorOccurance) == false)
               return;
            else
            {
               players[currentPlayer]->ships[shipClass * maxNumberOfShipsPerClass + shipNumber]->parts[i]->y = y-i;
               players[currentPlayer]->ships[shipClass * maxNumberOfShipsPerClass + shipNumber]->parts[i]->x = x;
            }
         }
         break;

      case 'W':
         {
            if(checkIfInPlayerPosition(y, x, y, x+i, direction, shipNumber, shipInitials, players, currentPlayer, errorOccurance) == false)
               return;
            else
            {
               players[currentPlayer]->ships[shipClass * maxNumberOfShipsPerClass + shipNumber]->parts[i]->y = y;
               players[currentPlayer]->ships[shipClass * maxNumberOfShipsPerClass + shipNumber]->parts[i]->x = x+i;
            }
         }
         break;
      
      case 'E':
         {
            if(checkIfInPlayerPosition(y, x, y, x-i, direction, shipNumber, shipInitials, players, currentPlayer, errorOccurance) == false)
               return;
            else
            {
               players[currentPlayer]->ships[shipClass * maxNumberOfShipsPerClass + shipNumber]->parts[i]->y = y;
               players[currentPlayer]->ships[shipClass * maxNumberOfShipsPerClass + shipNumber]->parts[i]->x = x-i;
            }
         }
         break;

      default:
         break;
      }
   }

   players[currentPlayer]->ships[shipClass * maxNumberOfShipsPerClass + shipNumber]->placedOnBoard = true;
}


void changeInitialsToSizeAndClass(char* const shipInitials, enum ShipSize* const shipSize, enum ShipClass* const shipClass)
{
   if (strcmp(shipInitials, "CAR") == 0) 
   {
      *shipSize = carrierSize;
      *shipClass = carrier;
   }
      
   else if (strcmp(shipInitials, "BAT") == 0)
   {
      *shipSize = battleshipSize;
      *shipClass = battleship;
   }
   else if (strcmp(shipInitials, "CRU") == 0)
   {
      *shipSize = cruiserSize;
      *shipClass = cruiser;
   }
   else
   {
      *shipSize = destroyerSize;
      *shipClass = destroyer;
   }
}


bool checkIfInPlayerPosition(const int y, const int x, const int partY, const int partX, const char direction, const int shipNumber, char* const shipInitials, Player** const players, const enum CurrentPlayer currentPlayer, bool* const errorOccurance)
{
   if(partY < players[currentPlayer]->initialPosition.y1 || partY > players[currentPlayer]->initialPosition.y2 || partX < players[currentPlayer]->initialPosition.x1 || partX > players[currentPlayer]->initialPosition.x2)
   {
      *errorOccurance = true;
      cout << "INVALID OPERATION \"PLACE_SHIP " << y << " " << x << " " << direction << " " << shipNumber << " " << shipInitials << "\": NOT IN STARTING POSITION";
      return false;
   }
   else
      return true;
}


bool checkIfAllShipsOfClassAlreadyPresent(const int y, const int x, const char direction, const int shipNumber, char* const shipInitials, const enum ShipClass shipClass, Player** const players, const enum CurrentPlayer currentPlayer, bool* const errorOccurance)
{
   if(shipNumber >= players[currentPlayer]->numberOfShips[shipClass])
   {
      *errorOccurance = true;
      cout << "INVALID OPERATION \"PLACE_SHIP " << y << " " << x << " " << direction << " " << shipNumber << " " << shipInitials << "\": ALL SHIPS OF THE CLASS ALREADY SET";
      return true;
   }
   else
      return false;
}


bool checkIfShipAlreadySet(const int y, const int x, const char direction, const int shipNumber, char* const shipInitials, const enum ShipClass shipClass, Player** const players, const enum CurrentPlayer currentPlayer, bool* const errorOccurance)
{
   if(players[currentPlayer]->ships[shipClass * maxNumberOfShipsPerClass + shipNumber]->placedOnBoard == true)
   {
      *errorOccurance = true;
      cout << "INVALID OPERATION \"PLACE_SHIP " << y << " " << x << " " << direction << " " << shipNumber << " " << shipInitials << "\": SHIP ALREADY PRESENT";
      return true;
   }
   else
      return false;
}


void shoot(const int sizeY, const int sizeX, Player** const players, bool* const errorOccurance)
{
   int y, x;
   
   cin >> y >> x;

   if(checkIfAllShipsArePlaced(y, x, players, errorOccurance) == false)
      return;
   if(checkIfShootInBoard(y, x, sizeY, sizeX, errorOccurance) == false)
      return;

   // these loops iterate over all parts of the already placed ships and check if their coordinates are the same as coordinates of the SHOOT
   for(int i = 0; i < numberOfPlayers; i++)
   {
      for(int j = 0; j < numberOfShipClasses; j++)
      {
         int numberOfShipsInClass = players[i]->numberOfShips[j];

         for(int k = 0; k < numberOfShipsInClass; k++)
         {
            if(players[i]->ships[j * maxNumberOfShipsPerClass + k]->placedOnBoard == true)
            {
               enum CurrentPlayer shipOwner = (i == playerA) ? playerA : playerB;
               enum ShipSize shipSize = players[i]->ships[j * maxNumberOfShipsPerClass + k]->size;

               for(int l = 0; l < shipSize; l++)
               {
                  Part* currentPart = players[shipOwner]->ships[j * maxNumberOfShipsPerClass + k]->parts[l];
                  int partX = currentPart->x;
                  int partY = currentPart->y;

                  if(x == partX && y == partY)
                     currentPart->destroyed = true;
               }
            }
         }
      }
   }
}


bool checkIfShootInBoard(const int y, const int x, const int sizeY, const int sizeX, bool* const errorOccurance)
{
   if(y < 0 || y >= sizeY || x < 0 || x >= sizeX)
   {
      *errorOccurance = true;
      cout << "INVALID OPERATION \"SHOOT " << y << " " << x << "\": FIELD DOES NOT EXIST";
      return false;
   }
   else
      return true;
}

bool checkIfAllShipsArePlaced(const int y, const int x, Player** const players, bool* const errorOccurance)
{
   for(int i = 0; i < numberOfPlayers; i++)
   {
      for(int j = 0; j < numberOfShipClasses; j++)
      {
         int numberOfShipsInClass = players[i]->numberOfShips[j];

         for(int k = 0; k < numberOfShipsInClass; k++)
            {
               if((players[i]->ships[j * maxNumberOfShipsPerClass + k]->placedOnBoard) != true)
               {
                  *errorOccurance = true;
                  cout << "INVALID OPERATION \"SHOOT " << y << " " << x << "\": NOT ALL SHIPS PLACED";
                  return false;
               }
            }
      }
   }

   return true;
}


bool checkWin(Player** const players, enum CurrentPlayer* const winningPlayer)
{
   int numberOfWorkingParts[numberOfPlayers] = {0, 0};

   // these loops iterate over all parts of all ships (placed and not placed) and check if there are some undamaged parts for both players
   for(int i = 0; i < numberOfPlayers; i++)
   {
      for(int j = 0; j < numberOfShipClasses; j++)
      {
         int numberOfShipsInClass = players[i]->numberOfShips[j];

         for(int k = 0; k < numberOfShipsInClass; k++)
         {
            enum ShipSize shipSize = players[i]->ships[j * maxNumberOfShipsPerClass + k]->size;

            for(int l = 0; l < shipSize; l++)
            {
               if(players[i]->ships[j * maxNumberOfShipsPerClass + k]->parts[l]->destroyed != true)
                  numberOfWorkingParts[i]++;
            }
         }
      }
   }

   if(numberOfWorkingParts[playerA] == 0)
   {
      *winningPlayer = playerB;
      return true;
   }
   else if(numberOfWorkingParts[playerB] == 0)
   {
      *winningPlayer = playerA;
      return true;
   }     
   else
      return false;
}


void freeAllMemoryForPlayers(Player** const players)
{
   for(int i = 0; i < numberOfPlayers; i++)
   {
      for(int j = 0; j < numberOfShipClasses * maxNumberOfShipsPerClass; j++)
      {
         for(int k = 0; k < maxShipLength; k++)
         {
            free(players[i]->ships[j]->parts[k]);
         }

         free(players[i]->ships[j]->parts);
         free(players[i]->ships[j]);
      }

      free(players[i]->ships);
      free(players[i]->numberOfShips);
      free(players[i]);
   }
}