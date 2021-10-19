#include <AD5933.h>
#include <Servo.h>
#include <Wire.h>

#define START_FREQ  (18000)
#define FREQ_INCR   (100)
#define NUM_INCR    (40)
#define REF_RESIST  (30000)

#define DRYING 0
#define DETERMINING 1
#define COMPLETE 2

Servo servo;

double gain[NUM_INCR+1];
int phase[NUM_INCR+1];
int count_inf=0;
int start = millis();
int fan = 6;
int VCO = 7;
int led = 8;
unsigned int determine_time;
byte det_state = DRYING;
int motor = 5;

void setup(void)
{
  // Begin I2C
  Wire.begin();
  
  servo.attach(motor);
  servo.write(90);

  pinMode(fan, OUTPUT);
  pinMode(VCO, OUTPUT);
  pinMode(led, OUTPUT);
  
  // Begin serial at 9600 baud for output
  Serial.begin(9600);
  //Serial.println("AD5933 Test Started!");


  // Perform initial configuration. Fail if any one of these fail.
  if (!(AD5933::reset() &&
        AD5933::setInternalClock(true) &&
        AD5933::setStartFrequency(START_FREQ) &&
        AD5933::setIncrementFrequency(FREQ_INCR) &&
        AD5933::setNumberIncrements(NUM_INCR) &&
        AD5933::setPGAGain(PGA_GAIN_X1)))
        {
            Serial.println("FAILED in initialization!");
            while (true) ;
        }

  // Perform calibration sweep
 
  getGain(gain);

  digitalWrite(fan, HIGH);
  digitalWrite(VCO, HIGH);
  digitalWrite(led, HIGH);
//  angle = 180;
}

void loop(void)
{
  // Easy to use method for frequency sweep
  frequencySweepEasy();
  
  servo.write(65);
  // Delay
  delay(1000);
  servo.write(115);
  measureTemperature();
  Serial.println("],");
  
  delay(1000);
  servo.write(170);
  // Complex but more robust method for frequency sweep
  frequencySweepRaw();
//  digitalWrite(VCO, HIGH);
  servo.write(115);
  // Delay
  delay(1000);
  servo.write(115);
  servo.write(65);
  measureTemperature();
  Serial.println("],");
  delay(1000);
  servo.write(10);
}

// Easy way to do a frequency sweep. Does an entire frequency sweep at once and
// stores the data into arrays for processing afterwards. This is easy-to-use,
// but doesn't allow you to process data in real time.
void frequencySweepEasy() {
    // Create arrays to hold the data
    int real[NUM_INCR+1], imag[NUM_INCR+1];
    unsigned long t;
    unsigned long sec;
    count_inf = 0;
    // Perform the frequency sweep
    if (AD5933::frequencySweep(real, imag, NUM_INCR+1)) {
      // Print the frequency data
      double cfreq = START_FREQ/1000.0;
      t=millis();
      sec = t / 1000;
      Serial.print("'"); 
      Serial.print(sec/3600);
      Serial.print(":");
      Serial.print((sec%3600)/60);
      Serial.print(":");
      Serial.print((sec%3600)%60);
      Serial.print("': [");
      for (int i = 0; i < NUM_INCR+1; i++, cfreq += FREQ_INCR/1000.0) {

        // Compute impedance
        double magnitude = sqrt(pow(real[i], 2) + pow(imag[i], 2));
        double impedance = 1/(magnitude*gain[i]);
       
        if(isinf(impedance))
          count_inf++;
        Serial.print(impedance);
        Serial.print(", ");
      }
      
      Serial.print(count_inf);
      Serial.print(", ");
      
     /* if(complete_dry(average, sec, count_inf))
      {
        digitalWrite(VCO, LOW);
        digitalWrite(fan, LOW);
        digitalWrite(led, LOW);
        Serial.println("Dry protocol complete!");
        servo.write(90);
        while(1);
      }
*/
    } else {
      Serial.println("Frequency sweep failed...");
    }
}

