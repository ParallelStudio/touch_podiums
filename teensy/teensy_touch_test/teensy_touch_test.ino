int sensor1 = 15;
int val1 = 0;
int THRESHOLD = 10000;
int sensorState1 = LOW;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);          //  setup serial

}

void loop() {
  // put your main code here, to run repeatedly:
   val1 = touchRead(sensor1);    // read the input pin
   if (val1 > THRESHOLD){
    sensorState1 = LOW;
   }else{
    sensorState1 = HIGH;
   }
    
   Serial.println(val1);             // debug value

}
