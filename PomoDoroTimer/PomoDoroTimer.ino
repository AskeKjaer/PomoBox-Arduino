//define potentiometers for setting, define timings for mapping,
//unsigned longs for timing (I hate millis)
int PotentPin1 = A5;
int potentOutput1 = 0;
int workTime = 0;
int workTimeOld = 0;
int PotentPin2 = A4;
int potentOutput2 = 0;
int shortBreakTime = 0;
int shortBreakTimeOld = 0;
int PotentPin3 = A3;
int potentOutput3 = 0;
int longBreakTime = 0;
int longBreakTimeOld = 0;
unsigned long workMillis = 0;
unsigned long shortBreakMillis = 0;
unsigned long longBreakMillis = 0;
//define bools and pins for start switch
int startPin = 2;
bool timerActive = 0;
//values for timing
unsigned long startTime = 0;
unsigned long displayTime = 0;
int timerStep = 0;
//Define Pizo-pin, switch & buzz duration
int buzzPin = 7;
int buzzDur = 400;
int buzzActive = 1;
int buzzSwitch = 0;
int buzzFreq = 2500;
//define LED pins
int offLED = 13;
int workLED = 5;
int breakLED1 = 4;
int breakLED2 = 6;
void setup() {
  //initiate serial connection for debugging
Serial.begin(9600);
  //define pinModes
pinMode(startPin, INPUT);
pinMode(offLED, OUTPUT);
pinMode(workLED, OUTPUT);
pinMode(breakLED1, OUTPUT);
pinMode(breakLED2, OUTPUT);
}

//Buzzer functions
void longBuzz (){
  tone(buzzPin, buzzFreq, buzzDur);
}

void shortBuzz (){
 tone(buzzPin, buzzFreq, (buzzDur/2));
}

void doubleBuzz (){
  tone(buzzPin, buzzFreq, (buzzDur/4));
  delay(buzzDur/2);
  tone(buzzPin, buzzFreq, (buzzDur/4));
}
//LED functions
void stbyLED(){
  digitalWrite(offLED, HIGH);
  digitalWrite(workLED, LOW);
  digitalWrite(breakLED1, LOW);
  digitalWrite(breakLED2, LOW);
}
void workLEDMode(){
  digitalWrite(offLED, LOW);
  digitalWrite(workLED, HIGH);
  digitalWrite(breakLED1, LOW);
  digitalWrite(breakLED2, LOW);
}
void shortBreakLED(){
  digitalWrite(offLED, LOW);
  digitalWrite(workLED, LOW);
  digitalWrite(breakLED1, HIGH);
  digitalWrite(breakLED2, LOW);
}
void longBreakLED(){
  digitalWrite(offLED, LOW);
  digitalWrite(workLED, LOW);
  digitalWrite(breakLED1, HIGH);
  digitalWrite(breakLED2, HIGH);
}