// Removes the frequencySweep abstraction from above. This saves memory and
// allows for data to be processed in real time. However, it's more complex.
void frequencySweepRaw() {
    // Create variables to hold the impedance data and track frequency
    int real, imag, i = 0;
    unsigned long t;
    unsigned long sec;
    double cfreq = START_FREQ/1000.0;
    count_inf = 0;
    // Initialize the frequency sweep
    if (!(AD5933::setPowerMode(POWER_STANDBY) &&          // place in standby
          AD5933::setControlMode(CTRL_INIT_START_FREQ) && // init start freq
          AD5933::setControlMode(CTRL_START_FREQ_SWEEP))) // begin frequency sweep
         {
             Serial.println("Could not initialize frequency sweep...");
         }
    t=millis();
      sec = t / 1000;
       Serial.print("'"); 
      Serial.print(sec/3600);
      Serial.print(":");
      Serial.print((sec%3600)/60);
      Serial.print(":");
      Serial.print((sec%3600)%60);
       Serial.print("': [");
    // Perform the actual sweep
    while ((AD5933::readStatusRegister() & STATUS_SWEEP_DONE) != STATUS_SWEEP_DONE) {
        // Get the frequency data for this frequency point
        if (!AD5933::getComplexData(&real, &imag)) {
            Serial.println("Could not get raw frequency data...");
        }

        // Print out the frequency data

        // Compute impedance
        double magnitude = sqrt(pow(real, 2) + pow(imag, 2));
        double impedance = 1/(magnitude*gain[i]);
  //      Serial.print("  |Z|=");
        Serial.print(impedance);
        Serial.print(", ");
        if(isinf(impedance))
          count_inf++;
        // Increment the frequency
        i++;
        cfreq += FREQ_INCR/1000.0;
        AD5933::setControlMode(CTRL_INCREMENT_FREQ);
    }
    
      
      Serial.print(count_inf);
      Serial.print(", ");
      
     /* if(complete_dry(average, sec, count_inf))
      {
        digitalWrite(VCO, LOW);
        digitalWrite(fan, LOW);
        digitalWrite(led, LOW);
        Serial.println("Dry protocol complete!");
        servo.write(90); 
        while(1);
      }
*/
    // Set AD5933 power mode to standby when finished
    if (!AD5933::setPowerMode(POWER_STANDBY))
        Serial.println("Could not set to standby...");
}

void measureTemperature()
{
  double temperature;
  //AD5933::enableTemperature(TEMP_MEASURE);
  temperature = AD5933::getTemperature();
  Serial.print(temperature, 4);
  //Serial.println(" ºC");
}
/*
bool complete_dry(double average, unsigned long sec, int count_inf){
  unsigned long three_min;

  if(average <= 75000){           // 값이 75,000 이하고 inf값이 2개 미만이면 건조상태
    if(count_inf < 2)
      det_state = DRYING;
    else if(det_state ==DRYING){
      determine_time = sec;
      det_state = DETERMINING;
    }
  }
  else if(det_state == DRYING){   // 값이 75,000 초과이며 건조상태이면 현재 시간을 저장하고 결정상태로 바꿈
    determine_time = sec;
    det_state = DETERMINING;
  }
  three_min = sec - determine_time;   // 저장된 시간을 작동시간에서 뺀다
  
  if(det_state == DETERMINING && three_min > 180)   // 결정상태이고, 저장된 시간이 작동시간을 뺀 값이 3분이 넘으면 완료상태로 바꿈
    det_state = COMPLETE;
    
  if(det_state == COMPLETE) // 완료상태이면 건조 완료
    return true;
  else                      // 완료 이외 모든 상태는 건조 미완료
    return false;
}
*/
void getGain(double * gain)
{
  gain[0] = 0.00000297540378570;
gain[1] = 0.00000310170030593;
gain[2] = 0.00000326579475402;
gain[3] = 0.00000346796798706;
gain[4] = 0.00000386969327926;
gain[5] = 0.00000420713281631;
gain[6] = 0.00000391812086105;
gain[7] = 0.00000360262250900;
gain[8] = 0.00000319245910644;
gain[9] = 0.00000295132617950;
gain[10] = 0.00000261181616783;
gain[11] = 0.00000237774324417;
gain[12] = 0.00000230518198013;
gain[13] = 0.00000229856252670;
gain[14] = 0.00000249795579910;
gain[15] = 0.00000280144481658;
gain[16] = 0.00000321802473068;
gain[17] = 0.00000406753444671;
gain[18] = 0.00000489499044418;
gain[19] = 0.00000624361610412;
gain[20] = 0.00000648247003555;
gain[21] = 0.00000644208812713;
gain[22] = 0.00000582044219970;
gain[23] = 0.00000506920719146;
gain[24] = 0.00000451203536987;
gain[25] = 0.00000468364715576;
gain[26] = 0.00000492221927642;
gain[27] = 0.00000502816486358;
gain[28] = 0.00000517699432373;
gain[29] = 0.00000499003458023;
gain[30] = 0.00000508032894134;
gain[31] = 0.00000533634138107;
gain[32] = 0.00000494208717346;
gain[33] = 0.00000553363037109;
gain[34] = 0.00000544800806045;
gain[35] = 0.00000550765323638;
gain[36] = 0.00000468655109405;
gain[37] = 0.00000443616342544;
gain[38] = 0.00000408422088623;
gain[39] = 0.00000310453629493;
gain[40] = 0.00000208471322059;
}
