#include <SPI.h>

//Add the SdFat Libraries
#include <SdFat.h>
#include <SdFatUtil.h>

//and the MP3 Shield Library
#include <SFEMP3Shield.h>

SdFat sd;
SFEMP3Shield MP3player;


//Turn on LED while reed switch is closed

#define LED 13     // pin for the LED
#define BUTTON 5  // input for BUTTON
#define SWITCH 3  // input for REED SWITCH
#define SONG "11.mp3"
long randNumber;

long millis_held;         // millisec button is held
long sec_held;            // How long the button was held (seconds)
long prev_sec_held;       // how long the button was held in the previous check
unsigned long firstTime;  // time the button was first pressed

int state;        // state of button, LOW is pushed
int door_state;
int butt_state;
byte previous_state = LOW;


void setup() {
  Serial.begin(115200);     // set up Serial library at 115200 bps
  pinMode(LED, OUTPUT);   //tell arduino LED is an output
  digitalWrite(BUTTON, HIGH);  // since we are using a 10k pull-up this is set to HIGH
  digitalWrite(SWITCH, HIGH);   //SWITCH is input
  
  //pinMode(BUTTON, INPUT);   //BUTTON is input
  
  uint8_t result; //result code from some function as to be tested at later time
  //Initialize the SdCard.
  if(!sd.begin(SD_SEL, SPI_FULL_SPEED)) sd.initErrorHalt();
  // depending upon your SdCard environment, SPI_HAVE_SPEED may work better.
  if(!sd.chdir("/")) sd.errorHalt("sd.chdir");

  //Initialize the MP3 Player Shield
  result = MP3player.begin();
  //check result, see readme for error codes.
  if(result != 0) {
    Serial.print(F("Error code: "));
    Serial.print(result);
    Serial.println(F(" when trying to start MP3 player"));
    if( result == 6 ) {
      Serial.println(F("Warning: patch file not found, skipping.")); // can be removed for space, if needed.
      Serial.println(F("Use the \"d\" command to verify SdCard can be read")); // can be removed for space, if needed.
    }
  }
}

void loop(){  
  /*
  door_state = digitalRead(SWITCH);
  butt_state = digitalRead(BUTTON);

  if (butt_state == LOW || door_state == LOW){
    state = LOW;
    Serial.println("Low state");
  }
  else{
    state = previous_state;
  }
  */
  state=digitalRead(SWITCH);
  if(state == LOW && previous_state == HIGH && (millis() - firstTime > 200)){
    firstTime = millis();
    Serial.println("Now LOW was HIGH");
  }

  millis_held = (millis() - firstTime);
  sec_held = millis_held / 1000;

  if (millis_held > 50){
    
    if (state == LOW && sec_held > prev_sec_held){
      ledblink(1, 50, LED); //
      Serial.println("Door Closed ");    
    }
    

    if (state == HIGH && previous_state == LOW){
      Serial.println("Door Opened ");
      playSONG();
    }
  }

  previous_state = state;
  prev_sec_held = sec_held;
}


// Just a simple helper function to blink an led in various patterns
void ledblink(int times, int lengthms, int pinnum){
  for (int x=0; x<times;x++) {
    digitalWrite(pinnum, HIGH);
    delay (lengthms);
    digitalWrite(pinnum, LOW);
    delay(lengthms);
  }
}


void playSONG()
{
    randNumber = random(10, 45); //Remember we named all the mp3s between 10 and 45?
    String testString = String(randNumber);  //Set string equal to that random number you just got
    testString += ".mp3";  //add .mp3 to the end of it
    testString.toCharArray(SONG,7); //converts the string into a character array
    Serial.println(SONG);
    SdFile file;
    file.getFilename(SONG);
    MP3player.playMP3(SONG);
    Serial.println("playing " SONG);
    file.close();
    return;
}
