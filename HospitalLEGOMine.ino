/* Team 1305 - Hospital LEGO Mine
 
 Adapted from Arduino/examples/Blink without delay
 Subsystems are:
 ELEVATOR - skip elevator - run by a directly-connected SERVO
 SHOP - conveyor and other shop-type equip
 BORER - vehicle with tunnel borer drill mechanism
 GRINDER - vertical gear below LEGO gal with sword
 DUMPTRUCK - Dump truck that moves forwards and dumps
 CHAINSAW - LEGO motor with chainsaw-like gears that also raises/lowers
 CRANE - swings the crane
 RAIL - worm gear that runs ore car
 */

#include <Servo.h>

Servo ElevatorServo; // Create Servo object to control the servo
Servo LEDVictor;
Servo BorerVictor;
Servo CraneVictor;
Servo DumpTruckAndShopVictor;
Servo ChainsawAndRailVictor;


// constants won't change. 
// set pin numbers:
const int IO_LED_W_VICTOR_PIN =  10; 
const int IO_ELEVATOR_PWM_OUT_PIN = 6;
const int IO_BORER_PIN = 3;
const int IO_CHAINSAW_PIN = 4;
const int IO_CRANE_PIN = 11;  
const int IO_DUMPTRUCK_AND_SHOP_PIN = 9;
const int IO_CHAINSAW_AND_RAIL_PIN = 5;
const int IO_COIN_SLOT_PIN = 13;
const int IO_SONG_PIN = 7;

//most motors cannot go at 100% of 5 volts.  These vars "scale" them
const float LED_VICTOR_PERCENT = 1.0;
const float SHOP_SPEED_PERCENT = 1.0;
const float BORER_SPEED_PERCENT = 0.25;
const float CHAINSAW_SPEED_PERCENT = 0.25;
const float CRANE_SPEED_PERCENT = 0.15;
const float DUMPTRUCK_AND_SHOP_SPEED_PERCENT = 0.5;
const float CHAINSAW_AND_RAIL_SPEED_PERCENT = 0.4;

const int VICTOR_MAX_SIGNAL = 2000;
const int VICTOR_MIN_SIGNAL = 1000;
const int VICTOR_NEUTRAL_SIGNAL = 1500;
const int VICTOR_ABS_SPEED = VICTOR_MAX_SIGNAL - VICTOR_NEUTRAL_SIGNAL;
  
const int MOTOR_DIRECTION_FORWARD = 1;
const int MOTOR_DIRECTION_REVERSE = -1;
const int MOTOR_DIRECTION_NEUTRAL = 0;

const unsigned int SERVO_MIN_POS = 0;
const unsigned int SERVO_MAX_POS = 3000;

const unsigned int ELEVATOR_LIFT_DUR_MS= 4000;           // interval at which to reset servo to min pos (milliseconds)
const unsigned int CHAINSAW_AND_RAIL_CYCLE_DUR_MS = 20000; //cycle before rail car reverses direction
const unsigned int CRANE_CYCLE_DUR_MS = 2500;
  
const float ELEVATOR_TURN_PER_MS = (((float)SERVO_MAX_POS - (float)SERVO_MIN_POS)/(float)ELEVATOR_LIFT_DUR_MS);
const unsigned int TURN_DURATION_MILLISEC = 30000;
const int COIN_READ_DELAY_MS = 5;
const int COIN_DROP_DETECT_VALUE = 1;

const int P=0, c = 261, d = 294, e = 329, f = 349, fS = 372, g = 391, gS = 415, a = 440, aS = 455, bF = 466, b = 493, cH = 523, cSH = 554, 
          dH = 587, dSH = 622, eH = 659, fH = 698, fSH = 740, gH = 784, gSH = 830, aH = 880, i=466,  N=740,  R=622,  u=415,  l=1046,  L=622,  k=227;

const int marioBeats[293] ={ 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, //Page 1
    2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1, 1, 2, 4, //Page 2
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2, //Page4
    1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, //Page 5
    1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1.3, 1.3, 1.3, 1.3, 1.3, 1.3, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1.3, 1.3, 1.3, 1, 1, 1, 1, 1, 1, 2   }; //Page 6
