#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "storage.h"

/* 
  definition of storage cell structure ----
  members :
  int building : building number of the destination
  int room : room number of the destination
  int cnt : number of packages in the cell
  char passwd[] : password setting (4 characters)
  char *contents : package context (message string)
*/
typedef struct {
	int building;
	int room;
	int cnt;
	char passwd[PASSWD_LEN+1];
	
	char *context;
} storage_t;


static storage_t** deliverySystem; 			//deliverySystem
static int storedCnt = 0;					//number of cells occupied
static int systemSize[2] = {0, 0};  		//row/column of the delivery system
static char masterPassword[PASSWD_LEN+1];	//master password




// ------- inner functions ---------------

//print the inside context of a specific cell
//int x, int y : cell to print the context
static void printStorageInside(int x, int y) {
	printf("\n------------------------------------------------------------------------\n");
	printf("------------------------------------------------------------------------\n");
	if (deliverySystem[x][y].cnt > 0)
		printf("<<<<<<<<<<<<<<<<<<<<<<<< : %s >>>>>>>>>>>>>>>>>>>>>>>>>>>>\n", deliverySystem[x][y].context);
	else
		printf("<<<<<<<<<<<<<<<<<<<<<<<< empty >>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
		
	printf("------------------------------------------------------------------------\n");
	printf("------------------------------------------------------------------------\n\n");
}

//initialize the storage
//set all the member variable as an initial value
//and allocate memory to the context pointer
//int x, int y : cell coordinate to be initialized
static void initStorage(int x, int y) 
{
	//initialize the information of building, room, context, passwd, and cnt
	int i;
	deliverySystem[x][y].building = 0;
	deliverySystem[x][y].room = 0;
	deliverySystem[x][y].context = 0;
	for(i=0; i<PASSWD_LEN+1; i++)
	{
			deliverySystem[x][y].passwd[i] = 0;
	}
	deliverySystem[x][y].cnt = 0;
	storedCnt--;
}

//get password input and check if it is correct for the cell (x,y)
//int x, int y : cell for password check
//return : 0 - password is matching, -1 - password is not matching
static int inputPasswd(int x, int y) 
{
	//print location of locker to be entered for password
	printf(" - input password for (%d, %d) storage : \n", x, y);
	//input the password of corresponding locker
	char inputpasswd[PASSWD_LEN+1];
	scanf("%s", inputpasswd);
	//check the matching of a password
	if (strcmp(inputpasswd, deliverySystem[x][y].passwd) == 0 || strcmp(inputpasswd, masterPassword) == 0 ) 
	{
		return 0;
	}
	return -1;
}





// ------- API function for main.c file ---------------

//backup the delivery system context to the file system
//char* filepath : filepath and name to write
//return : 0 - backup was successfully done, -1 - failed to backup
int str_backupSystem(char* filepath) 
{
	//open the pile
	FILE *fp;  
	fp=fopen(filepath, "w");
	//backup the file of the locker's systemSize(row, collumn) and masterPassword
	fprintf(fp, "%d %d\n", systemSize[0], systemSize[1]);
	fprintf(fp, "%s\n", masterPassword);
	//backup the corresponding row, column, building, room, password, context which storedCnt is 1
	int i, j;
	for (i=0;i<systemSize[0];i++)
	{
		for (j=0;j<systemSize[1];j++)
		{
			if (deliverySystem[i][j].cnt== 1)
			{
				fprintf(fp, "%d %d %d %d %s %s\n", i, j, deliverySystem[i][j].building, deliverySystem[i][j].room, deliverySystem[i][j].passwd, deliverySystem[i][j].context);
			}
		}
	}
	//close the file
	fclose(fp);
	//check opening is successful
	if (fp == NULL)
	{
		return -1;
	}
	
	return 0;
}



//create delivery system on the double pointer deliverySystem
//char* filepath : filepath and name to read config parameters (row, column, master password, past contexts of the delivery system
//return : 0 - successfully created, -1 - failed to create the system
int str_createSystem(char* filepath) 
{
	//insert variables to help repeat
	int i, j;
	char c;
	//apply corresponding row and column
	int inputrow, inputcolumn;
	//open the pile
	FILE *fp;   
	fp=fopen(filepath, "r");
	//declare the locker's systemSize(row, collumn) and masterPassword
	fscanf(fp, "%d %d", &systemSize[0], &systemSize[1]);
	fscanf(fp, "%s", &masterPassword);
	//allocate the locker's systemSize(row, collumn)
	deliverySystem = (storage_t**)malloc(systemSize[0]*sizeof(storage_t*));
	for(i=0;i<systemSize[0];i++)
	{
		deliverySystem[i]= (storage_t*)malloc(systemSize[1]*sizeof(storage_t));
	}
	//allocate message(context)
	for(i=0;i<systemSize[0];i++)
	{
		for(j=0;j<systemSize[1];j++)
			deliverySystem[i][j].context = (char *)malloc(100 * sizeof(char));
	}
	//allocate existence status(cnt) with 0
	for(i=0;i<systemSize[0];i++)
	{
		for(j=0;j<systemSize[1];j++)
			deliverySystem[i][j].cnt=0;
	}	
	//declare corresponding row, column, building, room, password, context, existence status(cnt) with adding storedCnt
	while((c=fgetc(fp))!=EOF)
	{
		fscanf(fp, "%d %d", &inputrow, &inputcolumn);
		fscanf(fp, "%d %d %s %s", &deliverySystem[inputrow][inputcolumn].building, &deliverySystem[inputrow][inputcolumn].room, deliverySystem[inputrow][inputcolumn].passwd, deliverySystem[inputrow][inputcolumn].context);
		deliverySystem[inputrow][inputcolumn].cnt=1;
		storedCnt++;
	}
	
	//close the file
	fclose(fp);
	//check opening is successful
	if (fp == NULL)
	{
		return -1;
	}
	
	return 0;
}

//free the memory of the deliverySystem 
void str_freeSystem(void) 
{
	int i, j;
	//free message(context)
	for(i=0;i<systemSize[0];i++)
	{
		for(j=0;j<systemSize[1];j++)
			free(deliverySystem[i][j].context);
	}
	//allocate the locker's systemSize(row, collumn)
	free(deliverySystem);
	for(i=0;i<systemSize[0];i++)
	{
		free(deliverySystem[i]);
	}
	//deliverySystem is declared **, how to free system?
	//What is the difference between systemSize which is used and deliverySystem which is not used?
}



//print the current state of the whole delivery system (which cells are occupied and the destination of the each occupied cells)
void str_printStorageStatus(void) {
	int i, j;
	printf("----------------------------- Delivery Storage System Status (%i occupied out of %i )-----------------------------\n\n", storedCnt, systemSize[0]*systemSize[1]);
	
	printf("\t");
	for (j=0;j<systemSize[1];j++)
	{
		printf(" %i\t\t",j);
	}
	printf("\n-----------------------------------------------------------------------------------------------------------------\n");
	
	for (i=0;i<systemSize[0];i++)
	{
		printf("%i|\t",i);
		for (j=0;j<systemSize[1];j++)
		{
			if (deliverySystem[i][j].cnt > 0)
			{
				printf("%i,%i\t|\t", deliverySystem[i][j].building, deliverySystem[i][j].room);
			}
			else
			{
				printf(" -  \t|\t");
			}
		}
		printf("\n");
	}
	printf("--------------------------------------- Delivery Storage System Status --------------------------------------------\n\n");
}



//check if the input cell (x,y) is valid and whether it is occupied or not
int str_checkStorage(int x, int y) {
	if (x < 0 || x >= systemSize[0])
	{
		return -1;
	}
	
	if (y < 0 || y >= systemSize[1])
	{
		return -1;
	}
	
	return deliverySystem[x][y].cnt;	
}


//put a package (msg) to the cell
//input parameters
//int x, int y : coordinate of the cell to put the package
//int nBuilding, int nRoom : building and room numbers of the destination
//char msg[] : package context (message string)
//char passwd[] : password string (4 characters)
//return : 0 - successfully put the package, -1 - failed to put
int str_pushToStorage(int x, int y, int nBuilding, int nRoom, char msg[MAX_MSG_SIZE+1], char passwd[PASSWD_LEN+1]) 
{
	int i;
	//check the success of storing corresponding deliverySystem
	if(deliverySystem[x][y].cnt==0)
	{
		//store the information of building, room, message(context), and passwd with adding cnt and storedCnt
		deliverySystem[x][y].building = nBuilding;
		deliverySystem[x][y].room = nRoom;
		deliverySystem[x][y].context = msg;
		for(i=0; i<PASSWD_LEN+1; i++)
		{
			deliverySystem[x][y].passwd[i] = passwd[i];
		}
		deliverySystem[x][y].cnt++;
		storedCnt++;
		return 0;
	}
	else
		return -1;	
		
	return 0;	

}



//extract the package context with password checking
//after password checking, then put the msg string on the screen and re-initialize the storage
//int x, int y : coordinate of the cell to extract
//return : 0 - successfully extracted, -1 = failed to extract
int str_extractStorage(int x, int y) 
{
	//if password is correct, extract the storage
	if(inputPasswd(x, y) == 0)
	{
		printf(" -----------> extracting the storage (%d, %d)... \n\n", x, y);
		printf("------------------------------------------------------------------------\n");
		printf("------------------------------------------------------------------------\n");
		printf("<<<<<<<<<<<<<<<<<<<<<<<< : %s >>>>>>>>>>>>>>>>>>>>>>>>>>>>\n", deliverySystem[x][y].context);
		printf("------------------------------------------------------------------------\n");
		printf("------------------------------------------------------------------------\n");
		initStorage(x, y);
		return 0;
	}
	else
	//if password is not correct, make fail extracting
	printf(" -----------> password is wrong!!\n");
	return -1;
}

//find my package from the storage
//print all the cells (x,y) which has my package
//int nBuilding, int nRoom : my building/room numbers
//return : number of packages that the storage system has
int str_findStorage(int nBuilding, int nRoom) 
{
	//designate a variable indicating the number of parcels delivered
	int cnt=0;
	//find the package with the buiding and the room number
	int i, j;
	for (i=0;i<systemSize[0];i++)
	{
		for (j=0;j<systemSize[1];j++)
		{
			if (deliverySystem[i][j].building == nBuilding && deliverySystem[i][j].room == nRoom)
			{
				printf(" -----------> Found a package in (%d, %d) \n", i, j);
				cnt++;
			}
		}
	}
	return cnt;
}
