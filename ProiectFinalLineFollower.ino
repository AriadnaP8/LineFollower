// Pini motor 1
#define motor1_pin1 5
#define motor1_pin2 6

// Pini motor 2
#define motor2_pin1 3
#define motor2_pin2 11

// Pini ultrasonic sensor
#define echoPin 2
#define trigPin 4

//XLine Sensor
#define s0 7 // Sensor Selection pins
#define s1 8
#define s2 12
#define s3 13
#define line_sensor_pin A0 // Analog input pin

#define neopixel_pin 10
#define num_pixels 16
#define buzzer_pin 9
#define potentiometer_pin A2
#define interval_ON 500
#define interval_OFF 700

int value;
unsigned int XlineSensorValues[16];

// RGB lights
#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel strip = Adafruit_NeoPixel(num_pixels, neopixel_pin, NEO_GRB + NEO_KHZ800);

// buzzer
int melody[] = {262, 196, 196, 220, 196, 0, 247, 262}; //frecvente de note muzicale
int noteDurations[] = {4, 8, 8, 4, 4, 4, 4, 4 };
int motor_speed = 128;

long unsigned previousMillis = 0; //pentru masurarea unor intervale
int interval_line_sensor = 10000; //definirea intervalului pentru masurarea line sensor
int interval_distance_sensor = 20000; //definirea intervalului pentru masurarea distantei

// Functia setup() se executa o singura data, la inceputul programului
void setup() {
  Serial.begin(9600); //functie pentru pornirea interfetei seriale, pentru debugging
  // configurarea pinilor motor ca iesire, initial valoare 0
  pinMode(motor1_pin1, OUTPUT);
  pinMode(motor1_pin2, OUTPUT);
  pinMode(motor2_pin1, OUTPUT);
  pinMode(motor2_pin2, OUTPUT);
  
  digitalWrite(motor1_pin1, 0);
  digitalWrite(motor1_pin2, 0);
  digitalWrite(motor2_pin1, 0);
  digitalWrite(motor2_pin2, 0);
  
  pinMode(potentiometer_pin, INPUT);
  //pinMode(buzzer_pin, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  pinMode(s0, OUTPUT); // 4 pini de selectie pentru line sensor sunt output
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);
  
  pinMode(line_sensor_pin, INPUT); //pinul analogic de la line sensor este input
  pinMode(neopixel_pin, OUTPUT);
  
  strip.begin(); //initializarea benzii de LED-uri RGB
  strip.setBrightness(50); //setarea luminozitatii pentru banda de LED-uri (maxim 255)
  strip.show(); //afisarea benzii de LED-uri
  
  functie_pornire();
  functie_melodie();
  previousMillis=0;
}

// Functia loop() se executa repetitiv
void loop() {
  int pot_value = analogRead(potentiometer_pin); //citirea valorii de la potentiometru
  motor_speed = 200;
  check_robot_position_and_redress(); //Executes first

// Funcție pentru controlul unui motor. Intrare: cei doi pini ai unui motor, direcția (0/1) și viteza de rotatie a motorului (semnal PWM)
void StartMotor (int m1, int m2, int forward, int speed)
{
  if (speed == 0) {
    digitalWrite(m1, 0); // oprire
    digitalWrite(m2, 0);
  } else {
    if (forward) {
      digitalWrite(m2, 0);
      analogWrite(m1, speed); // folosire PWM
    } else {
      digitalWrite(m1, 0);
      analogWrite(m2, speed);
    }
  }
}

// Funcție de oprire a ambelor motoare, urmată de delay
void delayStopped(int ms)
{
  StartMotor (motor1_pin1, motor1_pin2, 0, 0);
  StartMotor (motor2_pin1, motor2_pin2, 0, 0);
  delay(ms);
}

// Functie pentru verificarea distantei pana la obstacol si actionarea motoarelor si a LED-urilor in functie de obstacol
void check_distance() {
  int distance = measure_distance();
  //Serial.println(distance);
  if (distance < 30) {
    for (int i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(255, 0, 0)); //turn LEDs red
    }
    strip.show();
    delayStopped(50); //stop motors
    while (distance < 30) {
      // tone(buzzer_pin, melody[0], 200);
      for (int i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, strip.Color(255, 0, 0)); //turn LEDs red
      }
      strip.show();
      delay(200);
      // noTone(buzzer_pin);
      clear_leds();
      delay(200);
      distance = measure_distance();
    }
  }
  else {
    StartMotor (motor1_pin1, motor1_pin2, 0, 0);
    StartMotor (motor2_pin1, motor2_pin2, 0, 0);
    for (int i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(0, 255, 100)); //turn LEDs green
    }
    strip.show();
  }
}

