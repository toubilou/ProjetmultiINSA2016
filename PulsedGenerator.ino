  // include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 10, 9, 8, 7);                                                                             //Terminals needed by lcd. 12 to 7 included.
  
  
  
  #define condoterminal A0                                                                                          //Terminal A0 on Arduino is used to measure condo voltage
  #define button1terminal A1                                                                                        //Terminal A1 on Arduino is used to measure button1 state
  #define button2terminal 6                                                                                         //Terminal 6 on Arduino is used to measure button2 state
  #define button3terminal A3                                                                                        //Terminal A3 on Arduino is used to measure button3 state
  #define button4terminal 3                                                                                         //Terminal 3 on Arduino is used to measure button4 state
  #define charging 1                                                                                                //Terminal 1 on Arduino is used to control charging optocoupler relays
  #define shot 2                                                                                                    //Terminal 2 on Arduino is used to control shooter Thyristor
  #define securityrelay 4                                                                                           //Terminal 4 on Arduino is used to control security relay
 

  enum states {statestart,statedefaut,statecharge,statefincharge,statetir,statesecurity};                           //Different state types the machine can be in
  states state;                                                                                                     //State variable, indicates in which state the machine is in. (default, security, charging, shooting...)
  
  enum modes {manual, automatic};                                                                                   //Different charging modes the machine can be it
  modes mode;                                                                                                       //Charging mode variable. Manual allows one charging which is completely controlled. Automatic will do 5 charges/shots in a row.
  
  int autocompteur;                                                                                                 //Variable that indicates how many times the machine will charge/shot. Goes from 5 to 0 each times automatic mode is selected.
  float tensionconsigne = 350.0;                                                                                    //Variable that indicates at which voltage the condo will charge before being able to shot.
   
  boolean button1,button2,button3,button4;                                                                          //Numerical Boolean variables of the 4 buttons used to control the machine. First 2 buttons are used to select features. Button 3 is used to shot only. Button 4 is used for Security Only. Can be pushed any time
  float voltage;                                                                                                    //Voltage variable that is used to display time averaged condovoltage. (measure optocouplers are not really accurates)

void readbuttons () {                                                                                               //Function that reads every button states and puts results in numerical button variables.
  button1=digitalRead(button1terminal);
  button2=digitalRead(button2terminal);
  button3=digitalRead(button3terminal);
  button4=digitalRead(button4terminal);
}

void buttonsreset() {                                                                                               //Function that reset every numerical button variables to passive states. When boolean state is HIGH, button is not being pushed on.  If pushed on, terminal gets connected to ground and becomes LOW.
  button1=HIGH;
  button2=HIGH;
  button3=HIGH;
  button4=HIGH;
}

float condovoltage ()                                                                                               //Integer variable of the condo voltage.
{
  int tensionlue = analogRead(condoterminal);                                                                       //Reads voltage on condoterminal and attributes to numerical variable tensionlue a value(0 for 0 Volt and 1023 for 5 volts)
  lcd.setCursor(0,1);
  float realtensionlue=(370.0/200.0)*float(tensionlue-252);                                                          //Functions that translates value readed to the real value of the condo. Optocouplers actually give a value of 252 for 0 volts (due to Vref value of optocouplers) and are linear to a 370/200*x function. Calibrations were made here to determinates these values.
  lcd.print(realtensionlue,1);                                                                                      //Display the translated voltage readed.
  return (realtensionlue);
}


void choicemaxvoltage()                                                                                             //Function used to modify tensionconsigne float variable using lcd screen and buttons 1 and 2. Happens when the machine is starting.
{                                                                                           
  buttonsreset();   
  lcd.clear();
  lcd.print("set max voltage");                                                                                     //Display for 1 second the need to set max voltage.
  delay(1000);
  lcd.clear();
  while (button1==HIGH) {                                                                                           
  buttonsreset;
  lcd.print("1=>ok, 2=>change");                                                                                    //Display Instructions to set max voltage. 
  lcd.setCursor(0, 1);
  lcd.print(tensionconsigne,1);                                                                                     //Display Tensionconsigne to be saved.
  readbuttons();
  if (button2==LOW){                                                                                                //Changes displayed Tensionconsigne when button 2 is pressed. Button 1 is pressed to save the set up.
    if (tensionconsigne>=360) {
      tensionconsigne=0;
    }
    tensionconsigne=tensionconsigne+10;                                                                         
    delay(100); 
  }
  
  }
}



