

#include <Servo.h>

//TimedAction
//allows us to set actions to perform on separate timed intervals
//http://playground.arduino.cc/Code/TimedAction
//http://wiring.uniandes.edu.co/source/trunk/wiring/firmware/libraries/TimedAction
#include <TimedAction.h>
//NOTE: This library has an issue on newer versions of Arduino. After
//      downloading the library you MUST go into the library directory and
//      edit TimedAction.h. Within, overwrite WProgram.h with Arduino.h


// maybe use TimedAction (interrupts) for the tastks


// pin definition
const int g1 = 3;
const int g2 = 9;
const int r1 = 11;
const int r2 = 5;
const int y1 = 10;
const int y2 = 6;
const int button = 2;
const int servoPin = 12;

Servo servo;

const int centre = 85;
const int swing = 15;
const int left = centre + swing;
const int right = centre - swing;
const int s = 400;  // single delay in ms
const int d = 2*s;  // double delau in ms

// button state
volatile int buttonPressed;
// LED mode declaration
enum mode{all, blink_all, blink_color, snake};

// mode selection
int curr_mode;

//debounce variables
int debounce = 200;       // wanted debounce time in milliseconds
unsigned long last_updated = 0;     //timestamp of previous interrupt
unsigned long curr_time;      // current time in as reported by millis()

// do_all variables
const int mode_duration = 5000;    // in milliseconds
int mode_count = 3; //need to subtract 1 from the actual mode count as "all" is not used here

// mode parameters
const int blink_duration = 100;  // in milliseconds
int red_offset, yellow_offset;
const int fade_duration = 2000;   // in ms
unsigned long count = 0;

// duty cycle for pwm
byte fade = 0;
byte fade_green = 0;
byte fade_red = 50;
byte fade_yellow = 100;
bool fade_in, fade_in_green, fade_in_red, fade_in_yellow;
byte fade_max = 150;

// snake parameters
const int snake_state_duration = 100;   //in ms

// mode setup

void do_all(){
  unsigned long cycle_time = millis();
  // divide miliseconds to get a number of cycles
  // take modulus of that number to get the mode number
  // add 1 to shift away the "all" mode
  int mode_to_use = ((cycle_time / mode_duration) % mode_count) + 1;          // count does not reset!
  switch (mode_to_use){
    case blink_all:
      do_blink_all();
      break;
    case blink_color:
      do_blink_color();
      break;
    case snake:
      do_snake();
      break;
    default:
      do_blink_all();
  }  
  }

void do_blink_all(){
  bool ON = (count / blink_duration) % 2;
  if (ON){
   digitalWrite(g1, HIGH);
   digitalWrite(g2, HIGH);
   digitalWrite(r1, HIGH);
   digitalWrite(r2, HIGH);
   digitalWrite(y1, HIGH);
   digitalWrite(y2, HIGH);
  }
  else{
   digitalWrite(g1, LOW);
   digitalWrite(g2, LOW);
   digitalWrite(r1, LOW);
   digitalWrite(r2, LOW);
   digitalWrite(y1, LOW);
   digitalWrite(y2, LOW);
  }
  count++;
  delay(1);
  }
  
void do_fade_all(){
  if(fade_in){
    if(fade < 255){
      fade++;
      }
    else{
      fade_in = !fade_in;
      fade--;
      }
  }
   else{
    if (fade > 0){
      fade--;
      }
    else{
      fade_in = !fade_in;
      fade++;
      delay(20);
      }
    }
    analogWrite(g1, fade);
    analogWrite(g2, fade);
    analogWrite(r1, fade);
    analogWrite(r2, fade);
    analogWrite(y1, fade);
    analogWrite(y2, fade);
    delay(fade_duration/256);
    }


