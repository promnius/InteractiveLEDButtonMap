
unsigned long a = 0;
unsigned long b = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  a = 4294967000;
  b = a+1000;

}


void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("a: "); Serial.println(a);
  Serial.print("b: "); Serial.println(b);
  Serial.print("b-a: "); Serial.println(b-a);
  delay(200);

}
