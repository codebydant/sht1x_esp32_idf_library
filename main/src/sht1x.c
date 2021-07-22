#include "include/sht1x.h"

int LSBFIRST = 0;
int MSBFIRST = 1;

float readTempC(int data, int clk) {
  int obtVal;
  float temp;

  // Temperature constants for sht11 at 14bit
  const float D1 = -40.0; // at 3.3v
  const float D2 = 0.01;  // for 14 bits

  // get the temperature reading from sensor (raw data)
  obtVal = readTemperatureRaw(data, clk);

  // calculation for temperature given in data sheet
  temp = D1 + (D2 * obtVal);

  return temp;
}

float readTempF(int data, int clk) {
  int obtVal;
  float temp;

  const float D1 = -40.0; // at 3.3V
  const float D2 = 0.018; // for 14 bit data

  // get the reading from sensor (raw data)
  obtVal = readTemperatureRaw(data, clk);

  skipCRC(data, clk); // skip performing CRC

  // calculation for temperature given in datasheet
  temp = D1 + (D2 * obtVal);

  return temp;
}

float readHumidity(int data, int clk) {
  int obtVal, obtValTemp;
  float relHumidity;
  float HumidityTemp;

//   // Humidity constants for 12 bit
//   const float c1 = -2.0468;
//   const float c2 = 0.0367;
//   const float c3 = -0.0000015955; //-1.5955e-6

//   // Temperature constants for 12 bit
//   const float t1 = 0.01;    // for 12 bit
//   const float t2 = 0.00008; // for 12 bit
  
  const float c1 = -4.0;       // for 12 Bit
  const float c2 =  0.0405;    // for 12 Bit
  const float c3 = -0.0000028; // for 12 Bit
  const float t1 =  0.01;      // for 14 Bit @ 5V
  const float t2 =  0.00008;   // for 14 Bit @ 5V

  // get the sensor reading raw data for humidity
  obtVal = getHumidityfrmSensor(data, clk);
  skipCRC(data, clk);

  // convert it into relative humidity for linear compensation
  relHumidity = c1 + c2 * obtVal + c3 * obtVal * obtVal;

  // get the sensor reading raw data for temp
  obtValTemp = readTempC(data, clk);
  skipCRC(data, clk);

  // calculate relative humidity with temperature compensation
  HumidityTemp = (obtValTemp - 25.0) * (t1 + t2 * obtVal) + relHumidity;

  return HumidityTemp;
}

int readTemperatureRaw(int data, int clk) {

  int val;
  gpio_set_direction(data, GPIO_MODE_INPUT);
  gpio_set_direction(clk, GPIO_MODE_OUTPUT);

  int cmd = 0b00000011;

  // Send command to read temperature from sensor
  sendCommandSHT(cmd, data, clk);

  // wait till sensor finishes reading data
  waitForResult(data);

  // read the data from the Sensor
  val = getData16SHT(data, clk);
  skipCRC(data, clk);

  return val;
}

int getHumidityfrmSensor(int data, int clk) {

  gpio_set_direction(data, GPIO_MODE_INPUT);
  gpio_set_direction(clk, GPIO_MODE_OUTPUT);

  // cmd to read humidity
  int cmd = 0b000000101;

  // sendcmd to read humidity to sensor
  sendCommandSHT(cmd, data, clk);

  // poll untill sensor has finished measuring data
  waitForResult(data);

  // read the data from sensor
  int val = getData16SHT(data, clk);

  return val;
}

// Send the command to read temp or humidity to microcontroller
void sendCommandSHT(int cmd, int data, int clk) {
  int ack;

  gpio_set_direction(data, GPIO_MODE_OUTPUT);
  gpio_set_direction(clk, GPIO_MODE_OUTPUT);

  // Initiate the start signal to sensor
  gpio_set_level(data, HIGH);
  gpio_set_level(clk, HIGH);
  gpio_set_level(data, LOW);
  gpio_set_level(clk, LOW);
  gpio_set_level(clk, HIGH);
  gpio_set_level(data, HIGH);
  gpio_set_level(clk, LOW);

  // send the command to sensor
  shiftOut(data, clk, MSBFIRST, cmd);

  gpio_set_level(clk, HIGH);
  gpio_set_direction(data, GPIO_MODE_INPUT);

  // Check acknowlwdgement if the sensor has ack the cmd
  // ack = gpio_get_level(data);
  // if (ack != LOW) {
  //   Serial.println("Error: ack not received");//error;
  // }
  gpio_set_level(clk, LOW);
  // check if sensor has started measuring data after ack
  // ack = gpio_get_level(data);
  // if (gpio_get_level(data) != HIGH) {
  //   Serial.println("Error : not able to measure");
  // }
}

