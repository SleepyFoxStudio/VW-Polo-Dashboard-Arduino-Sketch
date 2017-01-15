
// constants won't change. Used here to set a pin number :
const int rpmPin =  2;      // the number of the LED pin
const int oilPin =  3;      // the number of the LED pin
const int speedPin =  4;      // the number of the LED pin

// Variables will change :
int mphState = LOW;             // ledState used to set the LED
int rpmState = LOW;             // ledState used to set the LED

int rpm = 0;
int mph = 0;

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMphMicros = 0;        // will store last time LED was updated
unsigned long previousRpmMicros = 0;        // will store last time LED was updated
unsigned long previousStatusLightsMicros = 0;

//long mphInterval = 0;
//long rpmInterval = 300000/1;

long rpmDivisor = 150000;
long mphDivisor = 300000;


boolean oilFault = false;
String commandBuffer;

void processCommand()
{
  if (commandBuffer.startsWith("SPD "))
  {
    commandBuffer.remove(0, 4);
    mph = commandBuffer.toInt();
    if (mph < 1)
      mph = 1;
    Serial.println("Ok.");
    Serial.print("Speed set to ");
    Serial.print(mph, DEC);
    Serial.println(" mph.");
  }
  else  if (commandBuffer.startsWith("RPM "))
  {
    commandBuffer.remove(0, 4);
    rpm = commandBuffer.toInt();
    if (rpm < 1)
      rpm = 1;
    Serial.println("Ok.");
    Serial.print("RPM set to ");
    Serial.print(rpm, DEC);
    Serial.println(" rpm.");
  }
  else  if (commandBuffer.startsWith("OIL "))
  {
    commandBuffer.remove(0, 4);

    if (commandBuffer.toInt() == 1)
    {
      oilFault = true;
    }
    else
    {
      oilFault = false;
    }

    Serial.println("Ok.");
    Serial.print("Oil set to ");
    Serial.print(oilFault, DEC);
    Serial.println(".");
  }
  else if (commandBuffer.indexOf("STATUS") > 0)
  {
    Serial.println("Ok.");
  }



  else
  {
    Serial.println("Error.");
    Serial.print("Unknown commmand: ");
    Serial.print(commandBuffer);
    Serial.print('\n');
  }
}

void setup() {
  // set the digital pin as output:

  pinMode(rpmPin, OUTPUT);
  pinMode(oilPin, OUTPUT);
  pinMode(speedPin, OUTPUT);
  digitalWrite(oilPin, HIGH);
  Serial.begin(9600);

}

void loop() {
  while (Serial.available() > 0) {
    int inChar = Serial.read();
    Serial.write(inChar);

    if (inChar == '\n')
    {
      processCommand();
      commandBuffer = "";
    }
    else
    {
      commandBuffer += (char)inChar;
    }
  }

  unsigned long currentMicros = micros();
  //Do MPH Logic
  if (currentMicros - previousMphMicros >= mphDivisor / mph && mph > 0) {
    // save the last time you updated
    previousMphMicros = currentMicros;

    // if the LED is off turn it on and vice-versa:
    if (mphState == LOW) {
      mphState = HIGH;
    } else {
      mphState = LOW;
    }

    // set the LED with the ledState of the variable:
    digitalWrite(speedPin, mphState);
  }

  //Do the RPM logic
  if (currentMicros - previousRpmMicros >= rpmDivisor / rpm && rpm > 0) {

    // save the last time you updated
    previousRpmMicros = currentMicros;

    // if the LED is off turn it on and vice-versa:
    if (rpmState == LOW) {
      rpmState = HIGH;
    } else {
      rpmState = LOW;
      // set the LED with the ledState of the variable:
    }
    digitalWrite(rpmPin, rpmState);
  }



  //Update the status lights every half second
  if (currentMicros - previousStatusLightsMicros >= 500000) {
    Serial.println("updating status lights");
    previousStatusLightsMicros = currentMicros;
    if (oilFault)
    {
      if (rpm > 5)
      {
        digitalWrite(oilPin, LOW);
      }
      else
      {
        digitalWrite(oilPin, HIGH);
      }
    }
    if (!oilFault)
    {
      if (rpm > 5)
      {
        digitalWrite(oilPin, HIGH);
      }
      else
      {
        digitalWrite(oilPin, LOW);
      }
    }
  }
}