const int marioTune[294] =  {
    eH,eH,P,eH,P,cH,eH,P,gH,P,P,g,P,P,cH,P,P,g,P,e,P,P,a,P,b,P,i,a,P,g,eH,gH,aH,P,fH,gH,P,eH,P,cH,dH,bF,P,cH,P,P,g,P,e,P,P,a,P,bF,P,i,a,P,g,eH,gH,aH,P,fH,gH,P,eH,P,cH,dH,bF,P,P,gH,N,fH,R,P,eH,P,u,a,cH,P,a,cH,dH,P,gH,N,fH,R,P,eH,P,l,P,l,l,P,P,P,gH,N,fH,R,P,eH,P,u,a,cH,P,a,cH,dH,P,L,P,P,dH,P,cH,P,P,P,cH,cH,P,cH,P,cH,dH,P,eH,cH,P,a,g,P,P,cH,cH,P,cH,P,cH,dH,eH,P,P,cH,cH,P,cH,P,cH,dH,P,eH,cH,P,a,g,P,P,eH,eH,P,eH,P,cH,eH,P,gH,P,P,g,P,P,cH,P,P,g,P,e,P,P,a,P,bF,bF,P,i,a,P,g,eH,gH,aH,P,fH,gH,P,eH,P,cH,dH,bF,P,cH,P,P,g,P,e,P,P,a,P,bF,P,i,a,P,g,eH,gH,aH,P,fH,gH,P,eH,P,cH,dH,bF,P,eH,cH,P,g,P,u,P,a,fH,P,fH,a,P,P,bF,aH,aH,aH,gH,fH,eH,cH,P,a,g,P,P,eH,cH,P,g,P,u,P,a,fH,P,fH,a,P,P,bF,fH,P,fH,fH,eH,dH,cH,e,P,e,c,P,P  };

const String STARWARS_SONG = "Starwars Song";

const int Starwars[80][2] =   {
  {a, 500}, {a, 500}, {a, 500}, {f, 350}, {cH, 150}, {a, 500}, {f, 350}, {cH, 150}, {a, 650}, {P, 500}, 
   {eH, 500}, {eH, 500}, {eH, 500}, {fH, 350}, {cH, 150}, {gS, 500}, {f, 350}, {cH, 150}, {a, 650}, {P, 500}, 
   {aH, 500}, {a, 300}, {a, 150}, {aH, 500}, {gSH, 325}, {gH, 175}, {fSH, 125}, {fH, 125}, {fSH, 250}, {P, 325}, 
   {aS, 250}, {dSH, 500}, {dH, 325}, {cSH, 175}, {cH, 125}, {b, 125}, {cH, 250}, {P, 325}, 
  {f, 250}, {gS, 500}, {f, 350}, {a, 125}, {cH, 500}, {a, 375}, {cH, 125}, {eH, 650}, {P, 500},  
  //Repeat second section
  {eH, 500}, {eH, 500}, {eH, 500}, {fH, 350}, {cH, 150}, {gS, 500}, {f, 350}, {cH, 150}, {a, 650}, {P, 500}, 
  {aH, 500}, {a, 300}, {a, 150}, {aH, 500}, {gSH, 325}, {gH, 175}, {fSH, 125}, {fH, 125}, {fSH, 250}, {P, 325}, 
  //Variant 2
  {f, 250}, {gS, 500}, {f, 375}, {cH, 125}, {a, 500}, {f, 375}, {cH, 125}, {a, 650}, {P, 650}  };

const String CHAINGANG_SONG = "Chaingang Song";
const int Chaingang[][2] = {
        {0, 250}, {e, 500}, {d, 250}, {e, 500}, {e, 250}, {c, 250}, {c, 500}, {0, 500}, {c, 250}, {d, 250}, {c, 250}, {d, 250},
        {e, 500}, {0, 1500}, {e, 500}, {g, 250}, {e, 500}, {0, 500}, 
        {0, 250}, {e, 500}, {d, 250}, {e, 500}, {e, 250}, {c, 250}, {c, 500}, {0, 500}, {c, 250}, {d, 250}, {c, 250}, {d, 250},
        {e, 500}, {0, 1500}, {c, 500}, {0, 1500}};

const String HIHO_SONG = "Hi Ho Song";

const int HiHo[][2] = {
  {d, 500}, {dH, 1500}, {0, 2000}, {d, 500}, {dH, 1500}, {0, 2000}, 
  {d, 250}, {g, 750}, {fS, 250}, {e, 750}, {g, 250}, {a, 350}, {b, 150}, {a, 250}, {g, 250}, {fS, 500}, 
  {d, 250}, {e, 250}, {g, 250}, {d, 250}, {d, 250}, {e, 250}, {fS, 250}, {g, 250}, {cH, 250}, {b, 750}, {g, 250}, {a, 250}, {d, 250}, {e, 250}, {fS, 250}, {g, 750}, 
   {fS, 250}, {e, 750}, {g, 250}, {a, 350}, {b, 150}, {a, 250}, {g, 250}, {fS, 500}, 
  {d, 250}, {e, 250}, {g, 250}, {d, 250}, {d, 250}, {e, 250}, {fS, 250}, {g, 250}, {cH, 250}, {b, 750}, {a, 250}, {g, 750}}; 

