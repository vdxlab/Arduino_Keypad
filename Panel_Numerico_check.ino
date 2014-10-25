
/*  Panel Numerico de Apertura de una puerta electrica, 
 *  con una clave de 4 digitos
 *  con panel de control web, para activarlo/desactivarlo 
 *  y poder abrir la puerta desde el movil. 
 */

#include <Keypad.h>
#include <Bridge.h>
#include <YunServer.h>
#include <YunClient.h>
 
YunServer server;

bool panelActivo = true;

const byte ROWS = 4; // Four rows - this is the 1,4,7,* and 2,5,8,0, etc
const byte COLS = 3; // Three columns - this is the 123 and 456 and 789 and *0#
char clave[4]= {'1','3','2','5'}; // el codigo para abrir la puerta clave de entrada
int posActual = 0; // this is the integer that rises as each of the keys is pressed correctly. a 4-digit code like 1,3,2,5 rises to 4.
 
// Define the Keymap
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'#','0','*'}
};

// Connect keypad ROW0, ROW1, ROW2 and ROW3 to these Arduino pins.
byte rowPins[ROWS] = {8,7,6,5}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {4, 3, 2}; 

// Create the Keypad
Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

#define ledpin 10 // the pin which sends signal to open or close the relay

void setup() // this runs a single time each time the arduino powers up
{

  Bridge.begin();
  server.listenOnLocalhost();
  server.begin();

  pinMode(ledpin,OUTPUT);
  digitalWrite(ledpin, HIGH);
  Serial.begin(9600); // this value must be the same as the serial value inside the write window (monitor serie)
  digitalWrite(ledpin, LOW);

}

int old_time=0; // time 

#define CONST_DELTA 10000 // 10 seconds 

void loop() // this is the loop that runs all the time
{
	if(check()){
		doServer();
	}else{
		doKeypad();
	}
     
	delay(50);
}

// time function
boolean check(){
	boolean result=false;
	int curr_time = millis();
	int delta = curr_time - old_time;

	if(delta<0) delta = 0; // every 50 days the millis reset 
	if(delta > CONST_DELTA){
		old_time = curr_time;
		result = true;
	}

	return result;
}

void doServer(){
    /**************************************
    *        PANEL DE CONTROL WEB
    **************************************/
      YunClient client = server.accept();
 
      if (client) {
        String command = client.readString();
        command.trim();
        client.print("On");
        if (command == "Conected")
          client.print("On");
        if (command == "Estado")
          mostrarEstado(client);
        if (command == "OnOff") {
          panelActivo = !panelActivo;
          mostrarEstado(client);
        }
        if (command == "Abrir")
          posActual = 4;
        client.stop();
      }

}


void doKeypad(){
  char key = kpd.getKey(); 
  
  if(key)  // Check for a valid key.
  {
    if (key == clave[posActual]) { //right key has been pressed
       posActual ++;  
    }
    else //wrong key has been pressed
        posActual = 0; 

    if (posActual == 4 && panelActivo) {   //you got the code right, it's a 4-digit code...
        posActual = 0;   //resets to waiting for the first key
        digitalWrite(ledpin, HIGH);  // tells to open relay on pin 10
        delay(2000);  //wait two seconds
        digitalWrite(ledpin, LOW);  // close relay
        
    }
  }

}
void mostrarEstado(YunClient client) {
   if (panelActivo)
        client.print("On");
   else
        client.print("Off");
}
