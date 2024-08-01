// Menu driven program in CPP to 
// system control using system() 
// function and command with option 
#include <iostream> 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <windows.h> 

void printMenu(); 

// Function to shutdown the computer 
void shutDown() 
{ 
	// Tp clears the screen 
	system("cls"); 
	printf("\nshuttingg down..\n"); 

	// System function call to 
	// shutdown system 
	system("C:\\WINDOWS\\System32\\shutdown /s"); 

	// To clears the screen 
	system("cls"); 
} 

// Function to restart the computer 
void reStart() 
{ 

	// Clears the screen 
	system("cls"); 
	printf("\nRestart in 30 seconds ..."); 

	// System function call to 
	// restart system 
	system("C:\\WINDOWS\\system32\\shutdown /r"); 
} 

// Function to log off user 
void logOff() 
{ 

	// To clears the screen 
	system("cls"); 
	printf("\n Shutting down under 30 seconds... "); 

	// System function call to log off user 
	system("C:\\WINDOWS\\system32\\shutdown /l"); 
} 

// Function to open manualShutdown 
// shutdown dialog box 
void manualShutdown() 
{ 
	// To clears the screen 
	system("cls"); 

	// System function call to manual shutdown 
	system("C:\\WINDOWS\\System32\\shutdown /i"); 
} 

void abortShutdown() 
{ 
	// To clears the screen 
	system("cls"); 

	// System function call to aboart shutdown 
	system("C:\\WINDOWS\\System32\\shutdown /a"); 
} 

// Function to take user choices and perform 
// the appropriate operation 
void selectMenu() 
{ 
	int choice; 
	printf("\n Enter your choice : "); 
	scanf("%d", &choice); 

	switch (choice) { 
	case 1: 
		shutDown(); 
		break; 

	case 2: 
		reStart(); 
		break; 

	case 3: 
		logOff(); 
		break; 

	case 4: 
		manualShutdown(); 
		break; 

	case 5: 
		abortShutdown(); 
		break; 

	case 6: 

		printf("\n Exiting... \n\n"); 
		printf("Exiting in 3 seconds...\n"); 
		Sleep(3000); 
		exit(1); 

	default: 
		printf("\ninvalid choice Try again \n"); 
		printMenu(); 
	} 
} 

// Function to print all the menus 
void printMenu() 
{ 
	// Set output color to blue 
	// background and white foreground 
	system("color 1F"); 
	printf("\n"); 

	// Create Menu 
	printf("\xB2 \xB2\xB2\xB2\xB2\xB2\xB2\xB2"
		"\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2"
		"\xB2\xB2\xB2\xB2\xB2 SYSTEM CONTROL \xB2"
		"\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2"
		"\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2\xB2"
		"\xB2\xB2 \xB2"); 
	printf("\n ______________________________"
		"_________________________________"); 
	printf("\n|\t\t\t\t\t\t\t\t|"); 
	printf("\n|\t\t\t\t\t\t\t\t|"); 
	printf("\n|\t\t\t 1. Shutdown Computer \t\t\t|"); 
	printf("\n|\t\t\t 2. Restart Computer \t\t\t|"); 
	printf("\n|\t\t\t 3. Log off \t\t\t\t|"); 
	printf("\n|\t\t\t 4. Manual Shutdown Settings\t\t|"); 
	printf("\n|\t\t\t 5. Abort Shutdown \t\t\t|"); 
	printf("\n|\t\t\t 6. Exit \t\t\t\t|"); 
	printf("\n|\t\t\t\t\t\t\t\t|"); 
	printf("\n|\t\t\t\t\t\t\t\t|"); 
	printf("\n|\t\t\t\t\t\t\t\t|"); 
	printf("\n|\t\t\t\t\t\t\t\t|"); 
	printf("\n\xB2_________________________________"
		"______________________________\xB2\n"); 

	// Function call for select options 
	selectMenu(); 
} 

// Driver Code 
int main() 
{ 
	// Function Call 
	printMenu(); 
	return 0; 
}