const int Scale[][2] = { {c,200}, {d,200}, {e,200}, {f,200}, {g,200}, {a,200}, {b,200}, {cH,200}, {dH,200}, {eH,200}, {fH,200}, {gH,200}};

// Variables that will change are below:
int CurrentLEDState = LOW;             // LEDState used to set the LED
long LEDChangeStartMillis = 0;        // will store last time LED was updated
long ElevatorChangeStartMillis = 0; //last time servo rotation cycle was restarted
long ChainsawAndRailChangeStartMillis = 0; //last time rail car reversed direction
int CurrentRailDirection = MOTOR_DIRECTION_FORWARD;
int CurrentCraneDirection = MOTOR_DIRECTION_FORWARD;
long CraneChangeStartMillis = 0;


int CoinSlotReading;
boolean TurnHasEnded = true;
boolean CoinDropHasStarted = false;

// the following three variables are a long because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
unsigned long MaxMillisecThisTurn = 0;  //timer to correct for built-in millis() fcn resetting every 50 days;
unsigned long CurrentMillisecReading = millis();
unsigned long TurnStartTime;

int CurrentNoteCounter = 0;
String CurrentSong;
int EndNoteCounter;

int CraneStartNote;
int CraneEndNote;
int ElevatorStartNote;
int ElevatorEndNote;
int DumpTruckAndShopStartNote;
int DumpTruckAndShopEndNote;
int ChainsawAndRailStartNote;
int ChainsawAndRailEndNote;
int BorerStartNote;
int BorerEndNote;
int SongRepeatCountdown;

void setup() {
  ElevatorServo.attach(IO_ELEVATOR_PWM_OUT_PIN); 
  LEDVictor.attach(IO_LED_W_VICTOR_PIN);
  BorerVictor.attach(IO_BORER_PIN);
  CraneVictor.attach(IO_CRANE_PIN);
  DumpTruckAndShopVictor.attach(IO_DUMPTRUCK_AND_SHOP_PIN);
  ChainsawAndRailVictor.attach(IO_CHAINSAW_AND_RAIL_PIN);
  CurrentSong = CHAINGANG_SONG;
  pinMode(IO_COIN_SLOT_PIN,INPUT);

  Serial.begin(9600);
  Serial.println("Starting");

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
      StopAll();
      CoinSlotReading = digitalRead(IO_COIN_SLOT_PIN);
      delay(COIN_READ_DELAY_MS);
      //Serial.println(CoinDropHasStarted);
      Serial.println("Sleeping");
      if (CoinSlotReading == COIN_DROP_DETECT_VALUE)
      {
          Serial.println("Detected Coin Drop Start");
          CoinDropHasStarted = true;
          Serial.println(CoinSlotReading);
      }
      else if (CoinDropHasStarted)
      {
        Serial.println("Waiting for Coin Drop End");
        if (CoinSlotReading != COIN_DROP_DETECT_VALUE)
        {
          Serial.println(CoinSlotReading);
          Serial.println("Detected Coin Drop End");
          CoinDropHasStarted = false;
          SetSong();
          TurnStartTime = millis();
          TurnHasEnded = false;
        }
      }
    }
    else
    {
      CurrentMillisecReading = millis();
      //Serial.println("Running");
      //Serial.println(CurrentMillisecReading - TurnStartTime);
      //if (CurrentMillisecReading - TurnStartTime < TURN_DURATION_MILLISEC)
      //{
        SetElevatorPosition();
        RunBorer();
        RunCrane();
        RunDumpTruckAndShop();
        RunChainsawAndRail();
        PlayNoteSmarter();
      //}
      //else
      //{
      //  TurnHasEnded = true;
      //}
    }
}

