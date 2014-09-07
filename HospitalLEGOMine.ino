/* Team 1305 - Hospital LEGO Mine
 
 Adapted from Arduino/examples/Blink without delay
 Subsystems are:
 ELEVATOR - skip elevator - run by a directly-connected SERVO
 SHOP - conveyor and other shop-type equip, controlled by a motor-controller
 BORER - vehicle with tunnel borer drill mechanism - controlled by a motor-controller
 GRINDER - vertical gear below LEGO gal with sword - controlled by a motor-controller
 DUMPTRUCK - Dump truck that moves forwards and dumps - controlled by a motor-controller
 CHAINSAW - LEGO motor with chainsaw-like gears that also raises/lowers - controlled by a motor-controller
 CRANE - runs the crane and ore car
 */

#include <Servo.h>

Servo ElevatorServo; // Create Servo object to control the servo
Servo LEDVictor;
Servo BorerVictor;
//Servo ChainSawVictor;
Servo CraneVictor;
Servo DumptruckAndShopVictor;
Servo ChainsawAndRailVictor;


// constants won't change. Used here to 
// set pin numbers:
//const int IO_LED_DIRECT_PIN =  13;      // the number of the LED pin that DOESN't go through the VICTOR
const int IO_LED_W_VICTOR_PIN =  10; //7;
const int IO_ELEVATOR_PWM_OUT_PIN = 6;
const int IO_BORER_PIN = 3;
const int IO_CHAINSAW_PIN = 4;
const int IO_CRANE_PIN = 11;  //is this crane?
const int IO_DUMPTRUCK_AND_SHOP_PIN = 9;
const int IO_CHAINSAW_AND_RAIL_PIN = 5;
const int IO_COIN_SLOT_PIN = 8;
const int IO_SONG_PIN = 7;

const int VICTOR_MAX_SIGNAL = 2000;
const int VICTOR_MIN_SIGNAL = 1000;
const int VICTOR_NEUTRAL_SIGNAL = 1500;
const int VICTOR_ABS_SPEED = VICTOR_MAX_SIGNAL - VICTOR_NEUTRAL_SIGNAL;
  
const int MOTOR_DIRECTION_FORWARD = 1;
const int MOTOR_DIRECTION_REVERSE = -1;
const int MOTOR_DIRECTION_NEUTRAL = 0;

const float LED_VICTOR_PERCENT = 1.0;
const float SHOP_SPEED_PERCENT = 1.0;
const float BORER_SPEED_PERCENT = 0.25;
const float CHAINSAW_SPEED_PERCENT = 0.25;
const float CRANE_SPEED_PERCENT = 0.15;
const float DUMPTRUCK_AND_SHOP_SPEED_PERCENT = 0.5;
const float CHAINSAW_AND_RAIL_SPEED_PERCENT = 0.4;

const unsigned int SERVO_MIN_POS = 0;
const unsigned int SERVO_MAX_POS = 3000;

const unsigned int LED_INTERVAL = 1000;           // interval at which to blink (milliseconds)
const unsigned int ELEVATOR_LIFT_TIME_MS= 4000;           // interval at which to reset servo to min pos (milliseconds)
const unsigned int CHAINSAW_AND_RAIL_CYCLE_TIME_MS = 20000; //cycle before rail car reverses direction
const unsigned int CRANE_CYCLE_TIME_MS = 2500;
  
const float ELEVATOR_TURN_PER_MS = (((float)SERVO_MAX_POS - (float)SERVO_MIN_POS)/(float)ELEVATOR_LIFT_TIME_MS);
const unsigned int TURN_DURATION_MILLISEC = 30000;

// Variables will change:
int CurrentLEDState = LOW;             // LEDState used to set the LED
long LEDChangeStartMillis = 0;        // will store last time LED was updated
long ElevatorChangeStartMillis = 0; //last time servo rotation cycle was restarted
long ChainsawAndRailChangeStartMillis = 0; //last time rail car reversed direction
int CurrentRailDirection = MOTOR_DIRECTION_FORWARD;
int CurrentCraneDirection = MOTOR_DIRECTION_FORWARD;
long CraneChangeStartMillis = 0;

const int COIN_READ_DELAY_MS = 50;
int CoinSlotReading;
const int COIN_DROP_DETECT_VALUE = 0;
boolean TurnHasEnded = true;