// Functie pentru masurarea distantei pana la cel mai apropiat obstacol cu ajutorul senzorului ultrasonic. Returneaza distanta in cm
int measure_distance() {
  long duration; // variable for the duration of sound wave travel
  int distance; // variable for the distance measurement
  
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
  return distance;
}

// Functie care reda o melodie pe buzzer
void functie_melodie() {
  for (int thisNote = 0; thisNote < 8; thisNote++) {
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(buzzer_pin, melody[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(buzzer_pin);
  }
}

// Functie pentru afisarea unei animatii pe LED-uri la pornire
void functie_pornire() {
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(255, 0, 255));
    strip.show();
    delay(50);
  }
  delay(200);
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
    strip.show();
    delay(50);
  }
  delay(200);
}

// Functie pentru miscarea robotului in fata
void move_robot_forward() {
  show_LEDs_forward();
  StartMotor (motor1_pin1, motor1_pin2, 1, motor_speed);
  StartMotor (motor2_pin1, motor2_pin2, 0, motor_speed);
}

void show_LEDs_forward() {
  clear_leds();
  for (int i = 0; i <= strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(0, 255, 0));
  }
  strip.show();
}

// Functie pentru miscarea robotului in spate
void move_robot_backward() {
  show_LEDs_backward();
  StartMotor (motor1_pin1, motor1_pin2, 0, motor_speed);
  StartMotor (motor2_pin1, motor2_pin2, 1, motor_speed);
}

void show_LEDs_backward() {
  clear_leds();
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(255, 255, 0));
  }
  strip.show();
}

// Functie pentru miscarea robotului in stanga
void move_robot_left() {
  show_LEDs_left();
  StartMotor (motor1_pin1, motor1_pin2, 0, motor_speed);
  StartMotor (motor2_pin1, motor2_pin2, 0, motor_speed);
}

void show_LEDs_left() {
  clear_leds();
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(0, 255, 255));
  }
  strip.show();
}

// Functie pentru miscarea robotului in dreapta
void move_robot_right() {
  show_LEDs_right();
  StartMotor (motor1_pin1, motor1_pin2, 1, motor_speed);
  StartMotor (motor2_pin1, motor2_pin2, 1, motor_speed);
}

void show_LEDs_right() {
  clear_leds();
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(255, 0, 255));
  }
  strip.show();
}

// Functie pentru oprirea LED-urilor
void clear_leds() {
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
  }
  strip.show();
}

