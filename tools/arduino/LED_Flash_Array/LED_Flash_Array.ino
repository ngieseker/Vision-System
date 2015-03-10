/* LED Flash Array
 */
long time = 0;
long prev_time = 0;
long count = 0;

void setup() {                
  pinMode(12, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(9, OUTPUT);
  time = micros();
  prev_time = time;
}

// the loop routine runs over and over again forever:
void loop() {
  time = micros();
  if (time - prev_time > 34500) {
    count++;
	prev_time = time;
  }
  digitalWrite(12, count & 0x1);
  digitalWrite(11, count & 0x2);
  digitalWrite(10, count & 0x4);
  digitalWrite(9, count & 0x8);
}