// the following three variables is a long because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
unsigned long MaxMillisecThisTurn = 0;  //timer to correct for built-in millis() fcn resetting every 50 days;
unsigned long CurrentMillisecReading = millis();
unsigned long TurnStartTime;
int CurrentNote = 1;
    
void setup() {
  ElevatorServo.attach(IO_ELEVATOR_PWM_OUT_PIN); // Servo is connected to digital pin 9
  LEDVictor.attach(IO_LED_W_VICTOR_PIN);
  BorerVictor.attach(IO_BORER_PIN);
  //ChainSawVictor.attach(IO_CHAINSAW_PIN);
  CraneVictor.attach(IO_CRANE_PIN);
  DumptruckAndShopVictor.attach(IO_DUMPTRUCK_AND_SHOP_PIN);
  ChainsawAndRailVictor.attach(IO_CHAINSAW_AND_RAIL_PIN);
  pinMode(IO_COIN_SLOT_PIN,INPUT);

  Serial.begin(9600);
  Serial.println("Starting");
  // set the digital pin as output:
  //direct LED is likely no longer needed!
//  pinMode(IO_LED_DIRECT_PIN, OUTPUT);      
  //Serial.println(ELEVATOR_TURN_PER_MS);
}

void loop()
{
    // Here is where you'd put code that needs to be running all the time.
    // Do NOT use a FOR or WHILE loop (or other "blocking" code)
    // in any of these called routines.  Just use milli-seconds (via MillisecCounter()) to decide
    // what state/position they should be in, set the motor for that state/pos, and
    // then return control to the main loop quickly so it can call the next subsystem
    if (TurnHasEnded)
    {
      CoinSlotReading = digitalRead(IO_COIN_SLOT_PIN);
      delay(COIN_READ_DELAY_MS);
      Serial.println(CoinSlotReading);
      Serial.println("Sleeping");
      if (CoinSlotReading == COIN_DROP_DETECT_VALUE)
      {
          Serial.println("Turn is starting");
          TurnStartTime = millis();
          TurnHasEnded = false;
      }
    }
    else
    {
      CurrentMillisecReading = millis();
      //Serial.println("Running");
      //Serial.println(CurrentMillisecReading - TurnStartTime);
      if (CurrentMillisecReading - TurnStartTime < TURN_DURATION_MILLISEC)
      {
        RunVictorLED();
        SetElevatorPosition();
        RunBorer();
        //xxRunChainsaw();
        RunCrane();
        RunDumpTruckAndShop();
        RunChainsawAndRail();
        PlayNote();
      }
      else
      {
        TurnHasEnded = true;
      }
    }
}