void do_blink_color(){
  bool ON_green = (count / blink_duration) % 2;
  bool ON_red = ((count + red_offset) / blink_duration) % 2;
  bool ON_yellow = ((count + yellow_offset) / blink_duration) % 2;
  // green
  if (ON_green){
   digitalWrite(g1, HIGH);
   digitalWrite(g2, LOW);
  }
  else{
   digitalWrite(g1, LOW);
   digitalWrite(g2, HIGH);
  }
  // red
  if (ON_red){
   digitalWrite(r1, HIGH);
   digitalWrite(r2, LOW);
  }
  else{
   digitalWrite(r1, LOW);
   digitalWrite(r2, HIGH);
  }
  // yellow
  if (ON_yellow){
   digitalWrite(y1, HIGH);
   digitalWrite(y2, LOW);
  }
  else{
   digitalWrite(y1, LOW);
   digitalWrite(y2, HIGH);
  }
  count++;
  delay(1);
  }


void do_fade_color(){
  //green
   if(fade_in_green){
    if(fade_green < fade_max){
      fade_green++;
      }
    else{
      fade_in_green = !fade_in_green;
      fade_green--;
      }
  }
   else{
    if (fade_green > 0){
      fade_green--;
      }
    else{
      fade_in_green = !fade_in_green;
      fade_green++;
      }
    }
    //red    
  if(fade_in_red){
    if(fade_red < fade_max){
      fade_red++;
      }
    else{
      fade_in_red = !fade_in_red;
      fade_red--;
      }
  }
   else{
    if (fade_red > 0){
      fade_red--;
      }
    else{
      fade_in_red = !fade_in_red;
      fade_red++;
      }
    }
    // yellow
    if(fade_in_yellow){
    if(fade_yellow < fade_max){
      fade_yellow++;
      }
    else{
      fade_in_yellow = !fade_in_yellow;
      fade_yellow--;
      }
  }
   else{
    if (fade_yellow > 0){
      fade_yellow--;
      }
    else{
      fade_in_yellow = !fade_in_yellow;
      fade_yellow++;
      }
    }
    analogWrite(g1, fade_green);
    analogWrite(g2, fade_max - fade_green);
    analogWrite(r1, fade_red);
    analogWrite(r2, fade_max - fade_red);
    analogWrite(y1, fade_yellow);
    analogWrite(y2, fade_max - fade_yellow);
    delay(fade_duration/256);
  }

