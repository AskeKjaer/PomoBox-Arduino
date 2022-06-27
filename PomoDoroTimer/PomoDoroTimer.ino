#include <LiquidCrystal.h>
//init LCD
LiquidCrystal lcd(9, 8, 7, 6, 5, 4);
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
int startPin = 3;
bool timerActive = 0;
//values for timing
unsigned long startTime = 0;
unsigned long displayTime = 0;
int elapsedMinutes = 0;
int elapsedSeconds = 0;
int timerStep = 0;
//Define Pizo-pin, switch & buzz duration
int buzzPin = 11;
int buzzDur = 400;
int buzzActive = 0;
int buzzSwitch = 2;
int buzzLED = A2;
int buzzFreq = 2500;
//define LED pins
int offLED = 13;
int workLED = A0;
int breakLED1 = A1;
int breakLED2 = 12;
void setup() {
  //initiate serial connection for debugging
Serial.begin(9600);
  //define pinModes
pinMode(startPin, INPUT);
pinMode(offLED, OUTPUT);
pinMode(workLED, OUTPUT);
pinMode(breakLED1, OUTPUT);
pinMode(breakLED2, OUTPUT);
pinMode(buzzSwitch, INPUT);
pinMode(buzzLED, OUTPUT);
//initialise screen
lcd.begin(16,2);
lcd.print("Screen ready!");
}

//Buzzer functions
void longBuzz (){
  tone(buzzPin, buzzFreq/2, buzzDur);
}

void shortBuzz (){
 tone(buzzPin, buzzFreq, (buzzDur/2));
}

void doubleBuzz (){
  tone(buzzPin, buzzFreq, (buzzDur/4));
  delay(buzzDur/2);
  tone(buzzPin, buzzFreq*1.3, (buzzDur/4));
}