void PlayNote()
{
//**/
const int P=0; 
const int c = 261;
const int d = 294;
const int e = 329;
const int f = 349;
const int g = 391;
const int gS = 415;
const int a = 440;
const int aS = 455;
const int b = 466;
const int cH = 523;
const int cSH = 554;
const int dH = 587;
const int dSH = 622;
const int eH = 659;
const int fH = 698;
const int fSH = 740;
const int gH = 784;
const int gSH = 830;
const int aH = 880;
  int marioBeats[] ={ 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, //Page 1
    2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1, 1, 2, 4, //Page 2
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2, //Page4
    1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, //Page 5
    1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1.3, 1.3, 1.3, 1.3, 1.3, 1.3, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1.3, 1.3, 1.3, 1, 1, 1, 1, 1, 1, 2   }; //Page 6


  int melody1[] = {a, a, a, f, cH, a, f, cH, a, P, eH, eH, eH, fH, cH, gS, f, cH, a, P, aH, a, a, aH, gSH, gH, fSH, fH, fSH, P, aS, dSH, dH, cSH, cH, b, cH, P};
//    E,E,P,E,P,C,E,P,G,P,P,g,P,P,C,P,P,g,P,e,P,P,a,P,b,P,i,a,P,g,E,G,A,P,F,G,P,E,P,C,D,b,P,C,P,P,g,P,e,P,P,a,P,b,P,i,a,P,g,E,G,A,P,F,G,P,E,P,C,D,b,P,P,G,N,F,R,P,E,P,u,a,C,P,a,C,D,P,G,N,F,R,P,E,P,l,P,l,l,P,P,P,G,N,F,R,P,E,P,u,a,C,P,a,C,D,P,L,P,P,D,P,C,P,P,P,C,C,P,C,P,C,D,P,E,C,P,a,g,P,P,C,C,P,C,P,C,D,E,P,P,C,C,P,C,P,C,D,P,E,C,P,a,g,P,P,E,E,P,E,P,C,E,P,G,P,P,g,P,P,C,P,P,g,P,e,P,P,a,P,b,P,i,a,P,g,E,G,A,P,F,G,P,E,P,C,D,b,P,C,P,P,g,P,e,P,P,a,P,b,P,i,a,P,g,E,G,A,P,F,G,P,E,P,C,D,b,P,E,C,P,g,P,u,P,a,F,P,F,a,P,P,b,A,A,A,G,F,E,C,P,a,g,P,P,E,C,P,g,P,u,P,a,F,P,F,a,P,P,b,F,P,F,F,E,D,C,e,P,e,c,P,P  };

int Starwars[80][2] =   {
  {a, 500},
  {a, 500},    
  {a, 500},
  {f, 350},
  {cH, 150},  
  {a, 500},
  {f, 350},
  {cH, 150},
  {a, 650},
 
  {P, 500},
 
  {eH, 500},
  {eH, 500},
  {eH, 500},  
  {fH, 350},
  {cH, 150},
  {gS, 500},
  {f, 350},
  {cH, 150},
  {a, 650},

  {P, 500},
 
  {aH, 500},
  {a, 300},
  {a, 150},
  {aH, 500},
  {gSH, 325},
  {gH, 175},
  {fSH, 125},
  {fH, 125},    
  {fSH, 250},

  {P, 325},
 
  {aS, 250},
  {dSH, 500},
  {dH, 325},  
  {cSH, 175},  
  {cH, 125},  
  {b, 125},  
  {cH, 250},  
 
   {P, 325},
   
  {f, 250},  
  {gS, 500},  
  {f, 350},  
  {a, 125},
  {cH, 500},
  {a, 375},  
  {cH, 125},
  {eH, 650},
 
  {P, 500}, 
 
  //Repeat second section
    {eH, 500},
  {eH, 500},
  {eH, 500},  
  {fH, 350},
  {cH, 150},
  {gS, 500},
  {f, 350},
  {cH, 150},
  {a, 650},

  {P, 500},
 
  {aH, 500},
  {a, 300},
  {a, 150},
  {aH, 500},
  {gSH, 325},
  {gH, 175},
  {fSH, 125},
  {fH, 125},    
  {fSH, 250},

  {P, 325},
//  secondSection(},
 
  //Variant 2
  {f, 250},  
  {gS, 500},  
  {f, 375},  
  {cH, 125},
  {a, 500},  
  {f, 375},  
  {cH, 125},
  {a, 650},  
 
 {P, 650}  

 };
//       Serial.println(CurrentNote);  
  if (CurrentNote < 75)
  {
       int NoteToPlay = Starwars[CurrentNote][0];
      int NoteLength = Starwars[CurrentNote][ 1];
      Serial.println(NoteLength); 
      Serial.println(NoteToPlay); 
     
     //tone(IO_SONG_PIN,melody1[CurrentNote],120);
     tone(IO_SONG_PIN, NoteToPlay, NoteLength);
     delay(NoteLength);
 
     CurrentNote = CurrentNote + 1;
  }
  else
  {
     noTone(IO_SONG_PIN);  
        Serial.println("done"); 
  }
}

void   RunVictorLED()
{
  ///tbd...  int currentMillis = MillisecCounter();
  //unsigned long currentMillis = millis();
 
  if(CurrentMillisecReading  - LEDChangeStartMillis > LED_INTERVAL) {
    // save the last time you blinked the LED 
    LEDChangeStartMillis = CurrentMillisecReading;   

    // if the LED is off turn it on and vice-versa:
    if (CurrentLEDState == LOW)
    {
      CurrentLEDState = HIGH;
      LEDVictor.writeMicroseconds(CalcVictor(MOTOR_DIRECTION_FORWARD, LED_VICTOR_PERCENT));
    }
    else
    {
      CurrentLEDState = LOW;
      LEDVictor.writeMicroseconds(CalcVictor(MOTOR_DIRECTION_FORWARD, LED_VICTOR_PERCENT * 0.4));
    }
  }
}

//void DropCoin()
//{
//  TurnStartTime = millis();
//}

