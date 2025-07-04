
#include <Keypad.h>
#include <Servo.h>

//Keypad Definitions
const byte ROWS=4;
const byte COLS=4;
char keys[ROWS][COLS]{
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'},
};
byte rowPins[ROWS]={22,23,24,25};
byte colPins[COLS]={26,27,28,29};
//Create a Keypad Object
Keypad customKeypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
//LED and Buzzer Pin Definitions
const int redLed=7;
const int greenLed=8;
const int buzzerPin=9;
//Create a Servo Object
Servo doorServo;
const int servoPin=10;
//Servo Door Positions
const int lockedPos=0;
const int unlockedPos=90;
//Passcode 
const char masterCode[]="1234";
const char lockCode[] = "0000";
const byte codeLength=4; //Length of Passcode
//Variables
char enteredCode[codeLength +1]; //+1 for null terminator
byte currentCodeIndex=0;

bool unlocked = false;            //Tracks if the door is currently unlocked
unsigned long unlockTimestamp = 0; //Stores the millis() time when the door was unlocked
const unsigned long autoLockDelay = 5000; //5 seconds (5000 milliseconds) for auto-lock
void setup(){
    Serial.begin(9600);
    Serial.println("Servo Door Lock Ready");
    Serial.println("Enter Code");
    pinMode(redLed,OUTPUT);
    pinMode(greenLed,OUTPUT);
    pinMode(buzzerPin,OUTPUT);
    //Attch Servo Pin
    doorServo.attach(servoPin);
    //Ensure LED is OFF and Door is Locked
    digitalWrite(redLed,LOW);
    digitalWrite(greenLed,LOW);
    digitalWrite(buzzerPin,LOW);
    lockDoor(); 
}
void loop(){
    char customKey = customKeypad.getKey();
    if(customKey){//A key was pressed
        Serial.print("Key Pressed");
        Serial.print(customKey);
        if(customKey=='#'){ //'#' is used to lock door and erase input
            Serial.println("Input Cleared. Locking Door");
            lockDoor();
            resetCodeInput();
        }
        else if (customKey=='*'){ //'*' is used to attempt unlock
            if (currentCodeIndex==codeLength){
                checkCode();
            }
            else {
                Serial.println("Code is Short");
                buzzWrongCode();
            }
        } else if(currentCodeIndex<codeLength){
            enteredCode[currentCodeIndex]=customKey;
            currentCodeIndex++;
            enteredCode[currentCodeIndex]='\0'; //Null Terminate 
            Serial.println("Current Input");
            Serial.println(enteredCode);
            buzzKeyPress();
        } else {
            Serial.println("Code Full");
            buzzWrongCode();
        }
    }
    // 5-second Auto-Lock Logic 
    if (unlocked && (millis() - unlockTimestamp >= autoLockDelay)) {
        Serial.println("Auto-locking door after 5 seconds");
        lockDoor();
        resetCodeInput(); // Clear input after auto-lock
    }
}
//Door Lock Logics
void checkCode() {
    if (strcmp(enteredCode,masterCode)==0){
        Serial.println("Access Granted");
        unlockDoor();
    } else{
        Serial.println("Access Denied");
        lockDoor();
        flashRedLed(3); //Flash Red Led 3 times
        buzzWrongCode();
    }
    resetCodeInput();
}
void resetCodeInput() {
    currentCodeIndex=0;
    for (byte i=0; i<codeLength;i++){
        enteredCode[i]='\0';
    }
    Serial.println("Enter New Code");
}
void lockDoor(){
    Serial.println("Door Locked");
    doorServo.write(lockedPos);
    digitalWrite(greenLed,LOW);
    digitalWrite(redLed,LOW); //Unless flashing for WrongCode
}
void unlockDoor(){
    Serial.print("Door Unlocked");
    doorServo.write(unlockedPos);
    digitalWrite(greenLed,HIGH);
    digitalWrite(redLed, LOW); //OFF
}
// Audio and Visual Functions
void flashRedLed(int times){
    for (int i=0; i<times;i++){
        digitalWrite(redLed,HIGH);
        delay(100);
        digitalWrite(redLed,LOW);
        delay(100);
    }
} 
void buzzKeyPress(){
    digitalWrite(buzzerPin,HIGH);
    delay(50);
    digitalWrite(buzzerPin,LOW);
} 
void buzzAccessGranted(){
    digitalWrite(buzzerPin,HIGH);
    delay(150);
    digitalWrite(buzzerPin,LOW);
    delay(100);
    digitalWrite(buzzerPin,HIGH);
    delay(150);
    digitalWrite(buzzerPin,LOW);
}
void buzzWrongCode() {
    digitalWrite(buzzerPin,HIGH);
    delay(150);
    digitalWrite(buzzerPin,LOW);
    delay(100);
    digitalWrite(buzzerPin,HIGH);
    delay(150);
    digitalWrite(buzzerPin,LOW);
}