// defines pins numbers
const int trigPin = 3;
const int echoPin = 2;
const int buzzer = 12;  //buzzer to arduino pin 9

// defines variables
long duration;
int distance;
void setup() {
  pinMode(trigPin, OUTPUT);  // Sets the trigPin as an Output
  pinMode(echoPin, INPUT);   // Sets the echoPin as an Input
  Serial.begin(9600);        // Starts the serial communication
  pinMode(buzzer, OUTPUT);   // Set buzzer - pin 12 as an output
}

void loop() {
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2;
  // Prints the distance on the Serial Monitor
  Serial.print("Distance: \t");
  Serial.println(distance);

  if (distance < 5) {
    digitalWrite(buzzer, HIGH);
    const int buzzer = 12;  //buzzer to arduino pin 12

    tone(buzzer, 2000);  // Send 1KHz sound signal...
    delay(200);
    noTone(buzzer);  // Stop sound...
    delay(200);      // ...for 1 sec
  }

  if (distance > 5) {
    digitalWrite(buzzer, LOW);
    noTone(buzzer);  // Stop sound...
    delay(1000);     // ...for 1sec
  }
}