void demarrage ()                                                                                                   //Function working on starting state "statestart" of the machine. Welcomes user using lcd, calls the set up of max voltage. Loading security features can be added here.                                                       
{

  lcd.clear();                                                                                                      //LCD displays welcome message and asks if user wants to start the machine. (not necessary, but security verifications can be added here)
  lcd.print("Welcome");
  lcd.setCursor(0, 1);
  lcd.print("Press 1 to start");
  buttonsreset();
  while (button1==HIGH) {                                                       
      readbuttons();
      delay(100);                                                                      
    } 
    
  lcd.clear();
  lcd.print("Starting...");
  delay(1000);
  
  choicemaxvoltage();                                                                                               //Calls max voltage set up
  digitalWrite(securityrelay, HIGH);                                                                                //Security relay is closed (current can flow between Diode Bridge and condo). Will be opened only on security state.
}



int defaut() {                                                                                                      //Function working at Default state of the machine.
  
  digitalWrite(shot, LOW);                                                        
  digitalWrite(charging, LOW);                                                                                      //There is no current going throu charging optocouplers or thyristor.

  
  delay(300);
    lcd.clear();
    lcd.print("Manual or Automatic?");                                                                              //Displays Instructions to choose the Charging Mode to be used. Manual when user want to control when to charge/shot. Automatic to execute 5 charges/shots in a row.
    lcd.setCursor(0, 1);
    lcd.print("Press 1 or 2");
    
    readbuttons();
  while ((button1==HIGH) and (button2==HIGH) and (button4==HIGH)) {                                                 //A mode is selected when a button is pressed on. Security state can also be called.      
      readbuttons();                                                  
      delay(100);                                                                                             
  }
  if (button4==LOW) { return 1; }
  if (button1==LOW) {mode=manual; return 0; }                                                                       //Manual mode is called. Variable mode is now manual and will be used in others machine states.
  if (button2==LOW) {mode=automatic; autocompteur=5; return 0; }                                                    //Security mode is called. automatic mode is now automatic and will be used in others machine states. Autocompteur starting value is set to 5 and can be modified in the program HERE. Autocompteur set function does not seem necessary, 5 seems to be a good value.
  }
  





int charge ()                                                                                                      // Condo charge. Condo maximum voltage value allowed for safe mode can be changed here.
{
                                                        
  if (mode==manual)                                                                                                //Case charging mode is manual, asks user for instructions to allow charge. Security state can be called here.
  {                                       
    lcd.clear();
    lcd.print("to charge:"); 
    lcd.setCursor(0,1);
    lcd.print("press 1");
    delay(500);
  while ((button1==HIGH) and (button4==HIGH)) {                                                          
    readbuttons();           
    delay(100);                                            
   }
   if (button4==LOW) { return 1; }
  }
  
 
  lcd.clear();    
  lcd.print("charging");                                                                                          //Displays that the condos are charging. 
  
  if (mode==automatic){lcd.setCursor(10,0); lcd.print(autocompteur);}                                             //If automatic mode was selected, it displays the number of charge/shot circles that are still going to be done. Goes from 5 to 1
  
  digitalWrite(charging, HIGH);                                                 
  delay(1000);
  voltage=0.0;
  while (voltage < tensionconsigne) {                                                                             //Functions that average a bit the voltage readed and displays each 0.2seconds the result in volts. (Displays a value going from 0 to tensionconsigne seted up)
    voltage=(condovoltage()/2.0)+voltage/2.0;
    delay(200);
    button4=digitalRead(button4terminal);
    if (button4==LOW) { return 1; }
    
  }
  
 digitalWrite(charging, LOW);                                                                                     //Opens the charging optocouplers once condo are charged to tensionconsigne. Current does not flow anymore.
 return 0;
}






int fincharge ()                                                                                                // Dangerous state. Condo is charged. This functions asks User if he wants to shot. He has 10 seconds to decide or generator will switch to safe mode. This state is called only if mode is manual.
{

  int compteur = 0;
  
  lcd.clear();
  lcd.print("Charged: Press 3 to shoot");                                                                       //Displays Instructions to close thyristor and let current flow through the coil.


  
  
  while ((button3==HIGH) and compteur <= 100 and (button4==HIGH))                                               //Button 3 can be pressed on to allow shoot. (Machine will go to shot state) or security button can be pressed to call security state.
  {
      readbuttons();
      delay(100);                                                                     
      compteur = compteur + 1;                                                                                  //a compteur is used and allow the charged state to be on during 10 seconds. If no buttons are pressed, il will automatically go to security mode.
      lcd.setCursor(0,1);                                                                                       //Displays the compteur on lcd screen, going from 10.0 seconds to 0.
      lcd.print(100-compteur);
    }
   
  
  if (compteur >= 100) {return 2;}                                                                             // if countdown is checked, goes to security mode.
  if (button4==LOW) { return 1;}
  if (button3==LOW) { return 0;}
}