void SetSong()
{
  CurrentNoteCounter = 0;
  if (CurrentSong == STARWARS_SONG)
  {
     Serial.println("gonna play ChainGang"); 
    CurrentSong = CHAINGANG_SONG;
  }
  else if (CurrentSong == CHAINGANG_SONG)
  {
    CurrentSong = HIHO_SONG;
    Serial.println("gonna play Hi Ho"); 
  }
  else 
  {
    CurrentSong = STARWARS_SONG;
    Serial.println("gonna play Starwars"); 
  }
  
  if (CurrentSong == STARWARS_SONG)
  {
       EndNoteCounter = 75;
       SongRepeatCountdown = 1;
       DumpTruckAndShopStartNote = 0;
       DumpTruckAndShopEndNote = 15;
       CraneStartNote = 30;
       CraneEndNote = 45;
       ElevatorStartNote = 15;
       ElevatorEndNote = 35;
       ChainsawAndRailStartNote = 45;
       ChainsawAndRailEndNote = 60;
       BorerStartNote = 60;
       BorerEndNote = 90;
  }
  else if (CurrentSong == CHAINGANG_SONG)
  {
       EndNoteCounter = 34;
       SongRepeatCountdown = 2;
       
       DumpTruckAndShopStartNote = 0;
       DumpTruckAndShopEndNote = 15;
       CraneStartNote = 30;
       CraneEndNote = 45;
       ElevatorStartNote = 15;
       ElevatorEndNote = 30;
       ChainsawAndRailStartNote = ElevatorStartNote;
       ChainsawAndRailEndNote = ElevatorEndNote;
       BorerStartNote = CraneStartNote;
       BorerEndNote = CraneEndNote;

  }
  else if (CurrentSong == HIHO_SONG)
  {
       EndNoteCounter = 52; //46;
       SongRepeatCountdown = 2;
       
       DumpTruckAndShopStartNote = 7;
       DumpTruckAndShopEndNote = 30;
       CraneStartNote = 0;
       CraneEndNote = 20;
       ElevatorStartNote = 30;
       ElevatorEndNote = 52;
       ChainsawAndRailStartNote = DumpTruckAndShopStartNote;
       ChainsawAndRailEndNote = DumpTruckAndShopEndNote;
       BorerStartNote = ElevatorStartNote;
       BorerEndNote = ElevatorEndNote;
  }
}

void StopAll()
{
  ChainsawAndRailVictor.writeMicroseconds(VICTOR_NEUTRAL_SIGNAL);
  BorerVictor.writeMicroseconds(VICTOR_NEUTRAL_SIGNAL);
  CraneVictor.writeMicroseconds(VICTOR_NEUTRAL_SIGNAL);
  DumpTruckAndShopVictor.writeMicroseconds(VICTOR_NEUTRAL_SIGNAL);
  ElevatorServo.writeMicroseconds(VICTOR_NEUTRAL_SIGNAL);
  LEDVictor.writeMicroseconds(VICTOR_NEUTRAL_SIGNAL);

}

void PlayNoteSmarter()
{
  int NoteToPlay, NoteLength;

  //Serial.println(CurrentNoteCounter);    
  if (CurrentNoteCounter >= EndNoteCounter) 
  {
    if (SongRepeatCountdown > 1)
    {
      SongRepeatCountdown = SongRepeatCountdown - 1;
      CurrentNoteCounter = 0;
    }
    else
    {
       noTone(IO_SONG_PIN);  
       Serial.println("done"); 
       delay(1000);
       CurrentNoteCounter = 0;
       TurnHasEnded = true;
    }
  }
  else
  {
    if (CurrentSong == STARWARS_SONG)
    {
         NoteToPlay = Starwars[CurrentNoteCounter][0];
         NoteLength = Starwars[CurrentNoteCounter][ 1];
    }
    else if (CurrentSong == CHAINGANG_SONG)
    {
         NoteToPlay = Chaingang[CurrentNoteCounter][0];
         NoteLength = Chaingang[CurrentNoteCounter][ 1];
    }
    else if (CurrentSong == HIHO_SONG)
    {
         NoteToPlay = HiHo[CurrentNoteCounter][0];
         NoteLength = HiHo[CurrentNoteCounter][ 1];
    }
      //Serial.println(NoteLength); 
      //Serial.println(NoteToPlay); 
     
      tone(IO_SONG_PIN, NoteToPlay, NoteLength);
     //Play LED full or partial depending on Current note counter
     
      if(CurrentNoteCounter % 2 == 0)
      {
         LEDVictor.writeMicroseconds(CalcVictor(MOTOR_DIRECTION_FORWARD, LED_VICTOR_PERCENT));
      }
      else
      {
          LEDVictor.writeMicroseconds(CalcVictor(MOTOR_DIRECTION_FORWARD, LED_VICTOR_PERCENT * 0.4));
      }
      delay(NoteLength); 
      CurrentNoteCounter = CurrentNoteCounter + 1;
   }
}

int MillisecCounter()
{
  // This routine ensures that when millis() overflows a long
  // (at approx 50 days) it doesn't impact ability to determine
  // whether a turn is over.
  // This is the only routine that should use millis() directly.
  // All other routines should call this to determine how man
  // milliseconds we are in to a turn.
  
  CurrentMillisecReading = millis();
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
  return motorVoltageCalc; 
}

