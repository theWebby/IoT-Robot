# include "Ex15.h"
# include "OTA.h"

Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *rightMotor = AFMS.getMotor(rightMotorPort);
Adafruit_DCMotor *leftMotor = AFMS.getMotor(leftMotorPort);
WiFiServer server(LISTEN_PORT);
aREST rest = aREST();

void setup() {
  pinMode(echoPinSide, INPUT);
  pinMode(echoPinFront, INPUT);
  pinMode(trigPinSide, OUTPUT);
  pinMode(trigPinFront, OUTPUT);
  if (printSerial) {
    Serial.begin(115200);
  }
  AFMS.begin();
  initRestRoutes();
  initWiFi();
  initServer();
  setupOTA();
}


void loop() {

  //could put a switch on a webserver for switching manual and autonomous control.
  //manualControl();
  autonomousControl();

  //here is where I can put code for another sensor and submit to a server.
}

//  will follow walls, while sensing and avoiding.
void autonomousControl(){
  //while too close to an object, turn right, otherwise, follow the wall
  
  getDistanceFront();
  if (distanceFront < 7) {
    right("");
    delay(700); //vary depending on where the sensor is facing
  }
  else {
    followWall();
  }
}

//  follows a wall by finding the change in distance moving forward and adjusting its direction.
//  TODO  make the adjustment to the wall proportional to delta distance
void followWall() {
  //clear any turning that was being done before measurements for accuracy
  forward(""); 
  delay(10);

  //record 2 distance measurements while moving forward
  getDistanceSide();
  int lastDuration = durationSide;
  moveForward(0.1);
  getDistanceSide();
  int thisDuration = durationSide;

  // find change in duration (proportional to change in distance)
  int deltaDuration = thisDuration - lastDuration;

    //  debugging
  dln(printSerial, "\n\n");
  dbg(printSerial, "first dur. ");
  dln(printSerial, lastDuration);
  dbg(printSerial, "last dist.  ");
  dln(printSerial, thisDuration);
  dbg(printSerial, "delta dur. ");
  dln(printSerial, deltaDuration);

  //turn to follow the wall
  if (deltaDuration < 0) {
    //getting closer to the wall
    dbg(printSerial, "getting closer");
    bank(0.75); //bank right
  }
  else if (deltaDuration > 0) {
    //further from the wall
    dbg(printSerial, "getting further");
    bank(1.35); //bank left
  }
  else {
    //moving parrallel to the wall
    forward("");
  }

  //how long will turns last
  delay(100);

  ArduinoOTA.handle();
}

//  Handles incomming rest requests to control the robot movement
void manualControl() {
  WiFiClient client = server.available();
  if (!client) {
    //  debugging
    if (!printedOTAMessage) {
      dln(printSerial, "No Client - Handling OTA");
      printedOTAMessage = true;
    }
    
    ArduinoOTA.handle();
    return;
  }

  //  handle client
  printedOTAMessage = false;
  dbg(printSerial, "Client Exists - Waiting for it to become available.");
  
  while (!client.available()) {
    dbg(printSerial, ".");
    delay(1);
  }
  dln(printSerial, "");
  dln(printSerial, "Client Available - Handling Client.");
  rest.handle(client);
}

//  MOVEMENT COMMANDS  

void moveForward(float seconds) {
  forward("");
  delay(seconds * 1000);
}

void moveLeft(float seconds) {
  left("");
  delay(seconds * 1000);
}

void moveRight(float seconds) {
  right("");
  delay(seconds * 1000);
}


//  finds the distance from the side ultrasonic sensor
void getDistanceSide() {
  // Clears the trigPin
  digitalWrite(trigPinSide, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPinSide, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinSide, LOW);

  //records time taken for signal to bounce back and calculate distance
  durationSide = pulseIn(echoPinSide, HIGH);
  distanceSide = (durationSide / 2) / 29.1;

  dbg(printSerial, distanceSide);
  dln(printSerial, "side distance cm");
}

//  finds the distance from the front ultrasonic sensor
void getDistanceFront() {
  // Clears the trigPin
  digitalWrite(trigPinFront, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPinFront, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinFront, LOW);
  durationFront = pulseIn(echoPinFront, HIGH);
  distanceFront = (durationFront / 2) / 29.1;

  dbg(printSerial, distanceFront);
  dln(printSerial, "front distance cm");
}

//  setup server
void initServer() {
  // Start the server
  server.begin();
  dln(printSerial, "Server Started.");

  // Print the IP address
  dln(printSerial, WiFi.localIP());
}

//  setup wifi
void initWiFi() {
  dbg(printSerial, "Connecting to: ");
  dbg(printSerial, ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    dbg(printSerial, ".");
  }
  dln(printSerial, "");
  dln(printSerial, "WiFi connected");
}

//  setup routus to functions
void initRestRoutes() {
  rest.function("stop", stop);
  rest.function("forward", forward);
  rest.function("left", left);
  rest.function("right", right);
  rest.function("backward", backward);
  rest.function("increaseSpeed", increaseSpeed);
  rest.function("decreaseSpeed", decreaseSpeed);
}

//  increases the speed by one until max
int increaseSpeed(String command) {
  if (speed < 255) {
    speed = speed + 1;
    printSpeed();
  }
  else {
    dln(printSerial, "MAX SPEED");
  }
}

//  decreases the speed by one until the minimum
int decreaseSpeed(String command) {
  if (speed > 0) {
    speed = speed - 1;
    printSpeed();
  }
  else {
    dln(printSerial, "MAX SPEED");
  }
}


//  Set the motors moving forward at different speeds to turn the robot by a given factor
//note   - factor of 1.2 reduces left wheel by 0.2 and increases right by 0.2 current speed 
//       - factor < 1 will bank right, factor of 1 is forward
int bank(float speedFactor) {
  //  debugging
  if (speedFactor > 1) {
    dln(printSerial, "Banking left");
  }
  else if (speedFactor < 1) {
    dln(printSerial, "Banking right");
  }
  else {
    dln(printSerial, "Forward");
  }

  // set motors speeds
  leftMotor-> setSpeed(speed - round((speedFactor - 1) * speed));
  leftMotor-> run(FORWARD);
  rightMotor->setSpeed(speed + round((speedFactor - 1) * speed));
  rightMotor->run(FORWARD);
}


//    BASIC MOVEMENT CONTROLS  

int forward(String command) {
  dln(printSerial, "FORWARD");
  leftMotor->setSpeed(speed);
  leftMotor->run(FORWARD);
  rightMotor->setSpeed(speed);
  rightMotor->run(FORWARD);
}

int backward(String command) {
  dln(printSerial, "BACKWARD");
  leftMotor->setSpeed(speed);
  leftMotor->run(BACKWARD);
  rightMotor->setSpeed(speed);
  rightMotor->run(BACKWARD);
}

int left(String command) {
  dln(printSerial, "LEFT");
  leftMotor->setSpeed(speed);
  leftMotor->run(BACKWARD);
  rightMotor->setSpeed(speed);
  rightMotor->run(FORWARD);
}

int right(String command) {
  dln(printSerial, "RIGHT");
  leftMotor->setSpeed(speed);
  leftMotor->run(FORWARD);
  rightMotor->setSpeed(speed);
  rightMotor->run(BACKWARD);
}

int stop(String command) {
  dln(printSerial, "STOP");
  leftMotor->run(RELEASE);
  rightMotor->run(RELEASE);
}

//  debugging
void printSpeed() {
  dbg(printSerial, "Speed: ");
  dln(printSerial, speed);
}