void tripleBuzz(){
  tone(buzzPin, buzzFreq, (buzzDur/8));
  delay(buzzDur/4);
  tone(buzzPin, buzzFreq*1.3, (buzzDur/8));
  delay(buzzDur/4);
  tone(buzzPin, buzzFreq*1.5, (buzzDur/8));
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
//Screen functions
void displayRuntime(int m, int s){
      lcd.setCursor(0, 1);
    lcd.print(m);
    if(m < 10){
      lcd.setCursor(1,1);
      lcd.print(":");
      lcd.print(s);
    }
    if(m >= 10){
     lcd.setCursor(2,1);
     lcd.print(":");
     lcd.print(s); 
    }
}

void loop() {
  //read & map potentiometers to get timings
  //adding an if loop to only allow adjustment if timer is off - if not, I'm pretty sure
  //something will break spectacularly
  if(timerActive == 0){
potentOutput1 = analogRead(PotentPin1);
workTime = map(potentOutput1, 0, 1023, 15, 36);
potentOutput2 = analogRead(PotentPin2);
shortBreakTime = map(potentOutput2, 0, 1023, 1, 11);
potentOutput3 = analogRead(PotentPin3);
longBreakTime = map(potentOutput3, 0, 1023, 10, 31);
  }
  //convert values to timing longs if there is a change
  //and print them in serial for debugging
if (workTime != workTimeOld){
  lcd.clear();
  lcd.print("Work time:");
  lcd.setCursor(0, 1);
  lcd.print(workTime);
  lcd.setCursor(3, 1);
  lcd.print("Min");
  Serial.println(" Work Time: ");
  Serial.println(workTime);
  workMillis = workTime*60000;
  Serial.println(" Or in ms: ");
  Serial.println(workMillis);
  workTimeOld = workTime;
}
if (shortBreakTime != shortBreakTimeOld){
  lcd.clear();
  lcd.print("S. break time:");
  lcd.setCursor(0, 1);
  lcd.print(shortBreakTime);
  lcd.setCursor(3, 1);
  lcd.print("Min");
  Serial.println(" Short Break: ");
  Serial.println(shortBreakTime);
  shortBreakMillis = shortBreakTime*60000;
  Serial.println(" Or in ms: ");
  Serial.println(shortBreakMillis);
  shortBreakTimeOld = shortBreakTime;
}
if (longBreakTime != longBreakTimeOld){
    lcd.clear();
  lcd.print("L. break time:");
  lcd.setCursor(0, 1);
  lcd.print(longBreakTime);
  lcd.setCursor(3, 1);
  lcd.print("Min");
  Serial.println(" Long Break ");
  Serial.println(longBreakTime);
  longBreakMillis = longBreakTime*60000;
  Serial.println(" Or in ms: ");
  Serial.println(longBreakMillis);
  longBreakTimeOld = longBreakTime;
}
//Define buzzer switch & LED functions
if(buzzActive == 1 && digitalRead(buzzSwitch) == HIGH){
  Serial.println("Buzzer Off!");
  lcd.clear();
  lcd.print("Sound off!");
  buzzActive = 0;
  delay(300);
}
if(buzzActive == 0 && digitalRead(buzzSwitch) == HIGH){
  buzzActive = 1;
  Serial.println("Buzzer On!");
  lcd.clear();
  lcd.print("Sound on!");
  tripleBuzz();
  delay(300);
}
switch(buzzActive){
  case 0:
  digitalWrite(buzzLED, LOW);
  break;
  case 1:
  digitalWrite(buzzLED, HIGH);
  break;
  default:
  break;
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
  lcd.clear();
  lcd.print("Timer stopped!");
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
//the buzzing is also in here - modify the Buzz() functions if
//you want to fuck around with piezo.

if (timerActive == 1 && millis() < (startTime+workMillis) && timerStep == 0){
  //start by limiting feedback to once every second
  if(millis() >= (displayTime+1000)) {
    Serial.println("Work time! ");
    Serial.println("Time Elapsed: ");
    Serial.print((millis()-startTime)/60000);
    Serial.print(" minutes");
    //Screen code
    elapsedMinutes = ((millis()-startTime)/60000);
    elapsedSeconds = (((millis()-startTime)/1000)-(elapsedMinutes*60));
    lcd.clear();
    lcd.print("Work Time:");
    displayRuntime(elapsedMinutes, elapsedSeconds);
    displayTime = millis();
  }
}
  //iterate timerStep when time elapses
  if(millis() >= (startTime+workMillis) && timerStep == 0 && timerActive == 1){
    timerStep++;
    startTime = startTime+workMillis;
    if(buzzActive == 1){
      shortBuzz();
    }
    Serial.println("Break time! ");
  }
if (timerActive == 1 && millis() < startTime && timerStep == 1){
  //start by limiting feedback to once every second
  if(millis() >= (displayTime+1000)) {
    Serial.println("Break time! ");
    Serial.println("Time Elapsed: ");
    Serial.print((millis()-startTime)/60000);
    Serial.print(" minutes");
    lcd.clear();
    lcd.print("Break Time:");
    lcd.setCursor(0, 1);
    elapsedMinutes = ((millis()-startTime)/60000);
    elapsedSeconds = (((millis()-startTime)/1000)-(elapsedMinutes*60));
    displayRuntime(elapsedMinutes, elapsedSeconds);
    displayTime = millis();
}
}
if(millis() >= startTime+shortBreakMillis && timerStep == 1 && timerActive == 1){
  Serial.println("Back to work! ");
  if(buzzActive == 1){
    doubleBuzz();
  }
  timerStep++;
  startTime = startTime+shortBreakMillis;
}
if(millis() >= startTime && timerStep == 2 && timerActive == 1){
    if(millis() >= (displayTime+1000)) {
    elapsedMinutes = ((millis()-startTime)/60000);
    elapsedSeconds = (((millis()-startTime)/1000)-(elapsedMinutes*60));
    lcd.clear();
    lcd.print("Back to work!");
    displayRuntime(elapsedMinutes, elapsedSeconds);
    displayTime = millis();
  }
}
if(millis() >= startTime+workMillis && timerStep == 2 && timerActive == 1){
  Serial.println("Second break time! ");
  if(buzzActive == 1){
    shortBuzz();
  }
  timerStep++;
  startTime = startTime+workMillis;
}
if(millis() >= startTime && timerStep == 3 && timerActive == 1){
  if(millis() >= (displayTime+1000)) {
    elapsedMinutes = ((millis()-startTime)/60000);
    elapsedSeconds = (((millis()-startTime)/1000)-(elapsedMinutes*60));
    lcd.clear();
    lcd.print("Second break!");
    displayRuntime(elapsedMinutes, elapsedSeconds);
    displayTime = millis();
  }
}
if(millis() >= startTime+shortBreakMillis && timerStep == 3 && timerActive == 1){
   Serial.println("Back to work! ");
   if(buzzActive == 1){
    doubleBuzz();
   }
   timerStep++;
   startTime = startTime+shortBreakMillis;
   
}
if(millis() >= startTime && timerStep == 4 && timerActive == 1){
  if(millis() >= (displayTime+1000)) {
    Serial.println("Work time! ");
    elapsedMinutes = ((millis()-startTime)/60000);
    elapsedSeconds = (((millis()-startTime)/1000)-(elapsedMinutes*60));
    lcd.clear();
    lcd.print("Work time!");
    displayRuntime(elapsedMinutes, elapsedSeconds);
    displayTime = millis();
  }
}
if(millis() >= startTime+workMillis && timerStep == 4 && timerActive == 1){
  Serial.println("Third break time! ");
  timerStep++;
  startTime = startTime+workMillis;
  if(buzzActive == 1){
    shortBuzz();
  }
}
if((millis() >= startTime) && timerStep == 5 && timerActive == 1){
  if(millis() >= (displayTime+1000)) {
    Serial.println("Break time! ");
    elapsedMinutes = ((millis()-startTime)/60000);
    elapsedSeconds = (((millis()-startTime)/1000)-(elapsedMinutes*60));
    lcd.clear();
    lcd.print("Break!");
    displayRuntime(elapsedMinutes, elapsedSeconds);
    displayTime = millis();
  }
}
if(millis() >= startTime+shortBreakMillis && timerStep == 5 && timerActive == 1){
  Serial.println("Back to work, one last time! ");
  if(buzzActive == 1){
    doubleBuzz();
  }
  timerStep++;
  startTime = startTime+shortBreakMillis;
}
if(millis() >= startTime && timerStep == 6 && timerActive == 1){
  if(millis() >= (displayTime+1000)) {
    Serial.println("Work time!");
    Serial.println("Time Elapsed: ");
    Serial.print((millis()-(startTime+(3*workMillis)+(3*shortBreakMillis)))/60000);
    Serial.print(" minutes");
    elapsedMinutes = ((millis()-startTime)/60000);
    elapsedSeconds = (((millis()-startTime)/1000)-(elapsedMinutes*60));
    lcd.clear();
    lcd.print("Last work:");
    displayRuntime(elapsedMinutes, elapsedSeconds);
    displayTime = millis();
  }
}
if(millis() >= startTime+workMillis && timerStep == 6 && timerActive == 1){
  Serial.println("Time for a longer break! ");
  timerStep++;
  startTime = startTime+workMillis;
  if(buzzActive == 1){
    shortBuzz();
  }
}
if(millis() >= startTime && timerStep == 7 && timerActive == 1){
  if(millis() >= (displayTime+1000)) {
    Serial.println("Long break time! ");
    Serial.println("Time Elapsed: ");
    Serial.print((millis()-(startTime+(4*workMillis)+(3*shortBreakMillis)))/60000);
    Serial.print(" minutes");
    elapsedMinutes = ((millis()-startTime)/60000);
    elapsedSeconds = (((millis()-startTime)/1000)-(elapsedMinutes*60));
    lcd.clear();
    lcd.print("Long break!");
    displayRuntime(elapsedMinutes, elapsedSeconds);
    displayTime = millis();
}
}
if(millis() >= startTime+longBreakMillis && timerStep == 7 && timerActive == 1) {
  Serial.println("Pomo session over! ");
  lcd.clear();
  lcd.print("Pomo session");
  lcd.setCursor(0, 1);
  lcd.print("complete!");
  if(buzzActive == 1){
    longBuzz();
  }
  timerStep = 0;
  timerActive = 0;
}
}
