/* Main Program */
#define BASE_PORT ((short) 0x378)
#define RELAY1_ON 100//0x2 //Data Port 1 @0-7
#define RELAY2_ON 1000 //Data Port 5
#define RELAY_OFF 0
#define TIME_PER_ANGLE 2
#define INI_VELOCITY 20
#define ACC_DUE_GRAVITY 9.8
#define STATUS BASE_PORT+1
#define CONTROL BASE_PORT+2
#define RANGE 40.816
#define PI 3.14159265
#define EOL "\n"

/* Linux Includes */
/*
#include <stdio.h>
#include <stdlib.h> // For exit
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/io.h> //asm/io.h
#include <time.h>
#include <math.h>
*/

/*   Known to build successfully with the following
 *   compilers:
 * 
 *  [ gcc-MinGW32 ]
 *  gcc.exe ./inpout32_test.c  ./inpout32.c  -oTEST -I"."
 * 
 *  [ Borland C++ 5.5 ]
 *  BCC32 -I. inpout32_test.c inpout32.c
 * 
 *  [ Microsoft Visual C++ v6 ]
 *  CL -I. inpout32_test.c inpout32.c
 */
/* Windows Includes */
#include <stdio.h>
#include <conio.h>
#include <windows.h> // For Sleep
#include <process.h>
#include <time.h>
#include <math.h>
#include "inpout32.h"

//Debug
/*
extern int inpout32_init(void);
extern void inpout32_unload(void);
extern short Inp32(short portaddr);
extern void Out32(short portaddr,short datum);
*/
/* Function Declarations */
int getport();
int switch_relay(int,int);
void reset();
int validate_range(float);
void attack();
void sleep_d(unsigned int mseconds);


/* Global Variables */
int resetted=1; /* Normally at 0 degrees */
int attacked=0; /* Are we mad to fire at 0 degrees */
int stat=0;
int exitted=0;
int menu_item=0;
float target_range=0;
int keypress=0;


int main() {
  HINSTANCE hLib;
  printf("\nProgram controller @aatish...\n");
  printf("\nGetting Port Access\n");
  // Attempt to initialize the interface
  if (inpout32_init() != 0) {
    
    fprintf(stderr,
	    "ERROR: Failed to initialize Inpout32 interface!\n");
    
    exit (-1);
  }
  printf("\nPort Access Gained successfully\n");
  while(exitted !=1) {
	switch_relay(1,0);
	switch_relay(2,0);
    printf("\nType '1' for reset\nType '2' for attack\nType 3 for exiting\n>>>");
    scanf("%d",&menu_item);
    if(menu_item==2) {
      printf("\nEnter target distance:\n>>>");
      scanf("%f",&target_range);
      if(!validate_range(target_range)) {
	printf("\nRange invalidated\n");
	continue;
      }
      if(resetted==1) {
	attack();
	continue;
      }
      else
	printf("You need to reset first\n");
	continue;
    }
    else if(menu_item==1){
      printf("Now resetting\n");
      reset();
      continue;
    }
    else {
      exitted=1;
      break;
    }
  }
  inpout32_unload();
}

int getport() {
  /* For linux */
  /*
  if(ioperm(BASE_PORT,3,1)) {
    printf("Couldn't get port BASE_PORT \n");
    return 0;
  }
  return 1;
  */
  /* For Windows */
  /*
  */
}

int switch_relay(relay_no,status) {
  if(relay_no==1) {
    if(status==1) {
      Out32(BASE_PORT,RELAY1_ON); //outb_p
      return 1;
    }
    else if(status==0) {
      Out32(BASE_PORT,RELAY_OFF);
      return 1;
    }
  }
  if(relay_no==2) {
    if(status==1) {
      Out32(BASE_PORT,RELAY2_ON);
      return 1;
    }
    else if(status==0) {
      Out32(BASE_PORT,RELAY_OFF);
      return 1;
    }
  }
}

int validate_range(float a){
  if(a>=RANGE || a<=0.) {
    return 0;
  }
  return 1;
}

void attack() {
  float timetaken;
  timetaken= ((asin((target_range*ACC_DUE_GRAVITY)/(INI_VELOCITY*INI_VELOCITY))/2) * TIME_PER_ANGLE) * (180/PI);
  //Debug:
  printf("\nDebug:Timetaken=%f\n",timetaken);
  printf("\nRadians=%f and Degrees=%f\n",(asin((target_range*ACC_DUE_GRAVITY)/(INI_VELOCITY*INI_VELOCITY))/2),(asin((target_range*ACC_DUE_GRAVITY)/(INI_VELOCITY*INI_VELOCITY))/2)*180/PI);
  switch_relay(1,1);
  Sleep(timetaken*1000);
  /*
   Declaration on POSIX compliant systems
    
   sleep(time_in_second)
   
   Declaration on windows system 
   
   Sleep(time_in_milisecond)
  */
  switch_relay(1,0);
  attacked=1;
  resetted=0;
}

void reset() {
  printf("\nPress \"a\" when on horizon:\n");
  switch_relay(2,1);
  while(1) {
    if((keypress=getch()) == 'a') {
       break;
    }
  }
  switch_relay(2,0);
  resetted=1;
  attacked=0;
}
void sleep_d(unsigned int mseconds)
{
    clock_t goal = mseconds + clock();
    while (goal > clock());
}

