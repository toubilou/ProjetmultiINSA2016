  #define bornecondo A0                                                                // Condo voltage
  #define BuzzerShot 2                                                                 // Generator going to shot. Buzzer makes noises. Get Away.
  #define DiodeCharged 3                                                               // Condo is charged. Red diode switches on. Very dangerous state.
  #define DiodeCharging 4                                                              // Condo is charging. Orange diode switches on. Dangerous state.
  #define DiodeOK 5                                                                    // Generator Startup. Green diode switches on once user asks for it after generator is put under voltage
  #define DiodeDefault 6                                                               // Default mode is the safe mode. Blue diode switches on. Potentially dangerous state, condo might be under discharge through security resistance. TODO: add a safe mode

  int secu =0;
  char incomingByte ='N';
  enum states {statestart,statedefaut,statecharge,statefincharge,statetir,statesecurity};
  states state;
  enum modes {manual, automatic};
  modes mode;
  int autocompteur;

  
 
int problem() {
  if (Serial.available() != 0) {
    incomingByte = Serial.read();
    Serial.println(incomingByte);
    if (incomingByte =='P') {return 1 ;}
    else return 0;
  }
  else return 0;
}






void demarrage ()                                                                      // Asks for a start 
                                                                                       // TODO Verifications de sécurités à rajouter
{
  Serial.println("plop mon vieux, bienvenue dans la matrice");                         // Welcome message
  incomingByte = 'N';
  while (incomingByte != 'Y') {                                                        // Check incommming char
    Serial.println("Si vous voulez démarrer tapez Y");                                 // Incorrect char typed or first iteration
    while (Serial.available() == 0) {                                                  // Verify incomming char
      delay(300);                                                                      // Pooling delay for serial port
    }
    incomingByte = Serial.read();
  }
  Serial.println("t'as tapé Y c'est bien");
  digitalWrite(DiodeOK, HIGH);
  delay(1000);
}






int tensioncondo ()                                                                    // Int variable of the condo voltage. Between 0 for 0V and 1023 for 5V
{
  int tensionlue = analogRead(bornecondo);
  Serial.println(tensionlue);
  return tensionlue;
}



int defaut() {
  
  digitalWrite(BuzzerShot, LOW);
  digitalWrite(DiodeCharged, LOW);
  digitalWrite(DiodeCharging, LOW);
  digitalWrite(DiodeOK, LOW);
  digitalWrite(DiodeDefault, HIGH);
  
  delay(300);
  incomingByte = 'N';
  while ((incomingByte != 'A') and (incomingByte != 'M') and (incomingByte != 'P')) {                                                        // Check incommming char
    Serial.println("Etat defaut, choisissez un Mode, Manuel en tapant M, ou Automatique en tapant A");                                 // Incorrect char typed or first iteration
    while (Serial.available() == 0) {                                                                                     // Verify incomming char
      delay(300);                                                                                              // Pooling delay for serial port
    }
    incomingByte = Serial.read();
  }
  
  switch (incomingByte) {
    case 'P': return 1; break;
    case 'M': mode=manual; return 0; break;
    case 'A': mode=automatic; autocompteur=5; return 0; break;
    
  }
  

}



int charge ()                                                                         // Condo charge. Condo maximum voltage value allowed for safe mode can be changed here.
{
  int tensionconsigne = 1000;                                                          // Condo maximum voltage variable.
  incomingByte='N';
  if (mode==automatic) {incomingByte = 'C';}
  
  while ((incomingByte != 'C') and (incomingByte != 'P')) {                                                        // Asks user to enter "C" to begin charge. Charge then begins.
    Serial.println("Si vous voulez charger tapez C");                       
    while (Serial.available() == 0) {delay(300);}
    
    incomingByte = Serial.read();
    if (incomingByte==1){return 1;}                                             
  }
  digitalWrite(DiodeDefault, LOW);                                                     // Switch off default mode diode
 
  Serial.println("Attention, le Condensateur commence à se charger");
  digitalWrite(DiodeCharging, HIGH);                                                   // Switch on Charging diode: orange
  while (tensioncondo() < tensionconsigne) {                                           // Display condo voltage until it's bigger than Max Value
    delay(100);
    if (problem()==1) {return 1;}
    
  }
 digitalWrite(DiodeCharging, LOW);
 return 0;
}