void loop() {
  //read & map potentiometers to get timings
  //adding an if loop to only allow adjustment if timer is off - if not, I'm pretty sure
  //something will break spectacularly
  if(timerActive == 0){
potentOutput1 = analogRead(PotentPin1);
workTime = map(potentOutput1, 0, 1023, 3, 30);
potentOutput2 = analogRead(PotentPin2);
shortBreakTime = map(potentOutput2, 0, 1023, 1, 10);
potentOutput3 = analogRead(PotentPin3);
longBreakTime = map(potentOutput3, 0, 1023, 2, 30);
  }
  //convert values to timing longs if there is a change
  //and print them in serial for debugging
if (workTime != workTimeOld){
  Serial.println(" Work Time: ");
  Serial.println(workTime);
  workMillis = workTime*60000;
  Serial.println(" Or in ms: ");
  Serial.println(workMillis);
  workTimeOld = workTime;
}
if (shortBreakTime != shortBreakTimeOld){
  Serial.println(" Short Break: ");
  Serial.println(shortBreakTime);
  shortBreakMillis = shortBreakTime*60000;
  Serial.println(" Or in ms: ");
  Serial.println(shortBreakMillis);
  shortBreakTimeOld = shortBreakTime;
}
if (longBreakTime != longBreakTimeOld){
  Serial.println(" Long Break ");
  Serial.println(longBreakTime);
  longBreakMillis = longBreakTime*60000;
  Serial.println(" Or in ms: ");
  Serial.println(longBreakMillis);
  longBreakTimeOld = longBreakTime;
}
//start timer if button is pressed & timer is off
if (digitalRead(startPin) == HIGH && timerActive == 0){
  timerActive = 1;
  Serial.println("Timer Active! ");
  Serial.print(workTime);
  Serial.println(" minutes of work, ");
  Serial.print(shortBreakTime);
  Serial.println(" minutes of short break, ");
  Serial.print(longBreakTime);
  Serial.println(" minutes of long break time. ");
  startTime = millis();
  if(buzzActive == 1){
    longBuzz();
  }
  delay(300);
}
//stop timer if button is pressed and timer is running
if (digitalRead(startPin) == HIGH && timerActive == 1){
  timerActive = 0;
  timerStep = 0;
  if(buzzActive == 1){
    longBuzz();
  }
  Serial.println("Timer stopped & reset! ");
  delay(300);
}
//LED control code
if(timerActive == 0){
  stbyLED();
}
else{
  digitalWrite(offLED, LOW);
  switch (timerStep){
    case 0:
      workLEDMode();
      break;
    case 1:
      shortBreakLED();
      break;
     case 2:
      workLEDMode();
      break;
     case 3: 
      shortBreakLED();
      break;
     case 4:
      workLEDMode();
      break;
     case 5:
      shortBreakLED();
      break;
     case 6:
      workLEDMode();
      break;
     case 7:
      longBreakLED();
      break;
     default:
      break;
  }
}
//NOTICE: Below this is a truly extraordinary mess of godforsaken spaghetti code.
//If you need to call something, use timerActive & timerStep to find the state. Don't touch this.
//This should be illegal. It might very well be illegal. That being said, timer logic!
//a quick word on timerStep - it shows how far we are in the Pomo schedule. 0 is first work,
//and 7 is the final, long break. Use this if you want to see how far we are, but don't call it unless you know the timer
//is active - in stdby mode, timerStep == 0 without the timer running.
//the buzzing is also in here - modify the longBuzz(), shortBuzz() and doubleBuzz() functions if
//you want to fuck around with piezo.

if (timerActive == 1 && millis() < (startTime+workMillis) && timerStep == 0){
  //start by limiting feedback to once every 30 seconds
  if(millis() >= (displayTime+30000)) {
    Serial.println("Work time! ");
    Serial.println("Time Elapsed: ");
    Serial.print((millis()-startTime)/60000);
    Serial.print(" minutes");
    displayTime = millis();
  }
}
  //iterate timerStep when time elapses
  if(millis() >= (startTime+workMillis) && timerStep == 0 && timerActive == 1){
    timerStep++;
    if(buzzActive == 1){
      shortBuzz();
    }
    Serial.println("Break time! ");
  }
if (timerActive == 1 && millis() < (startTime+workMillis+shortBreakMillis) && timerStep == 1){
  //start by limiting feedback to once every 30 seconds
  if(millis() >= (displayTime+30000)) {
    Serial.println("Break time! ");
    Serial.println("Time Elapsed: ");
    Serial.print((millis()-(startTime+workMillis))/60000);
    Serial.print(" minutes");
    displayTime = millis();
}
}
if(millis() >= startTime+workMillis+shortBreakMillis && timerStep == 1 && timerActive == 1){
  Serial.println("Back to work! ");
  if(buzzActive == 1){
    doubleBuzz();
  }
  timerStep++;
}
if(millis() >= startTime+workMillis+shortBreakMillis && timerStep == 2 && timerActive == 1){
    if(millis() >= (displayTime+30000)) {
    Serial.println("Work time! ");
    Serial.println("Time Elapsed: ");
    Serial.print((millis()-(startTime+workMillis+shortBreakMillis))/60000);
    Serial.print(" minutes");
    displayTime = millis();
  }
}
if(millis() >= startTime+(2*workMillis)+shortBreakMillis && timerStep == 2 && timerActive == 1){
  Serial.println("Second break time! ");
  timerStep++;
}
if(millis() >= startTime+(2*workMillis)+shortBreakMillis && timerStep == 3 && timerActive == 1){
  if(millis() >= (displayTime+30000)) {
    Serial.println("Break time! ");
    Serial.println("Time Elapsed: ");
    Serial.print((millis()-(startTime+(2*workMillis)+shortBreakMillis))/60000);
    Serial.print(" minutes");
    displayTime = millis();
  }
}
if(millis() >= startTime+(2*workMillis)+(2*shortBreakMillis) && timerStep == 3 && timerActive == 1){
   Serial.println("Back to work! ");
   timerStep++;
}
if(millis() >= startTime+(2*workMillis)+(2*shortBreakMillis) && timerStep == 4 && timerActive == 1){
  if(millis() >= (displayTime+30000)) {
    Serial.println("Work time! ");
    Serial.println("Time Elapsed: ");
    Serial.print((millis()-(startTime+(2*workMillis)+(2*shortBreakMillis)))/60000);
    Serial.print(" minutes");
    displayTime = millis();
  }
}
if(millis() >= startTime+(3*workMillis)+(2*shortBreakMillis) && timerStep == 4 && timerActive == 1){
  Serial.println("Third break time! ");
  timerStep++;
}
if(millis() >= startTime+(3*workMillis)+(2*shortBreakMillis) && timerStep == 5 && timerActive == 1){
  if(millis() >= (displayTime+30000)) {
    Serial.println("Break time! ");
    Serial.println("Time Elapsed: ");
    Serial.print((millis()-(startTime+(3*workMillis)+(2*shortBreakMillis)))/60000);
    Serial.print(" minutes");
    displayTime = millis();
  }
}
if(millis() >= startTime+(3*workMillis)+(3*shortBreakMillis) && timerStep == 5 && timerActive == 1){
  Serial.println("Back to work, one last time! ");
  timerStep++;
}
if(millis() >= startTime+(3*workMillis)+(3*shortBreakMillis) && timerStep == 6 && timerActive == 1){
  if(millis() >= (displayTime+30000)) {
    Serial.println("Work time! ");
    Serial.println("Time Elapsed: ");
    Serial.print((millis()-(startTime+(3*workMillis)+(3*shortBreakMillis)))/60000);
    Serial.print(" minutes");
    displayTime = millis();
  }
}
if(millis() >= startTime+(4*workMillis)+(3*shortBreakMillis) && timerStep == 6 && timerActive == 1){
  Serial.println("Time for a longer break! ");
  timerStep++;
}
if(millis() >= startTime+(4*workMillis)+(3*shortBreakMillis) && timerStep == 7 && timerActive == 1){
  if(millis() >= (displayTime+30000)) {
    Serial.println("Long break time! ");
    Serial.println("Time Elapsed: ");
    Serial.print((millis()-(startTime+(4*workMillis)+(3*shortBreakMillis)))/60000);
    Serial.print(" minutes");
    displayTime = millis();
}
}
if(millis() >= startTime+(4*workMillis)+(3*shortBreakMillis)+longBreakMillis && timerStep == 7 && timerActive == 1) {
  Serial.println("Pomo session over! ");
  timerStep = 0;
  timerActive = 0;
}
}
