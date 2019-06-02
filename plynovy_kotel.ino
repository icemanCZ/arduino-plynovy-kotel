#include <OneWire.h>

byte P_LED1 = 13;
byte P_LED2 = 11;
byte P_LED3 = 9;
byte P_LED4 = 7;
byte P_LED5 = 5;
byte P_LED6 = 3;

byte P_REL = 2;
byte P_CIDLO = 4;

int MIN_TEMP = 50;
int SOFT_MAX_TEMP = 60;
int MAX_TEMP = 65;
float step = 0;

int DELAY = 15 * 60;
int SOFT_DELAY = 90;

int elapsed = 0;
int softMaxElapsed = 0;
bool isOff = false;


bool first = true;

OneWire ds(P_CIDLO);

void setup()
{
	//Serial.begin(9600);

	pinMode(P_LED1, OUTPUT);
	pinMode(P_LED2, OUTPUT);
	pinMode(P_LED3, OUTPUT);
	pinMode(P_LED4, OUTPUT);
	pinMode(P_LED5, OUTPUT);
	pinMode(P_LED6, OUTPUT);
	pinMode(P_REL, OUTPUT);

	pinMode(P_CIDLO, INPUT);

	step = (float)(MAX_TEMP - MIN_TEMP) / 5;

}

void loop()
{
	if (first) {
		// warmup
		first = false;
		getTemp();
		delay(1000);
		getTemp();
		delay(1000);
	}


	float temp = getTemp();
	//Serial.println(temp);

	if (isOff)
	{
		// vypnuto, bud tam dokud neubehl DELAY nebo je teplota vetsi nez MAX_TEMP
		elapsed++;
		if (elapsed > DELAY && temp < MAX_TEMP)
		{
			isOff = false;
			elapsed = 0;
			softMaxElapsed = 0;
		}
	}
	else
	{
		// zapnuto
		if (temp > SOFT_MAX_TEMP || softMaxElapsed > 0)
		{
			// pokud jednou prekrocis SOFT_MAX_TEMP, vypni se po SOFT_DELAY
			softMaxElapsed++;
			if (softMaxElapsed > SOFT_DELAY)
				isOff = true;
		}

		if (temp > MAX_TEMP)
		{
			// pokud prekrocis MAX_TEMP, vypni se
			isOff = true;
		}
	}
	digitalWrite(P_REL, isOff ? LOW : HIGH);
	digitalWrite(P_LED6, isOff ? HIGH : LOW);

	digitalWrite(P_LED1, (temp > MIN_TEMP) ? HIGH : LOW);
	digitalWrite(P_LED2, (temp > MIN_TEMP + step) ? HIGH : LOW);
	digitalWrite(P_LED3, (temp > MIN_TEMP + 2 * step) ? HIGH : LOW);
	digitalWrite(P_LED4, (temp > MIN_TEMP + 3 * step) ? HIGH : LOW);
	digitalWrite(P_LED5, (temp > MIN_TEMP + 4 * step) ? HIGH : LOW);

	delay(1000);
}















float getTemp() {
	//returns the temperature from one DS18S20 in DEG Celsius

	byte data[12];
	byte addr[8];

	if (!ds.search(addr)) {
		//no more sensors on chain, reset search
		ds.reset_search();
		return -1000;
	}

	if (OneWire::crc8(addr, 7) != addr[7]) {
		Serial.println("CRC is not valid!");
		return -1000;
	}

	if (addr[0] != 0x10 && addr[0] != 0x28) {
		Serial.print("Device is not recognized");
		return -1000;
	}

	ds.reset();
	ds.select(addr);
	ds.write(0x44, 1); // start conversion, with parasite power on at the end

	byte present = ds.reset();
	ds.select(addr);
	ds.write(0xBE); // Read Scratchpad


	for (int i = 0; i < 9; i++) { // we need 9 bytes
		data[i] = ds.read();
	}

	ds.reset_search();

	byte MSB = data[1];
	byte LSB = data[0];

	float tempRead = ((MSB << 8) | LSB); //using two's compliment
	float TemperatureSum = tempRead / 16;

	return TemperatureSum;

}