int fincharge ()                                                                      // Dangerous state. Condo is charged. This functions asks User if he wants to shot. He has 10 seconds to decide or generator will switch to safe mode.
{
  incomingByte = 'N';
  int compteur = 0;
  
  Serial.println("Le condensateur est chargé, vous avez 10 secondes pour activer manuellement le tir ou une décharge sera automatiquement lancée.");
  digitalWrite(DiodeCharged, HIGH);                                                   // Switch on charged diode :Red diode
  digitalWrite(DiodeCharging, LOW);                                                   // Switch off charging diode : orange diode
  
  
  while (incomingByte != 'T' and compteur <= 100 and incomingByte != 'P') {
    Serial.println("Si vous voulez tirer tapez T");                                   // Asks User to type T to shoot.
    while (Serial.available() == 0 and compteur <= 100) {                             // Countdown of 10 seconds begins
      delay(100);                                                                     // Tries to read if user typed something every 0.1seconds
      compteur = compteur + 1;
    }
    if (Serial.available() != 0) {                                                    // If user typed something, it checks if it is T
      incomingByte = Serial.read();
    }
  }
  if (compteur >= 100) {                                                              // if countdown is checked, goes to default mode ( safe mode)
    return 2;
  }
  else if (incomingByte=='T') {
    return 0;                                                                           // if user types T, goes to Shot mode
  }
  else if (incomingByte=='P') {return 1;}
}






int tir() {                                                                          // Shoot mode: condo is going to shot. Get away.
  Serial.println("TIR DANS 1SECONDE!!!");
  digitalWrite(2, HIGH);                                                              // Buzzer makees noise for 1sec
  delay(1000);
  
  digitalWrite(2, LOW);
  if (problem()==1) {return 1;}

  Serial.println("BOOOOM!!!");
  return 0;
}








void security(){

    Serial.println("securité");                                                        // Welcome message
  incomingByte = 'N';
  while (incomingByte != 'Y') {                                                        // Check incommming char
    Serial.println("Si vous voulez reprendre tapez Y");                                // Incorrect char typed or first iteration
    while (Serial.available() == 0) {                                                  // Verify incomming char
      delay(300);                                                                      // Pooling delay for serial port
    
    }
    incomingByte = Serial.read();
  }
 
}




void setup() {

// set pins as input  
  pinMode(bornecondo, INPUT);                                                      // Condo voltage
// set pins as output
  pinMode(BuzzerShot, OUTPUT);                                                     // Generator going to shot. Buzzer makes noises. Get Away.
  pinMode(DiodeCharged, OUTPUT);                                                   // Condo is charged. Red diode switches on. Very dangerous state
  pinMode(DiodeCharging, OUTPUT);                                                  // Condo is charging. Orange diode switches on. Dangerous state.
  pinMode(DiodeOK, OUTPUT);                                                        // Generator Startup. Green diode switches on once user asks for it after generator is put under voltage
  pinMode(DiodeDefault, OUTPUT);                                                   // Default mode is the safe mode. Blue diode switches on. Potentially dangerous state, condo might be under discharge through security resistance. TODO: add a safe mode

  Serial.begin(9600);
  
  state=statestart;
}

void loop() {
 switch ( state)
 {
  case statestart:
    demarrage();
    state=statedefaut;
    
  case statedefaut:
    switch (defaut()) {
    case 0: state=statecharge; break;
    case 1: state=statesecurity; break;}
    break;
    
  case statecharge:
     switch (charge()) {
      case 0: switch (mode) {
        case manual:state=statefincharge;break;
        case automatic: state=statetir;break;}
        break;
      case 1: state=statesecurity;break;      }                                // charge returns 1 if there is a problem. else returns 0
      
    break;
    
  case statefincharge:                                                            
    switch (fincharge()) {                                                       // fincharge returns 0 if user asked for shot within 10sec allowed, returns 1 if there is a problem and 2 if user did not do anything withis 10sec.
      case 0: state=statetir; break;
      case 1: state=statesecurity; break;
      case 2: state=statedefaut; break;
    }
    break;
    
  case statetir:                                                                
    switch (tir()) {
      case 0: switch(mode) 
      { case manual: state=statedefaut; break;
        case automatic : autocompteur=autocompteur-1;Serial.println(autocompteur); if (autocompteur>=1) {state=statecharge;} else state=statedefaut; break;}
        break;
      case 1: state=statesecurity;break;
    }
    break;
    
  case statesecurity:
    security();
    state=statedefaut;
    break;
 } 
}