void do_snake(){
    int snake_state = (count / snake_state_duration) % 12;
    switch (snake_state) {
      case 0:
         digitalWrite(r1, LOW);
         digitalWrite(g1, LOW);
         digitalWrite(y1, LOW);
         digitalWrite(r2, LOW);
         digitalWrite(g2, LOW);
         digitalWrite(y2, LOW);
      break;
      case 1:
         digitalWrite(r1, HIGH);
         digitalWrite(g1, LOW);
         digitalWrite(y1, LOW);
         digitalWrite(r2, LOW);
         digitalWrite(g2, LOW);
         digitalWrite(y2, LOW);
      break;
      case 2:
         digitalWrite(r1, HIGH);
         digitalWrite(g1, HIGH);
         digitalWrite(y1, LOW);
         digitalWrite(r2, LOW);
         digitalWrite(g2, LOW);
         digitalWrite(y2, LOW);
      break;
      case 3:
         digitalWrite(r1, HIGH);
         digitalWrite(g1, HIGH);
         digitalWrite(y1, HIGH);
         digitalWrite(r2, LOW);
         digitalWrite(g2, LOW);
         digitalWrite(y2, LOW);
      break;
      case 4:
         digitalWrite(r1, HIGH);
         digitalWrite(g1, HIGH);
         digitalWrite(y1, HIGH);
         digitalWrite(r2, HIGH);
         digitalWrite(g2, LOW);
         digitalWrite(y2, LOW);
      break;
      case 5:
         digitalWrite(r1, HIGH);
         digitalWrite(g1, HIGH);
         digitalWrite(y1, HIGH);
         digitalWrite(r2, HIGH);
         digitalWrite(g2, HIGH);
         digitalWrite(y2, LOW);
      break;
      case 6:
         digitalWrite(r1, HIGH);
         digitalWrite(g1, HIGH);
         digitalWrite(y1, HIGH);
         digitalWrite(r2, HIGH);
         digitalWrite(g2, HIGH);
         digitalWrite(y2, HIGH);
      break;
      case 7:
         digitalWrite(r1, LOW);
         digitalWrite(g1, HIGH);
         digitalWrite(y1, HIGH);
         digitalWrite(r2, HIGH);
         digitalWrite(g2, HIGH);
         digitalWrite(y2, HIGH);
      break;
      case 8:
         digitalWrite(r1, LOW);
         digitalWrite(g1, LOW);
         digitalWrite(y1, HIGH);
         digitalWrite(r2, HIGH);
         digitalWrite(g2, HIGH);
         digitalWrite(y2, HIGH);
      break;
      case 9:
         digitalWrite(r1, LOW);
         digitalWrite(g1, LOW);
         digitalWrite(y1, LOW);
         digitalWrite(r2, HIGH);
         digitalWrite(g2, HIGH);
         digitalWrite(y2, HIGH);
      break;
      case 10:
         digitalWrite(r1, LOW);
         digitalWrite(g1, LOW);
         digitalWrite(y1, LOW);
         digitalWrite(r2, LOW);
         digitalWrite(g2, HIGH);
         digitalWrite(y2, HIGH);
      break;
      case 11:
         digitalWrite(r1, LOW);
         digitalWrite(g1, LOW);
         digitalWrite(y1, LOW);
         digitalWrite(r2, LOW);
         digitalWrite(g2, LOW);
         digitalWrite(y2, HIGH);
      break;
      default:
         digitalWrite(r1, HIGH);
         digitalWrite(g1, HIGH);
         digitalWrite(y1, HIGH);
         digitalWrite(r2, HIGH);
         digitalWrite(g2, HIGH);
         digitalWrite(y2, HIGH);
      }
      delay(1);
      count++;
  }



// light LED based on the selected mode
void lighting(int mode){
  switch (mode){
    case all:
      do_all();
      break;
    case blink_all:
      do_blink_all();
      break;
    case blink_color:
      do_blink_color();
      break;
    case snake:
      do_snake();
      break;
    default:
      do_all();
    }
    delay(1);
  }

bool jiggle = 0;
// change lighting mode on button click
void button_ISR(){
  delay(500);
  jiggle = !jiggle;
  servo.write(centre);

/*  last_updated = curr_time;
  count = 0;
  fade_in = 1;
  fade_in_green = 1;
  fade_in_red = 1;
  fade_in_yellow = 1;
  fade = 0;
  fade_green = 0;
  fade_red = 50;
  fade_yellow = 100;
  */
  }

int angle = left;


void timedServo(){
  if (jiggle){
    servo.write(angle);
    
    if (angle == left){
      angle = right;
      }
    else{
      angle = left;
      }
    } 
} 

// create timed interrupt
TimedAction runServo = TimedAction(400,timedServo);
  
// pin mode declarations and initialization
void setup() {
  // assign servo to a pin
  servo.attach(servoPin);
  servo.write(centre);

  // set pins to output
  pinMode(g1, OUTPUT);
  pinMode(g2, OUTPUT);
  pinMode(r1, OUTPUT);
  pinMode(r2, OUTPUT);
  pinMode(y1, OUTPUT);
  pinMode(y2, OUTPUT);

  pinMode(button, INPUT);
  // initialization
  curr_mode = all;

  // so that leds dont blink at the same time
  red_offset = blink_duration / 3;
  yellow_offset = 2 * red_offset;

  //interrupt setup
  attachInterrupt(0, button_ISR, FALLING);
}

void loop() {
  // put your main code here, to run repeatedly:

 // set light mode to current mode
  lighting(curr_mode);
  runServo.check();
}