int tir() {                                                                                                   // Shoot mode: condo is going to shot. Get away.
  lcd.clear();
  lcd.print("SHOOOOT!!!");                                                                                    //Lcd displays instructions to get away, condo is shooting.
  digitalWrite(2, HIGH);                                                                                      //Thyristor closes for 0.5 seconds, letting current flow.
  delay(500);
  digitalWrite(2, LOW);
  return 0;
}








void security(){                                                                                             //Security state function. Every relay is opened, and mecanical relay output is connedted to ground, allowing condo to discharge through charging resistance. ( and also a bit through measure dividing bridge)
  
  digitalWrite(shot, LOW);
  digitalWrite(charging,LOW);
  digitalWrite(securityrelay, LOW);


    lcd.clear();
    lcd.print("Security");
    lcd.setCursor(0,1);
    lcd.print("press 1 to leave");                                                                          //Displays Instructions to leave Security mode: Press button 1.
  while (button1==HIGH) {                                                       
      readbuttons();
      delay(100);                                                                      
    }
   digitalWrite(securityrelay, HIGH);                                                                       //Mecanical relay state can be modified only at the start of the machine and in security state.
  }
 





void setup() 
{
  lcd.begin(16, 3);
// set pins as input  
  pinMode(condoterminal, INPUT);                                                                           //Terminal A0 on Arduino is used to measure condo voltage
  pinMode(button1terminal,INPUT);                                                                          //Terminal A1 on Arduino is used to measure button1 state
  pinMode(button2terminal,INPUT);                                                                          //Terminal 6 on Arduino is used to measure button2state
  pinMode(button3terminal,INPUT);                                                                          //Terminal A3 on Arduino is used to measure button3 state
  pinMode(button4terminal,INPUT);                                                                          //Terminal 3 on Arduino is used to measure button4 state
// set pins as output
  pinMode(shot, OUTPUT);                                                                                   //Terminal 2 on Arduino is used to control shooter Thyristor
  pinMode(charging, OUTPUT);                                                                               //Terminal 1 on Arduino is used to control charging optocoupler relays
  pinMode(securityrelay,OUTPUT);                                                                           //Terminal 4 on Arduino is used to control security relay
 
  state=statestart;                                                                                        //First state of th machine is Statestart.
}

void loop()                                                                                                //Loop function that begins with Statestart and switches states according to the presentstate and the answer of the machine. It can be used to establish State diagram.
{
 switch ( state)
 {
  case statestart:                                                                                         //First state statestart calls function demarrage, and goes to statedefaut once choicemaxvoltage is set up. Loading features can be added in function demarrage.
    demarrage();
    state=statedefaut;
    
  case statedefaut:                                                                                        //From here, security state can be called Anytime in any state. Default state asks user to chose charging mode and then goes to charge state.
    switch (defaut()) {
    case 0: state=statecharge; break;
    case 1: state=statesecurity; break;}
    break;
    
  case statecharge:                                                                                        //Statecharge is the charging mode. Condo is charging and lcd screens displays condo voltage. The charge begins automatically when automatic mode was selected.
     switch (charge()) {
      case 0: switch (mode) {
        case manual:state=statefincharge;break;
        case automatic: state=statetir;break;}
        break;
      case 1: state=statesecurity;break;      }                                
    break;
    
  case statefincharge:                                                                                     //statefincharge is a state that can be on only when condo is charged to tensionconsigne, and if manual mode was selected. It gives 10seconds to the user to order a shoot.
    switch (fincharge()) {                                                      
      case 0: state=statetir; break;
      case 1: state=statesecurity; break;
      case 2: state=statesecurity; break;
    }
    break;
    
  case statetir:                                                                                          //Shooting state. Lets current flow through coil. If automatic mode was selected, another charge will begin automatically until 5 charges were executed. Then, goes to default state and waits for instructions.
    switch (tir()) {
      case 0: switch(mode) 
      { case manual: state=statedefaut; break;
        case automatic : autocompteur=autocompteur-1; if (autocompteur>=1) {state=statecharge;} else state=statedefaut; break;}
        break;
      case 1: state=statesecurity;break;
    }
    break;
    
  case statesecurity:                                                                                     //Security state. If the condo was charged when security state was called, it will discharge quickly (~15sec). This state cuts currents everywhere and can be called anytime.
    security();
    state=statedefaut;
    break;
 } 
 buttonsreset();
}
