
class Motor{
  private:
  int pinOne; //directional pin
  int pinTwo; //directional pin
  int enablePin; //speed

  public: 
  //constructor: to be a "motor" you must have these 3 things
  Motor(int enPin, int dirPin1, int dirPin2){
    enablePin = enPin;
    pinOne = dirPin1;
    pinTwo = dirPin2;


  };
  void begin(){
    pinMode(pinOne, OUTPUT);
    pinMode(pinTwo, OUTPUT);
    pinMode(enablePin, OUTPUT);
  }
//movement methods
  void forward(int speed = 155){
    digitalWrite(pinOne, HIGH);
    digitalWrite(pinTwo, LOW);
    analogWrite(enablePin, speed);
  }

  void backward(int speed = 155){
    digitalWrite(pinOne, LOW);
    digitalWrite(pinTwo, HIGH);
    analogWrite(enablePin, speed);

  }

void stop(){
    digitalWrite(pinOne, LOW);
    digitalWrite(pinTwo, LOW);
    analogWrite(enablePin, 0);
}


};

 class Sensor{
    private:
    int trigPin; //output
    int echoPin; //input

    public:
//sensor constructor,  sensor must have these two i/o pins
    Sensor(int tPin, int ePin){
     trigPin = tPin;
     echoPin = ePin; //member = parameter

    }
//begin sets up the pins
    void begin(){
      pinMode(trigPin, OUTPUT);
      pinMode(echoPin, INPUT);
    }

    float readCM(){
      digitalWrite(trigPin, LOW); 
      delayMicroseconds(2); //clears it so the "shout" starts clean
      digitalWrite(trigPin, HIGH); //the "shout" aka pulse
      delayMicroseconds(10); 
      digitalWrite(trigPin, LOW); //done shouting
      
      //(echoPin = listening, HIGH = sound round trip, 30000 - max wait, nothing back it gives up )
    long duration = pulseIn(echoPin, HIGH, 30000);

  
      float distance = (duration * 0.0343/2);
      return distance;

    }
    

    //fine tunes sensor readings by usign averages
      float readAverage(int samples = 10){
        float total = 0;
        for(int i = 0; i < samples; i++){
          total += readCM();
          delay(60);
        }
        return total / samples;

      }
      



     









  }; 
  

class Robot{
  private:
  Motor leftMotor;
  Motor rightMotor;

  public:
  //passes all pins through constructor, then intialzier list splits them up into dedicated motors
  Robot(int e1, int p1, int p2, int e2, int p3, int p4) : leftMotor(e1,p1,p2) , rightMotor(e2, p3, p4)
  {}

 
  void begin(){
    leftMotor.begin();
    rightMotor.begin();


  }
  void turnRight(){
    leftMotor.forward();
    rightMotor.stop();
  }

  void turnLeft(){
    leftMotor.stop();
    rightMotor.forward();
  }

  void forward(int speed = 155){
    leftMotor.forward(speed);
    rightMotor.forward(speed);
  }

  void backward(int speed = 155){
    leftMotor.backward(speed);
    rightMotor.backward(speed);
  }

  void stop(){
    leftMotor.stop();
    rightMotor.stop();
  }


};
  
//giving pin numbers actual values 
Robot myRobot(9, 12, 11, 6, 7, 8);
Sensor frontSensor(4, 3); //(trigPin, echoPIn actual values)

void motorTest(){
  myRobot.forward();
  delay(2000);
  myRobot.backward();
  delay(2000);
  myRobot.stop();
}

//happens once
void setup() {

myRobot.begin(); //set motor pins as OUTPUT
frontSensor.begin(); //sets sensor pins
Serial.begin(9600); //serial monitor connected
myRobot.stop(); //turn off; like parking car


 
/*
RAMP PWMmin TEST
Serial.begin(9600);

for(int speed=0; speed<255; speed+=10){
  myRobot.forward(speed);
  Serial.println(speed);
  delay(1000);
}

*/
  




}
unsigned long lastRead =0;
unsigned long lastPrint =0;
float d =0;
 

//happens over & over
void loop() {
 //should get 4 readings per second
if(millis()- lastPrint >=300){
  lastPrint = millis();
  Serial.println(d);
}

if(millis() - lastRead >= 60){ //has 60 ms passed?
  lastRead = millis(); //reset timer
  d = frontSensor.readCM();
  

    if(d==0){
      myRobot.stop();
    }
  else if(d > 40){
    myRobot.forward(150);
  }
    else if(d < 20){
      myRobot.backward(150);//backup getting too close
    }
    else{
      myRobot.stop();
    }


  }
}

 

  


