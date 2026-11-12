/*
 * Person-Following Robot — Arduino firmware
 * Author: Vince Watson Jr.
 *
 * Current behavior: non-blocking zone controller
 *   - no echo      -> stop
 *   - > 40 cm      -> drive toward target
 *   - 20–40 cm     -> hold (stop)
 *   - < 20 cm      -> back up
 *
 * Wiring:
 *   Left motor  ENA D9,  IN1 D12, IN2 D11
 *   Right motor ENB D6,  IN3 D7,  IN4 D8
 *   HC-SR04     TRIG D4, ECHO D3, VCC 5V, GND GND
 */

// ---------------- Motor ----------------
// One side of the robot. IN1/IN2 set direction (H-bridge), ENA sets speed (PWM).
class Motor {
  private:
    int pinOne;     // direction pin
    int pinTwo;     // direction pin
    int enablePin;  // speed (PWM)

  public:
    Motor(int enPin, int dirPin1, int dirPin2) {
      enablePin = enPin;
      pinOne = dirPin1;
      pinTwo = dirPin2;
    }

    void begin() {
      pinMode(pinOne, OUTPUT);
      pinMode(pinTwo, OUTPUT);
      pinMode(enablePin, OUTPUT);
    }

    void forward(int speed = 155) {
      digitalWrite(pinOne, HIGH);
      digitalWrite(pinTwo, LOW);
      analogWrite(enablePin, speed);  // 0–255 duty cycle
    }

    void backward(int speed = 155) {
      digitalWrite(pinOne, LOW);
      digitalWrite(pinTwo, HIGH);
      analogWrite(enablePin, speed);
    }

    void stop() {  // coast
      digitalWrite(pinOne, LOW);
      digitalWrite(pinTwo, LOW);
      analogWrite(enablePin, 0);
    }
};

// ---------------- Sensor ----------------
// HC-SR04 ultrasonic distance sensor.
class Sensor {
  private:
    int trigPin;  // output
    int echoPin;  // input

  public:
    Sensor(int tPin, int ePin) {
      trigPin = tPin;
      echoPin = ePin;
    }

    void begin() {
      pinMode(trigPin, OUTPUT);
      pinMode(echoPin, INPUT);
    }

    // Single reading in cm. Returns 0 if no echo within 30 ms (~5 m).
    float readCM() {
      digitalWrite(trigPin, LOW);
      delayMicroseconds(2);          // start clean
      digitalWrite(trigPin, HIGH);
      delayMicroseconds(10);         // 10 µs trigger pulse
      digitalWrite(trigPin, LOW);

      long duration = pulseIn(echoPin, HIGH, 30000);  // round-trip time (µs)
      return duration * 0.0343 / 2;                   // d = t * v_sound / 2
    }

    // Averages several readings to reduce noise (blocking — use for testing only).
    float readAverage(int samples = 10) {
      float total = 0;
      for (int i = 0; i < samples; i++) {
        total += readCM();
        delay(60);  // let old echoes die out
      }
      return total / samples;
    }
};

// ---------------- Robot ----------------
// Owns the left and right motors and coordinates both sides.
class Robot {
  private:
    Motor leftMotor;
    Motor rightMotor;

  public:
    Robot(int e1, int p1, int p2, int e2, int p3, int p4)
      : leftMotor(e1, p1, p2), rightMotor(e2, p3, p4) {}

    void begin() {
      leftMotor.begin();
      rightMotor.begin();
    }

    void forward(int speed = 155) {
      leftMotor.forward(speed);
      rightMotor.forward(speed);
    }

    void backward(int speed = 155) {
      leftMotor.backward(speed);
      rightMotor.backward(speed);
    }

    void turnRight() {
      leftMotor.forward();
      rightMotor.stop();
    }

    void turnLeft() {
      leftMotor.stop();
      rightMotor.forward();
    }

    void stop() {
      leftMotor.stop();
      rightMotor.stop();
    }
};

// ---------------- Objects ----------------
Robot myRobot(9, 12, 11, 6, 7, 8);
Sensor frontSensor(4, 3);  // (trig, echo)

// ---------------- Timing ----------------
unsigned long lastRead = 0;   // last control update
unsigned long lastPrint = 0;  // last serial print
float d = 0;                  // latest distance (cm)

const unsigned long CONTROL_INTERVAL = 60;  // ms
const unsigned long PRINT_INTERVAL = 300;   // ms

// ---------------- Test helpers ----------------
// Call one of these from setup() when you want to run a test.

void motorTest() {
  myRobot.forward();
  delay(2000);
  myRobot.backward();
  delay(2000);
  myRobot.stop();
}

// Finds PWM_min: raises speed by 10 each second and prints it.
// Measured: left 100, right 80.
void pwmRampTest() {
  for (int speed = 0; speed < 255; speed += 10) {
    myRobot.forward(speed);
    Serial.println(speed);
    delay(1000);
  }
  myRobot.stop();
}

// ---------------- Main ----------------
void setup() {
  myRobot.begin();      // motor pins -> OUTPUT
  frontSensor.begin();  // sensor pins
  Serial.begin(9600);
  myRobot.stop();       // safety: motors start off
}

void loop() {
  // Control loop: read sensor and decide motors every 60 ms
  if (millis() - lastRead >= CONTROL_INTERVAL) {
    lastRead = millis();
    d = frontSensor.readCM();

    if (d == 0) {
      myRobot.stop();         // nothing detected
    } else if (d > 40) {
      myRobot.forward(150);   // too far
    } else if (d < 20) {
      myRobot.backward(150);  // too close
    } else {
      myRobot.stop();         // hold zone
    }
  }

  // Logging: print distance every 300 ms
  if (millis() - lastPrint >= PRINT_INTERVAL) {
    lastPrint = millis();
    Serial.println(d);
  }
}
