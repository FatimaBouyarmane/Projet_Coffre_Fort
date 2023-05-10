#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <Keypad.h>
 
#define SS_PIN 53
#define RST_PIN 5
#define SERVO_PIN 3
#define MAX_ATTEMPTS 3

#define ACCESS_DELAY 2000
#define DENIED_DELAY 1000



Servo myservo;
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.




 int R=6;
 int V=7;
 int buzzer=4;
 int wrongAttempts=0;
 int rightAttempts=0;
 int wrongpsw=0;  
//Pad
char* password = "888"; // change the password here, just pick any 3 numbers
int position = 0;
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
{'1','2','3','A'},
{'4','5','6','B'},
{'7','8','9','C'},
{'*','0','#','D'}
};
byte rowPins[ROWS] = { 21, 20, 19, 18 };
byte colPins[COLS] = { 17, 16, 15, 14 };
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
int RedpinLock = 12;
int GreenpinUnlock = 13;



void setup() 
{
  Serial.begin(9600);   // Initiate a serial communication
  SPI.begin();          // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  myservo.attach(SERVO_PIN);
  Serial.println("Put your card to the reader...");
  Serial.println();
  pinMode(V,OUTPUT);
  pinMode(R,OUTPUT);
  pinMode(buzzer,OUTPUT);
  LockedPosition(true);

}




void loop() {



  char key = keypad.getKey();
   if (key == '*' || key == '#') 
   {
      position = 0;
      LockedPosition(true);
   }
   if (key == password[position])
   {
      position ++;
   }
   if (position == 3)
   {
      LockedPosition(false);
   }

  delay(100);

  
  digitalWrite(buzzer,HIGH);
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  if (content.substring(1) == "FA 39 27 28") //change here the UID of the card
  {
    UnlockDoor();
  }
 else   
  {
   LockDoor();

  }

}



//fonctions


void UnlockDoor(){
    rightAttempts++;    
    Serial.println("Access autorise");
    digitalWrite(V,HIGH);
    Serial.println();
    delay(3500);
    digitalWrite(V,LOW);
    myservo.write( 0 );
    Serial.println("the door is opened");
    delay(3000);
    if (rightAttempts % 2==0)
    {
    myservo.write( 180 );
    Serial.println("the door is closed");
    }
  }

//fonction pour fem
  void LockDoor(){

    Serial.println(" Access non autorise");
     wrongAttempts++;
    digitalWrite(R,HIGH);
    for(int i=0;i<MAX_ATTEMPTS;i++)
    {
         digitalWrite(R,HIGH);
         delay(500);
         digitalWrite(R,LOW);
         if (wrongAttempts>= MAX_ATTEMPTS)
         {
                        
            digitalWrite(R,HIGH)  ;                    
            digitalWrite(buzzer,LOW);//l3aks
            delay(5000);
            digitalWrite(buzzer,HIGH);
            digitalWrite(R,LOW);
            wrongAttempts=0;
         }      
    }
    delay(DENIED_DELAY);
    
  }

  

void LockedPosition(int locked){
    if (locked)
    {
       digitalWrite(RedpinLock, HIGH);
       digitalWrite(GreenpinUnlock, LOW);
       myservo.write(180);
       digitalWrite(V,HIGH);
       digitalWrite(buzzer,LOW);
      delay(300);   
      digitalWrite(buzzer,HIGH);  
      digitalWrite(V,LOW); 
  
    }
    else
    {
             
      if(password="888"){          
      digitalWrite(RedpinLock, LOW);
      digitalWrite(GreenpinUnlock, HIGH);
      myservo.write(0);
      digitalWrite(V,HIGH);
      digitalWrite(buzzer,LOW);      
      delay(300);   
      digitalWrite(buzzer,HIGH);
      digitalWrite(V,LOW); 
      }
      else {
        LockDoor();
        }
      
    }

 }

/*
void TestPassword(){
   if (password=="888") {
      LockedPosition(false);  
   }

   else {
      wrongpsw++;      
      digitalWrite(R,HIGH); 
      if (wrongpsw >= 3)
      {
         digitalWrite(buzzer,LOW);
         delay(3000);
         digitalWrite(buzzer,HIGH); 
      } 
      digitalWrite(R,LOW);          
    }     
   
}

*/


