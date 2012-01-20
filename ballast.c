/* Main Program */
#define BASE_PORT 0x378
#define RELAY1_ON 0xC0 //Data Port 1 @0-7
#define RELAY2_ON 0x84 //Data Port 5
#define RELAY_OFF 0
#define TIME_PER_ANGLE 2
#define INI_VELOCITY 20
#define ACC_DUE_GRAVITY 9.8
#define STATUS BASE_PORT+1
#define CONTROL BASE_PORT+2
#define RANGE 40.816
#define PI 3.14159265

/* Linux Includes */
#include <stdio.h>
#include <stdlib.h> // For exit
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/io.h> //asm/io.h
#include <time.h>
#include <math.h>


/* Windows Includes */
/*
#include <stdio.h>
#include <conio.h>
#include <dos.h>
#include <process.h>
#include <time.h>
#include <math.h>
*/

/* Function Declarations */
int getport();
int switch_relay(int,int);
void reset();
int validate_range(int);
void attack();


/* Global Variables */
int resetted=1; /* Normally at 0 degrees */
int attacked=0; /* Are we mad to fire at 0 degrees */
int stat=0;
int exitted=0;
int menu_item=0;
int target_range=0;
int keypress=0;


int main() {
  printf("Program controller @aatish...\n");
  printf("Getting Port Access\n");
  stat=getport();
  if(!stat){
    printf("Cannot Get Access to Port!!! Exiting\n");
    return 0;
  }
  printf("Port Access Gained successfully\n");
  while(exitted !=1) {
    printf("Type '1' for reset\nType '2' for attack\nType anything other than 1 or 2 for exiting\n>>>");
    scanf("%d",&menu_item);
    if(menu_item==2) {
      printf("\nEnter target distance:");
      scanf("%d",&target_range);
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
}

int getport() {
  /* For linux */
  if(ioperm(BASE_PORT,3,1)) {
    printf("Couldn't get port BASE_PORT \n");
    return 0;
  }
  return 1;
  
  /* For Windows */
  /*
  */
}

int switch_relay(relay_no,status) {
  if(relay_no==1) {
    if(status==1) {
      outb(RELAY1_ON,BASE_PORT); //outb_p
      return 1;
    }
    else if(status==0) {
      outb(RELAY_OFF,BASE_PORT);
      return 1;
    }
  }
  if(relay_no==2) {
    if(status==1) {
      outb(RELAY2_ON,BASE_PORT);
      return 1;
    }
    else if(status==0) {
      outb(RELAY_OFF,BASE_PORT);
      return 1;
    }
  }
}

int validate_range(a){
  if(a>=RANGE || a<=0) {
    return 0;
  }
}

void attack() {
  clock_t t1,t2;
  int starttime,timetaken,currenttime,timeelasped;
  timetaken= (int) (asin((target_range*ACC_DUE_GRAVITY)/(INI_VELOCITY*INI_VELOCITY)) * TIME_PER_ANGLE)* (PI/180);
  //Debug:
  printf("\nDebug:Starttime=%d Timetake=%d Current time=%d Timeelasped=%d\n",starttime,timetaken,currenttime,timeelasped);
  switch_relay(1,1);
  t1=clock();
  starttime= t1 / CLOCKS_PER_SEC;
  while(1) {
    t2=clock();
    currenttime= t2 / CLOCKS_PER_SEC;
    timeelasped=currenttime-starttime;
    //Debug:
    printf("\nDebug:Starttime=%d Timetake=%d Current time=%d Timeelasped=%d\n",starttime,timetaken,currenttime,timeelasped);
    
    if(timeelasped>=timetaken) {
      switch_relay(1,0);
      attacked=1;
      resetted=0;
      break;
    }
  }
}

void reset() {
  printf("Press \"a\" when on horizon:\n");
  switch_relay(2,1);
  while(1) {
    if((keypress=getchar()) =='a') {
      switch_relay(2,0);
      resetted=1;
      attacked=0;
      break;
    }
  }
}

