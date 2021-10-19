int led = 13;
int val;
int cnt = 0;
unsigned long t, sec;

void setup() {
 
  Serial.begin(9600);
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
}
 
void loop() {
  cnt++;
  if(Serial.available())
    val = Serial.read();

  if('1' == val)
    digitalWrite(led, HIGH);
  if('0' == val)
    digitalWrite(led, LOW);
  t=millis();
  sec = t / 1000;
  Serial.print("'"); 
  Serial.print(sec/3600);
  Serial.print(":");
  Serial.print((sec%3600)/60);
  Serial.print(":");
  Serial.print((sec%3600)%60);
  Serial.print("': [");

  for (int i = 0; i < 7; i++)
  {
    double imp = cnt*(1e+2) + i;
    Serial.print(imp);
    Serial.print(", ");
  }
  Serial.print(cnt);
  Serial.println("]");
  /*Serial.println("'01:09:19': [0001.10, 3922.11, 5322.49, 3244.96, 4134.93, 4122.32, 4127.32, 0],");
  delay(500);
  Serial.println("'01:09:23': [1001.10, 3922.11, 5322.49, 3244.96, 4134.93, 6432.55, 4127.32, 0],");
  delay(500);
  Serial.println("'01:09:30': [2001.05, 5122.99, 2352.25, 4459.23, 3551.74, 5632.14, 3861.65, 0],");
  delay(500);
  Serial.println("'01:09:36': [3001.11, 3002.22, 3003.33, 3004.44, 3005.55, 6614.22, 3006.66, 0],");
  delay(500);
  Serial.println("'01:09:44': [4001.11, 4002.22, 2352.25, 4459.23, 3551.74, 5614.22, 3861.65, 0],");
  delay(500);
  Serial.println("'01:10:10': [5001.11, 5122.99, 2352.25, 4459.23, 3551.74, 7614.22, 3861.65, 0],");
  delay(500);
  Serial.println("'01:10:21': [6001.87, 8244.14, 8451.53, inf, 7853.43, 6614.22, 7997.00, 1],");
  delay(500);
  Serial.println("'01:10:25': [7001.78, inf, 7682.11, 6932.89, inf, 69614.22, 7686.26, 2],");
  delay(500);
  Serial.println("'01:10:30': [8001.87, 8244.14, 8451.53, inf, 7853.43, 6614.22, 7997.00, 1],");
  delay(500);
  Serial.println("'01:10:36': [9001.87, 9244.14, 9451.53, 8614.22, inf, 7853.43, 7997.00, 1],");
  delay(500);
  Serial.println("'01:10:43': [1101.78, 1153.43, inf, 3612.22, 6932.89, inf, 7686.26, 2],");
  delay(500);
  Serial.println("'01:10:49': [1201.53, 9001.44, 7682.11, 9614.22, 6932.89, 9744.11, 7686.26, 0],");
  delay(500);
  Serial.println("'01:10:58': [1301.53, 9001.44, 7682.11, 6932.89, 9744.11, 8614.22, 8831.55, 0],");
  delay(500);  
  */
}