void delay(int number_of_seconds) {
  // Converting time into milli_seconds
  int milli_seconds = 1000 * number_of_seconds;

  // Stroing start time
  clock_t start_time = clock();

  // looping till required time is not acheived
  while (clock() < start_time + milli_seconds)
    ;
}

// pooling for the sensor to complete measuring data
void waitForResult(int data) {
  int i;
  int flag = 0;
  int ack;
  gpio_set_direction(data, GPIO_MODE_INPUT);
  for (i = 0; i < 100; i++) {
    vTaskDelay(10 / portTICK_PERIOD_MS);
    ack = gpio_get_level(data);
    if (ack == LOW) {
      flag = 1;
      break;
    }
  }
  if (ack == HIGH) {
    flag = 2;
  }
}

// Read measurement from sensor
int getData16SHT(int data, int clk) {

  gpio_set_direction(data, GPIO_MODE_INPUT);
  gpio_set_direction(clk, GPIO_MODE_OUTPUT);

  int val1, read1 = 0;
  int MSBFIRST_ = MSBFIRST;
  read1 = shiftIn(data, clk, MSBFIRST_); // read MSB byte
  val1 = (read1 << 8);                   // read1*256;

  // Send acknowledgement to sensor that MSB byte is read
  gpio_set_direction(data, GPIO_MODE_OUTPUT);

  gpio_set_level(data, HIGH);
  gpio_set_level(data, LOW);
  gpio_set_level(clk, HIGH);
  gpio_set_level(clk, LOW);

  gpio_set_direction(data, GPIO_MODE_INPUT);

  read1 = 0;
  read1 = shiftIn(data, clk, MSBFIRST_); // read LSB byte of from the sensor
  val1 |= read1;                         // add LSB to MSB

  // return the read value from sensor
  return val1;
}

uint8_t shiftIn(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder) {

  uint8_t value = 0;
  uint8_t i;

  for (i = 0; i < 8; ++i) {
    gpio_set_level(clockPin, HIGH);
    if (bitOrder == LSBFIRST) {
      value |= gpio_get_level(dataPin) << i;

    } else {
      value |= gpio_get_level(dataPin) << (7 - i);
    }
    gpio_set_level(clockPin, LOW);
  }
  return value;
}

void shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val) {
  uint8_t i;

  for (i = 0; i < 8; i++) {
    if (bitOrder == LSBFIRST)
      gpio_set_level(dataPin, !!(val & (1 << i)));
    else
      gpio_set_level(dataPin, !!(val & (1 << (7 - i))));

    gpio_set_level(clockPin, HIGH);
    gpio_set_level(clockPin, LOW);
  }
}

// Skip the CRC
void skipCRC(int data, int clk) {

  gpio_set_direction(data, GPIO_MODE_OUTPUT);
  gpio_set_direction(clk, GPIO_MODE_OUTPUT);

  gpio_set_level(data, HIGH);
  gpio_set_level(clk, HIGH);
  gpio_set_level(clk, LOW);
}

esp_err_t sht1x_init(gpio_num_t data_pin, gpio_num_t clc_pin) {

  float temp_c;
  float temp_f;
  float humidity;

  printf("~~~~~~~~~~~~~~~~~\n");
  printf(" SHT1X Sensor \n");
  printf("~~~~~~~~~~~~~~~~~\n");
  printf("-> setting data pin %d \n", data_pin);
  printf("-> setting sck pin %d \n", clc_pin);
  printf("\n***************************\n");

  for (;;) {
    // Read values from the sensor
    temp_c = readTempC(data_pin, clc_pin);
    temp_f = readTempF(data_pin, clc_pin);
    humidity = readHumidity(data_pin, clc_pin);

    printf("-> Temperature:  %.2f °C \n", temp_c);
    printf("-> Temperature:  %.2f F \n", temp_f);
    printf("-> Humidity:  %.2f%% \n", humidity);
    printf("\n");

    vTaskDelay(3000 / portTICK_PERIOD_MS);
  }
  return ESP_OK;
}