int MillisecCounter()
{
  // This routine ensures that when millis() overflows a long
  // (at approx 50 days) it doesn't impact ability to determine
  // whether a turn is over.
  // This is the only routine that should use millis() directly.
  // All other routines should call this to determine how man
  // milliseconds we are in to a turn.
  
  //CurrentMillisecReading = millis();
  if (CurrentMillisecReading > MaxMillisecThisTurn)
  {
    MaxMillisecThisTurn = CurrentMillisecReading ;
    return  (int) CurrentMillisecReading - TurnStartTime;
  }
  else
  {
    return (int) MaxMillisecThisTurn - TurnStartTime + CurrentMillisecReading ;
  }  
}

int CalcVictor(int Motor_Direction, float PercentPower)
{
  int motorVoltageCalc; 
  motorVoltageCalc = VICTOR_NEUTRAL_SIGNAL + (Motor_Direction * VICTOR_ABS_SPEED * PercentPower);
  //Serial.println( motorVoltageCalc);
  return motorVoltageCalc; 

}

void RunChainsawAndRail(){
    //  unsigned long currentMillis = millis();
//tbd...    int currentMillis = MillisecCounter();
    int millisecCount = CurrentMillisecReading - ChainsawAndRailChangeStartMillis;

    
  if(millisecCount > CHAINSAW_AND_RAIL_CYCLE_TIME_MS) {
    // save the last time you reset the server to min pos 
    ChainsawAndRailChangeStartMillis = CurrentMillisecReading; 
    millisecCount = 0;
    if (CurrentRailDirection == MOTOR_DIRECTION_FORWARD)
    {
      CurrentRailDirection = MOTOR_DIRECTION_REVERSE;
//      Serial.println(1);
    }
    else
    {
      CurrentRailDirection = MOTOR_DIRECTION_FORWARD;
//      Serial.println(0);
    }
  }
  
    ChainsawAndRailVictor.writeMicroseconds(CalcVictor(CurrentRailDirection, CHAINSAW_AND_RAIL_SPEED_PERCENT));
}

void RunBorer(){
    BorerVictor.writeMicroseconds(CalcVictor(MOTOR_DIRECTION_FORWARD, BORER_SPEED_PERCENT));
    
}

void RunCrane(){
//        unsigned long currentMillis = millis();
//tbd...    int currentMillis = MillisecCounter();
    int millisecCount = CurrentMillisecReading  - CraneChangeStartMillis;

    
  if(millisecCount > CRANE_CYCLE_TIME_MS) {
    // save the last time you reset the server to min pos 
    CraneChangeStartMillis = CurrentMillisecReading; 
    millisecCount = 0;
    if (CurrentCraneDirection == MOTOR_DIRECTION_FORWARD)
    {
      CurrentCraneDirection = MOTOR_DIRECTION_REVERSE;
//      Serial.println(1);
    }
    else
    {
      CurrentCraneDirection = MOTOR_DIRECTION_FORWARD;
//      Serial.println(0);
    }
  }
  float speedMultiplier;
  if (CurrentCraneDirection == MOTOR_DIRECTION_FORWARD)
  {
    speedMultiplier = 1.25;
  }
  else
  {
    speedMultiplier = 0.8;
  }
  //Serial.println(CalcVictor(CurrentCraneDirection, CRANE_SPEED_PERCENT * speedMultiplier ));
    CraneVictor.writeMicroseconds(CalcVictor(CurrentCraneDirection, CRANE_SPEED_PERCENT * speedMultiplier));
}

void RunDumpTruckAndShop(){
    DumptruckAndShopVictor.writeMicroseconds(CalcVictor(MOTOR_DIRECTION_FORWARD, DUMPTRUCK_AND_SHOP_SPEED_PERCENT));
}
  
void SetElevatorPosition()
{
  
  //  unsigned long currentMillis = millis();
//tbd...    int currentMillis = MillisecCounter();
    int millisecCount = CurrentMillisecReading - ElevatorChangeStartMillis;
  
  if(millisecCount > ELEVATOR_LIFT_TIME_MS) {
    // save the last time you reset the server to min pos 
    ElevatorChangeStartMillis = CurrentMillisecReading; 
    millisecCount = 0;
  }
  long ElevatorPos = SERVO_MIN_POS + (millisecCount * ELEVATOR_TURN_PER_MS);
  //Serial.println(ElevatorPos);
  //Serial.println(currentMillis);
  ElevatorServo.writeMicroseconds(ElevatorPos);
}

Enter file contents here
