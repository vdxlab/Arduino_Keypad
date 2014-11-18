


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
 
#define ledpin 10 // the pin which sends signal to open or close the relay

YunServer server;

// CONSTANTS
const byte ROWS = 4; // Four rows - this is the 1,4,7,* and 2,5,8,0, etc
const byte COLS = 3; // Three columns - this is the 123 and 456 and 789 and *0#
const int CHECK_TIME = 1000; // 500 miliseconds
const int SIZE_CLAVE = 4; // digits numbers size

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

// GLOBAL VARIABLES

int old_time; // time 
bool panelActivo;
char clave[SIZE_CLAVE];// {'1','3','2','5'}; // el codigo para abrir la puerta clave de entrada
int posActual = 0; // this is the integer that rises as each of the keys is pressed correctly. a 4-digit code like 1,3,2,5 rises to 4.

void setup() // this runs a single time each time the arduino powers up
{

  Bridge.begin();
  server.listenOnLocalhost();
  server.begin();

  old_time = millis();
  panelActivo = true;
  clave[0] = '2';
  clave[1] = '3';
  clave[2] = '4';
  clave[3] = '4';
  
  pinMode(ledpin,OUTPUT);
  digitalWrite(ledpin, HIGH);
  Serial.begin(9600); // this value must be the same as the serial value inside the write window (monitor serie)
  digitalWrite(ledpin, LOW);

}


void loop() // this is the loop that runs all the time
{

       // testFunctions(); // uncomment this for run test
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

	if(delta > CHECK_TIME){
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
        if (command == "connected"){
          Serial.println("connected");
          client.print("Connected ");
        }
        else if (command == "estado"){
          mostrarEstado(client);
        }
        else if (command == "onoff") {
          togglePanelActivo();
          printPanel();
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

/******************************************/
/*************** GET AND SET **************/
/******************************************/

boolean getPanelActivo(){
 return panelActivo;
}
void setPanelActivo(boolean b){
  panelActivo = b;
}
void togglePanelActivo(){
  panelActivo = !panelActivo;
}

/**
 * Set the new key value.
 * Warning:
 *  There aren't checks. Make sure to pass correct value or implement sanity check.
 * @params:
 *    key - an array of char with default dimension (SIZE_CLAVE) (@see testFunctions() as example)
*/
void setClave(char *key){
  memcpy(&clave,key,SIZE_CLAVE);
}

/********************************************/
/************** PRINT ***********************/
/********************************************/

void printClave(){
  char *tmp = (char*)malloc(5);
  memset(tmp,'\0',5);
  memcpy(tmp,clave,4);
  Serial.println("Current Key is:\t");
  Serial.println(tmp);
  free(tmp);
}

void printPanel(){
  boolean b;
  b = getPanelActivo();
  //char *msg = (b==true) ? "Activo" : "Not Activo";
  Serial.print("Panel is ");
  if(b){
      Serial.println("Activo ");
  }else{
      Serial.println("NO Activo ");
  }
}
/********************************************/
/************** TESTS ***********************/
/********************************************/

void testFunctions(){
  Serial.println("TEST FUNCTION");
  
  printClave();
  char kt[4] = {'1','2','3','4'}; // new key value
  
  Serial.println("Setting key ");
  setClave(kt);

  Serial.println("New key ");
  printClave();
  
  // example with pointers
  char *newKey = (char*)malloc(SIZE_CLAVE*sizeof(char));
  for(int i=0;i<SIZE_CLAVE;i++){
    newKey[i] = '7'; // fill 7
  }
  setClave(newKey);
  printClave();
  
  Serial.println("End Test Key ");
  
  printPanel();
  setPanelActivo(false);
  printPanel();
  togglePanelActivo();
  printPanel();

  Serial.flush();
   // stop loop to see log
  for(;;);
}

