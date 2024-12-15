#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Initialize LCD object with 32 as I2C address, 16 columns, and 2 rows
LiquidCrystal_I2C lcd(32, 16, 2);

// Define pin numbers for components
#define PIR_PIN 2 
#define TRIGGER_PIN1 3 
#define ECHO_PIN1 4 
#define TRIGGER_PIN2 5 
#define ECHO_PIN2 6 
#define LED_PIN1 10 
#define SERVO_PIN 9 

// Initialize Servo motor object
Servo gateServo; 

// Variables to track parking slot occupancy
bool isSlot1Empty = false;
bool isSlot2Empty = false;

void setup() {
  // Set pin modes for components
  pinMode(PIR_PIN, INPUT);
  pinMode(TRIGGER_PIN1, OUTPUT);
  pinMode(ECHO_PIN1, INPUT);
  pinMode(TRIGGER_PIN2, OUTPUT);
  pinMode(ECHO_PIN2, INPUT);
  pinMode(LED_PIN1, OUTPUT);
  
  // Initialize and configure LCD display
  lcd.begin(16, 2);
  lcd.init();
  lcd.backlight();
  
  // Attach servo motor to designated pin
  gateServo.attach(SERVO_PIN); 
}

void loop() {
  // Check if vehicle is detected by PIR sensor
  if (isVehicleDetected()) {
    // Update slot occupancy status
    isSlot1Empty = !isSlotOccupied(1);
    isSlot2Empty = !isSlotOccupied(2);
    
    // If either slot is empty, open the gate
    if (isSlot1Empty || isSlot2Empty) {
      // Open gate to 180 degrees
      gateServo.write(180);
      // Turn on LED indicating availability
      digitalWrite(LED_PIN1, HIGH);
      // Display status on LCD
      lcd.clear();
      lcd.setCursor(0, 0);
      // Display appropriate message based on slot occupancy
      if (isSlot1Empty && isSlot2Empty) {
        lcd.print("Both slots empty.");
      } else if (isSlot1Empty) {
        lcd.print("Slot 1 empty.");
      } else {
        lcd.print("Slot 2 empty.");
      }
      // Delay to display message
      delay(5000); 
      // Turn off LED and close gate after delay
      digitalWrite(LED_PIN1, LOW); 
      gateServo.write(0);
    } else {
      // If both slots are occupied, display parking full message
      closeGate();
      digitalWrite(LED_PIN1, LOW);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Parking is occupied.");
      delay(5000); 
    }
  }
}

// Function to check if vehicle is detected by PIR sensor
bool isVehicleDetected() {
  return digitalRead(PIR_PIN) == HIGH;
}

// Function to check if parking slot is occupied
bool isSlotOccupied(int slot) {
  // Check distance using ultrasonic sensor to determine occupancy
  if (slot == 1) {
    return getDistance(TRIGGER_PIN1, ECHO_PIN1) < 50; 
  } else if (slot == 2) {
    return getDistance(TRIGGER_PIN2, ECHO_PIN2) < 50; 
  }
}

// Function to measure distance using ultrasonic sensor
float getDistance(int triggerPin, int echoPin) {
  // Send ultrasonic pulse
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  // Measure duration of pulse
  float duration = pulseIn(echoPin, HIGH);
  // Convert duration to distance in centimeters
  float distance = duration * 0.034 / 2;
  return distance;
}

// Function to close the gate
void closeGate() {
  gateServo.write(0);
}
