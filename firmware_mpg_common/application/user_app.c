/**********************************************************************************************************************
File: user_app.c                                                                

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app as a template:
 1. Copy both user_app.c and user_app.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "user_app" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "UserApp" with "YourNewTaskName"
 6. Use ctrl-h to find and replace all instances of "USER_APP" with "YOUR_NEW_TASK_NAME"
 7. Add a call to YourNewTaskNameInitialize() in the init section of main
 8. Add a call to YourNewTaskNameRunActiveState() in the Super Loop section of main
 9. Update yournewtaskname.h per the instructions at the top of yournewtaskname.h
10. Delete this text (between the dashed lines) and update the Description below to describe your task
----------------------------------------------------------------------------------------------------------------------

Description:
This is a user_app.c file template 

------------------------------------------------------------------------------------------------------------------------
API:

Public functions:


Protected System functions:
void UserAppInitialize(void)
Runs required initialzation for the task.  Should only be called once in main init section.

void UserAppRunActiveState(void)
Runs current task state.  Should only be called once in main loop.


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserAppFlags;                       /* Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */

extern u8 G_au8DebugScanfBuffer[];                     /* From debug.c */
extern u8 G_u8DebugScanfCharCount;                     /* From debug.c  */

/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp_StateMachine;            /* The state machine function pointer */
static u32 UserApp_u32Timeout;                      /* Timeout counter used across states */

static u8 UserApp_au8UserInputBuffer[USER_INPUT_BUFFER_SIZE];  /* Char buffer */

static u8 CursorPosition;
/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions                                                                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------*/
/* Protected functions                                                                                                */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
Function: UserAppInitialize

Description:
Initializes the State Machine and its variables.

Requires:
  -

Promises:
  - 
*/
void UserAppInitialize(void)
{
  LCDClearChars(LINE1_START_ADDR,20);
  
  LedOn(LCD_RED);
  LedOff(LCD_GREEN);
  LedOn(LCD_BLUE);

  u8 au8Message[] = "     Liuweifeng";
  LCDMessage(LINE1_START_ADDR, au8Message);
  
  static u8 u8BufferMessage[]   = "\n\rBuffer contents:\n\r";
  DebugPrintf(u8BufferMessage);
  
  for(u8 i = 0; i < USER_INPUT_BUFFER_SIZE; i++)
  {
    UserApp_au8UserInputBuffer[i] = '\0';
  }

  CursorPosition = 0;

  /* If good initialization, set state to Idle */
  if( 1 )
  {
    UserApp_StateMachine = UserAppSM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp_StateMachine = UserAppSM_FailedInit;
  }

} /* end UserAppInitialize() */


