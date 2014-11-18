

/*  Panel Numerico de Apertura de una puerta electrica, 
 *  con una clave de 4 digitos
 *  con panel de control web, para activarlo/desactivarlo 
 *  y poder abrir la puerta desde el movil. 
 *
 * By: Davide y Santiago
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

int old_time; // time 

int check_time = 1000; // 500 miliseconds

void setup() // this runs a single time each time the arduino powers up
{

  Bridge.begin();
  server.listenOnLocalhost();
  server.begin();

  old_time = millis();
  
  pinMode(ledpin,OUTPUT);
  digitalWrite(ledpin, HIGH);
  Serial.begin(9600); // this value must be the same as the serial value inside the write window (monitor serie)
  digitalWrite(ledpin, LOW);

}


void loop() // this is the loop that runs all the time
{

        //Each 10 seconds we check the Server
	if(check()){
                 Serial.println("\nServer"); 
                 doServer();
	}
        //The rest of the time we check the Keypad
        else{
                Serial.print(".");
                
         	doKeypad();		

	}
     
	delay(50);
}



// time function
boolean check(){
	boolean result=false;
	int curr_time = millis();
	int delta = curr_time - old_time;

	if(delta < 0) old_time = millis();//delta = 0; // every 50 days the millis reset 

	if(delta > check_time){
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
      bool response = true;
 
      if (client) {
        Serial.print("Connected! a ");
        String command = client.readString();
        command.trim();
        client.print("On");
        if (command == "connected"){
          Serial.println("connected");
          client.print("On");
        }
        else if (command == "estado"){
          mostrarEstado(client);
           
        }
        else if (command == "onoff") {
          panelActivo = !panelActivo;
          Serial.println("OnOff");
          mostrarEstado(client);
        }
        else if (command == "abrir" && panelActivo) {
          openDoor();
           Serial.println("Abrir");
        }
        else
          response = false;
        client.stop();
        

      }
}


void doKeypad(){
  char key = kpd.getKey(); 
  
  if(key)  // Check for a valid key.
  {
    Serial.println(key);
    if (key == clave[posActual]) { //right key has been pressed
       posActual ++;  
    }
    else //wrong key has been pressed
        posActual = 0; 
    if (posActual == 4 && panelActivo) {   //you got the code right, it's a 4-digit code...
        posActual = 0;   //resets to waiting for the first key
        openDoor();
        
    }    
  }

}

void openDoor() {
 
        digitalWrite(ledpin, HIGH);  // tells to open relay on pin 10
        delay(2000);  //wait two seconds
        digitalWrite(ledpin, LOW);  // close relay 
        
}

void mostrarEstado(YunClient client) {
   if (panelActivo) {
        client.print("On");
        Serial.println("Estado panel: On");
   }
   else {
        client.print("Off");
        Serial.println("Estado panel: Off");        
   }
}
