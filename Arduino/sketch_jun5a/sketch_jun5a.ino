int led = 26;
int ldr= 35;
void setup(){
  Serial.begin(115200);
  pinMode(ldr, INPUT);
  pinMode(led, OUTPUT);
}
void loop(){
  Serial.println(analogRead(ldr));
  int brightness = map(analogRead(ldr), 0 , 2800 , 255 , 0);
  analogWrite(ldr, brightness);
  delay(500);
}