/*----------------------------------------------------------------------------------------------------------------------
Function UserAppRunActiveState()

Description:
Selects and runs one iteration of the current state in the state machine.
All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
  - State machine function pointer points at current state

Promises:
  - Calls the function to pointed by the state machine function pointer
*/
void UserAppRunActiveState(void)
{
  UserApp_StateMachine();

} /* end UserAppRunActiveState */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for a message to be queued */
static void UserAppSM_Idle(void)
{
  /*Initialize some variables and arrays*/
  static u8 u8NumCharsMessage[] = "\n\rTotal characters received: ";
  static u8 u8NextLine[]={"\n\r"};
  static u8 u8Clearcount[]={"\n\rCharacter count has been cleared!\n\r"};
  static u8 u8CurrentLetter[]={"\n\rThe current letter:\n\r"};
  static u8 u8counter=0;
  static u8 u8counter1=0;
  static u8 u8counter2=0;
  static u8 u8NameIndex=0;
  static u8 u8InputNameIndex=0;
  static u8 u8InputNameCounter=0;
  static u8 u8InputName[200];
  static u8 u8InputMyNameCounter[10];
  static u8 u8TimeCounter[10];
  static u8 u8MyName[]={"Liuweifeng"};
  static u8 flag=FALSE;
  static u8 u8CharCount;
  static u32 u32InputMessagecounter=0;
  u8counter++;
  /*Delay 50ms*/
  if(u8counter==50)
  {
    /*According to the flag to turn off LED and BUZZER*/
    if(flag)
    {
      u8counter1++;
      if(u8counter1==100)
      {
        flag=FALSE;
        u8counter1=0;
        LedOff(RED);
        PWMAudioOff(BUZZER1);
      }
    }
    u8counter=0;
    u8CharCount = DebugScanf(UserApp_au8UserInputBuffer);
    UserApp_au8UserInputBuffer[u8CharCount] = '\0'; 
    if(u8CharCount>0)
    {
      
      u32InputMessagecounter++;
      /*Clear screen*/ 
      if(CursorPosition==20)
      {
        CursorPosition=0;
        LCDClearChars(LINE2_START_ADDR, 20);
      }
      /*Output what you Input*/
      LCDMessage(LINE2_START_ADDR+CursorPosition , UserApp_au8UserInputBuffer);
      CursorPosition++; 
      /*Monitor each character as it arrives and compare it to the letter of your name.As each letter
      arrived that matches the next letter of your name,store the letter in a buffer.*/
      if(UserApp_au8UserInputBuffer[0]==u8MyName[u8NameIndex]||
         (UserApp_au8UserInputBuffer[0]-32)==u8MyName[u8NameIndex]||
         (UserApp_au8UserInputBuffer[0]+32)==u8MyName[u8NameIndex] )
      {   
        u8InputName[u8InputNameIndex]=UserApp_au8UserInputBuffer[0];
        u8NameIndex++;
        u8InputNameIndex++;
        if(u8NameIndex==10)
        {
          u8NameIndex=0;
        }
        /*When has inputed your name's total characters.*/
        if(u8InputNameIndex%10==0)
        {
          LCDClearChars(LINE2_START_ADDR, 20);
          CursorPosition=10;
          LCDMessage(LINE2_START_ADDR, u8MyName);
          CursorPosition=CursorPosition+2;
          u8InputNameCounter++;
          u8InputMyNameCounter[0]=(u8InputNameCounter/100)+48;
          u8InputMyNameCounter[1]=(u8InputNameCounter%100/10)+48;
          u8InputMyNameCounter[2]=(u8InputNameCounter%10)+48;
          u8InputMyNameCounter[3]='\0';  
          if(u8InputMyNameCounter[0]=='0')
          {
            if(u8InputMyNameCounter[1]=='0')
            {
              u8TimeCounter[0]=u8InputMyNameCounter[2];
              u8counter2=1;
            }
            else
            {
              u8TimeCounter[0]=u8InputMyNameCounter[1];
              u8TimeCounter[1]=u8InputMyNameCounter[2];
              u8counter2=2;
            }
          }
          else
          {
            u8TimeCounter[0]=u8InputMyNameCounter[0];
            u8TimeCounter[1]=u8InputMyNameCounter[1];
            u8TimeCounter[2]=u8InputMyNameCounter[2];
            u8counter2=3;
          }          
          /*Whenever all of the letter of your name have been entered run a second task that makes some
          sound, blinks some LEDs,and outputs your name and a counter with how many times your name has been detected.*/
          LCDMessage(LINE2_START_ADDR+CursorPosition, u8TimeCounter);
          LCDMessage(LINE2_START_ADDR+CursorPosition+u8counter2, "time");
          CursorPosition=CursorPosition+u8counter2+5;
          PWMAudioOn(BUZZER1);
          PWMAudioSetFrequency(BUZZER1, 500);
          LedBlink(RED, LED_2HZ);
          flag=TRUE;
        }
      }
    }
  }
  /*BUTTON0 clears the line of text so the next character stars from the beginning.*/
  if(WasButtonPressed(BUTTON0))
  {
    ButtonAcknowledge(BUTTON0);
    CursorPosition=0;
    LCDClearChars(LINE2_START_ADDR, 20);
  }
  /*BUTTON1 print the total number of character count and report a message.*/
  if(WasButtonPressed(BUTTON1))
  {
    ButtonAcknowledge(BUTTON1);
    
    /* Read the buffer and print the contents */
    DebugPrintf(u8NumCharsMessage);
    DebugPrintNumber(u32InputMessagecounter);
    DebugPrintf(u8NextLine);
  }
  /*BUTTON2 clear the total character count and reports a message.*/
   if(WasButtonPressed(BUTTON2))
  {
    ButtonAcknowledge(BUTTON2);
    u32InputMessagecounter=0;
    DebugPrintf(u8Clearcount);
  }
  /*BUTTON3 prints the current letter buffer that is storing your name.*/
  if(WasButtonPressed(BUTTON3))
  {
    ButtonAcknowledge(BUTTON3);
    DebugPrintf(u8CurrentLetter);
    DebugPrintf(u8InputName);
    DebugPrintf(u8NextLine);
  }
  
} /* end UserAppSM_Idle() */
     

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserAppSM_Error(void)          
{
  
} /* end UserAppSM_Error() */


/*-------------------------------------------------------------------------------------------------------------------*/
/* State to sit in if init failed */
static void UserAppSM_FailedInit(void)          
{
    
} /* end UserAppSM_FailedInit() */


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
