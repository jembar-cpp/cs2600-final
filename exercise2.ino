#define PIN_ANALOG_IN 4

void setup() {
  Serial.begin(115200);
}

void loop() { // read the temperature every second
  int adcValue = analogRead(PIN_ANALOG_IN);
  voltage = (float)adcValue / 4095.0 * 3.3;
  double Rt = 10 * voltage / (3.3 - voltage);
  double tempK = 1 / (1/(273.15 + 25) + log(Rt / 10)/3950.0);
  double tempC = tempK - 273.15;
  Serial.printf("ADC value : %d,\tVoltage : %.2fV, \tTemperature : %.2fC\n", adcValue, voltage, tempC);
  delay(1000);
}
