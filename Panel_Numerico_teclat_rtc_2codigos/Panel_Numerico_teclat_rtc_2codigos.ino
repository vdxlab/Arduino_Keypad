
/*  Panel Numerico de Apertura de una puerta electrica, 
 *  con dos claves de 4 digitos introducidas en keypad
 *  con Real Time Clock para establecer los horarios de apertura
 */

#include <Keypad.h>

bool panelActivo = true;

const byte ROWS = 4; // Four rows - this is the 1,4,7,* and 2,5,8,0, etc
const byte COLS = 3; // Three columns - this is the 123 and 456 and 789 and *0#
char claveCSO[4]= {'1','2','3','4'}; // el codigo para abrir la puerta clave de entrada sujeto a horario del cso
char claveVIV[4]= {'5','6','7','8'}; // el codigo para abrir la puerta clave de entrada 7 dias 24 horas.
//int posActual = 0; // this is the integer that rises as each of the keys is pressed correctly. a 4-digit code like 1,3,2,5 rises to 4
int CSO=0; //aquest marcador aumenta cada cop que una tecla del codi del CSO es prem correctament.
int VIV=0; //aquest marcador aumenta cada cop que una tecla del codi de lA VIVenda es prem correctament.
 
// Define the Keymap
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

// Connect keypad ROW0, ROW1, ROW2 and ROW3 to these Arduino pins.
byte rowPins[ROWS] = {8,7,6,5}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {4, 3, 2}; 

// Create the Keypad
Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

#include <virtuabotixRTC.h>

/////////////////////////////////////////
// Creation of the Real Time Clock Object
//   SCLK -> 9,   I/O -> 11,   CE -> 12
virtuabotixRTC myRTC(9, 11, 12);


#define ledpin 10 // the pin which sends signal to open or close the relay  -----   PIN RELE


void setup() // this runs a single time each time the arduino powers up
{
  pinMode(ledpin,OUTPUT);
  digitalWrite(ledpin, HIGH);
  Serial.begin(9600); // this value must be the same as the serial value inside the write window (monitor serie)
  digitalWrite(ledpin, LOW);
}


void loop() // this is the loop that runs all the time
{
  doKeypad();
}



void doKeypad(){
  char key = kpd.getKey(); 
  
  if(key){  // Check for a valid key.    
     if (key == claveCSO[CSO]) {
        CSO++;
        }
        else
        CSO=0;  
        
     if (key == claveVIV[VIV]) {
        VIV++ ;
        }
        else
        VIV=0;
     

     Serial.print(key);
     Serial.println(" ");

     if (CSO == 4) {   //you got the code right, it's a 4-digit code...
         doRtc();      
         if (myRTC.dayofweek>0 && myRTC.dayofweek<6 && myRTC.hours>7 && myRTC.hours<21) {
         Serial.println("CSO");
            openDoor();
            CSO = 0;   //resets to waiting for the first key
            }
        }
     if (VIV == 4) {   //you got the code right, it's a 4-digit code...
         Serial.println("Vivenda");
         openDoor();
         VIV = 0;   //resets to waiting for the first key
        }
     }
}



void doRtc(){
  
  /////////////////////////////////////////////////////////////////////////////////////////
  // This allows for the update of variables for time or accessing the individual elements.                //|
  myRTC.updateTime();
  
  //////////////////////////////////////////////
  // Start printing RTC elements as individuals                                                            //|   

  //  Serial.print("Current Date / Time: ");                                                                 //|
  switch (myRTC.dayofweek)                  // Esto pone nombre del dia
  {
    case 1:
    Serial.print(" Dilluns"); break;
    case 2:
    Serial.print(" Dimarts"); break;
    case 3:
    Serial.print(" Dimecres"); break;
    case 4:
    Serial.print(" Dijous"); break;
    case 5:
    Serial.print(" Divendres"); break;
    case 6:
    Serial.print(" Dissabte"); break;
    case 7:
    Serial.print(" Diumege"); break;
  }

  Serial.print("  ");                                                                                    //| 
  Serial.print(myRTC.dayofmonth);                                                                        //| 
  Serial.print("/");                                                                                     //| 
  Serial.print(myRTC.month);                                                                             //| 
  Serial.print("/");                                                                                     //| 
  Serial.print(myRTC.year);                                                                              //| 
  Serial.print("  ");                                                                                    //| 
  Serial.print(myRTC.hours);                                                                             //| 
  Serial.print(":");                                                                                     //| 
  Serial.print(myRTC.minutes);                                                                           //| 
  Serial.print(":");                                                                                     //| 
  Serial.println(myRTC.seconds); 
  Serial.println (" ");
}


void openDoor(){
        digitalWrite(ledpin, HIGH);  // tells to open relay on pin 10
        Serial.println (" openDoor ");
        delay(2000);  //wait two seconds
        Serial.println (" closeDoor ");
        digitalWrite(ledpin, LOW);  // close relay
}



//FIN