void read_line_sensor() {
  for (int j = 0; j < 16; j++) {
    XlineSensorValues[j] = 0;
  }
  for (int j = 0; j < 1; j++) { // media aritmetica a 10 citiri
    for (int i = 0; i <= 15; i++) { // Total 16 loop inside for statement.
      switch (i) {
        case 15:
          digitalWrite(s0, LOW);
          digitalWrite(s1, LOW);
          digitalWrite(s2, LOW);
          digitalWrite(s3, LOW);
          delay(1);
          break;
        case 14:
          digitalWrite(s0, HIGH);
          digitalWrite(s1, LOW);
          digitalWrite(s2, LOW);
          digitalWrite(s3, LOW);
          delay(1);
          break;
        case 13:
          digitalWrite(s0, LOW);
          digitalWrite(s1, HIGH);
          digitalWrite(s2, LOW);
          digitalWrite(s3, LOW);
          delay(1);
          break;
        case 12:
          digitalWrite(s0, HIGH);
          digitalWrite(s1, HIGH);
          digitalWrite(s2, LOW);
          digitalWrite(s3, LOW);
          delay(1);
          break;
        case 11:
          digitalWrite(s0, LOW);
          digitalWrite(s1, LOW);
          digitalWrite(s2, HIGH);
          digitalWrite(s3, LOW);
          delay(1);
          break;
        case 10:
          digitalWrite(s0, HIGH);
          digitalWrite(s1, LOW);
          digitalWrite(s2, HIGH);
          digitalWrite(s3, LOW);
          delay(1);
          break;
        case 9:
          digitalWrite(s0, LOW);
          digitalWrite(s1, HIGH);
          digitalWrite(s2, HIGH);
          digitalWrite(s3, LOW);
          break;
        case 8:
          digitalWrite(s0, HIGH);
          digitalWrite(s1, HIGH);
          digitalWrite(s2, HIGH);
          digitalWrite(s3, LOW);
          break;
        case 7:
          digitalWrite(s0, LOW);
          digitalWrite(s1, LOW);
          digitalWrite(s2, LOW);
          digitalWrite(s3, HIGH);
          break;
        case 6:
          digitalWrite(s0, HIGH);
          digitalWrite(s1, LOW);
          digitalWrite(s2, LOW);
          digitalWrite(s3, HIGH);
          break;
        case 5:
          digitalWrite(s0, LOW);
          digitalWrite(s1, HIGH);
          digitalWrite(s2, LOW);
          digitalWrite(s3, HIGH);
          break;
        case 4:
          digitalWrite(s0, HIGH);
          digitalWrite(s1, HIGH);
          digitalWrite(s2, LOW);
          digitalWrite(s3, HIGH);
          break;
        case 3:
          digitalWrite(s0, LOW);
          digitalWrite(s1, LOW);
          digitalWrite(s2, HIGH);
          digitalWrite(s3, HIGH);
          break;
        case 2:
          digitalWrite(s0, HIGH);
          digitalWrite(s1, LOW);
          digitalWrite(s2, HIGH);
          digitalWrite(s3, HIGH);
          break;
        case 1:
          digitalWrite(s0, LOW);
          digitalWrite(s1, HIGH);
          digitalWrite(s2, HIGH);
          digitalWrite(s3, HIGH);
          break;
        case 0:
          digitalWrite(s0, HIGH);
          digitalWrite(s1, HIGH);
          digitalWrite(s2, HIGH);
          digitalWrite(s3, HIGH);
          break;
      }
      value = analogRead(line_sensor_pin);
      XlineSensorValues[i] += value;
    }
  }
  
  for (int j = 0; j < 16; j++) {
    XlineSensorValues[j] /= 1;
    Serial.print(XlineSensorValues[j]);
    Serial.print(' ');
  }
  Serial.println();
  for (int k = 0; k < 16; k++) {
    if (XlineSensorValues[k] > 500) {
      strip.setPixelColor(k, strip.Color(100, 0, 0)); //red, vede linia
    }
    else {
      strip.setPixelColor(k, strip.Color(0, 100, 0)); //green, nu vede linia
    }
  }
  strip.show();
  //delay(150);
}

void check_robot_position_and_redress() {
  read_line_sensor();
  
  //Daca robotul este centrat (adica citeste linia cu senzorii din mijloc - valori sub 500), atunci motoarele mers in fata cu aceeasi viteza
  if (XlineSensorValues[7] > 750 || XlineSensorValues[8] > 750) {
    move_robot_forward();
  }
  //Daca robotul o ia spre dreapta (adica citeste linia cu senzorii din stanga), atunci motoarele au nevoie de o viteza diferita, ca sa vireze spre stanga (sa se redreseze)
  else if (XlineSensorValues[0] > 750 || XlineSensorValues[1] > 750 || XlineSensorValues[2] > 750 || XlineSensorValues[3] || XlineSensorValues[4] > 750 || XlineSensorValues[5] > 750 || XlineSensorValues[6] > 750) {
    move_robot_left();
  }
  //Daca robotul o ia spre stanga (adica citeste linia cu senzorii din dreapta), atunci motoarele au nevoie de o viteza diferita, ca sa vireze spre dreapta (sa se redreseze)
  else if (XlineSensorValues[9] > 750 || XlineSensorValues[10] > 750 || XlineSensorValues[11] > 750 || XlineSensorValues[12] > 750 || XlineSensorValues[13] > 750 || XlineSensorValues[14] > 750 || XlineSensorValues[15] > 750) {
    move_robot_right();
  }
  else{
    move_robot_forward();
  }
}