void RunChainsawAndRail(){
  if (CurrentNoteCounter < ChainsawAndRailStartNote | CurrentNoteCounter > ChainsawAndRailEndNote)
  {
    ChainsawAndRailVictor.writeMicroseconds(VICTOR_NEUTRAL_SIGNAL);
    return;
  }
  Serial.println("Chainsaw and Rail");
  if (CurrentMillisecReading < ChainsawAndRailChangeStartMillis)
  {
    // new turn - reset change timer
     ChainsawAndRailChangeStartMillis = CurrentMillisecReading;
  }
    
  if(CurrentMillisecReading - ChainsawAndRailChangeStartMillis > CHAINSAW_AND_RAIL_CYCLE_DUR_MS) 
  {
    // save the last time you reset the server to min pos 
    ChainsawAndRailChangeStartMillis = CurrentMillisecReading; 
    if (CurrentRailDirection == MOTOR_DIRECTION_FORWARD)
    {
      CurrentRailDirection = MOTOR_DIRECTION_REVERSE;
    }
    else
    {
      CurrentRailDirection = MOTOR_DIRECTION_FORWARD;
    }
  }  
    ChainsawAndRailVictor.writeMicroseconds(CalcVictor(CurrentRailDirection, CHAINSAW_AND_RAIL_SPEED_PERCENT));
}

void RunBorer(){
  if (CurrentNoteCounter < BorerStartNote | CurrentNoteCounter > BorerEndNote)
  {
    BorerVictor.writeMicroseconds(VICTOR_NEUTRAL_SIGNAL);
    return;
  }
  Serial.println("Borer");
  BorerVictor.writeMicroseconds(CalcVictor(MOTOR_DIRECTION_FORWARD, BORER_SPEED_PERCENT));  
}

void RunCrane(){
  if (CurrentNoteCounter < CraneStartNote | CurrentNoteCounter > CraneEndNote)
  {
    CraneVictor.writeMicroseconds(VICTOR_NEUTRAL_SIGNAL);
    return;
  }
  Serial.println("Crane");
  if (CurrentMillisecReading < CraneChangeStartMillis)
  {
    // new turn - reset change timer
      CraneChangeStartMillis = CurrentMillisecReading;
  }
  
  if(CurrentMillisecReading  - CraneChangeStartMillis > CRANE_CYCLE_DUR_MS) {
    // save the last time you reset the server to min pos 
    CraneChangeStartMillis = CurrentMillisecReading; 
    if (CurrentCraneDirection == MOTOR_DIRECTION_FORWARD)
    {
      CurrentCraneDirection = MOTOR_DIRECTION_REVERSE;
    }
    else
    {
      CurrentCraneDirection = MOTOR_DIRECTION_FORWARD;
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
  CraneVictor.writeMicroseconds(CalcVictor(CurrentCraneDirection, CRANE_SPEED_PERCENT * speedMultiplier));
}

void RunDumpTruckAndShop(){
  Serial.println("Shop and Dumptruck");
  if (CurrentNoteCounter < DumpTruckAndShopStartNote | CurrentNoteCounter > DumpTruckAndShopEndNote)
  {
    DumpTruckAndShopVictor.writeMicroseconds(VICTOR_NEUTRAL_SIGNAL);
    return;
  }
  
  DumpTruckAndShopVictor.writeMicroseconds(CalcVictor(MOTOR_DIRECTION_FORWARD, DUMPTRUCK_AND_SHOP_SPEED_PERCENT));
}
  
void SetElevatorPosition()
{
   if (CurrentNoteCounter < ElevatorStartNote | CurrentNoteCounter > ElevatorEndNote)
   {
     ElevatorServo.writeMicroseconds(VICTOR_NEUTRAL_SIGNAL);
     return;
   }
  Serial.println("Elevator");
  if (CurrentMillisecReading < ElevatorChangeStartMillis)
  {
    // new turn - reset change timer
    ElevatorChangeStartMillis = CurrentMillisecReading;
  }

  int millisecInThisDirection = CurrentMillisecReading - ElevatorChangeStartMillis;
  if(millisecInThisDirection > ELEVATOR_LIFT_DUR_MS) 
  {
    // save the last time you reset the server to min pos 
    ElevatorChangeStartMillis = CurrentMillisecReading; 
  }
    
  long ElevatorPos = SERVO_MIN_POS + (millisecInThisDirection * ELEVATOR_TURN_PER_MS);
  ElevatorServo.writeMicroseconds(ElevatorPos);
}

