#include <Servo.h>
#include <BitVoicer11.h>

BitVoicerSerial bvSerial = BitVoicerSerial();
byte dataType = 4;

Servo servoDoor;
Servo servoWindow;

int ledPins[] = {2, 3, 4, 5, 6, 7};
int servoPins[] = {9, 10};

const int temperaturePin = A0;
const double temperatureThreshold = 70;

const int buzzerPin = 13;
const int songLength = 18;
char notes[songLength] = {'c', 'd', 'f', 'd', 'a', ' ', 'a', 'g', ' ', 'c', 'd', 'f', 'd', 'g', ' ', 'g', 'f', ' '};
int beats[songLength] = {1, 1, 1, 1, 1, 1, 4, 4, 2, 1, 1, 1, 1, 1, 1, 4, 4, 2};
int tempo = 113;

boolean doorClosed = true;
boolean windowClosed = true;
boolean lights = false;

const int closedPosition = 180;
const int openPosition = 0;

void setup()
{
    servoDoor.attach(servoPins[0], 900, 2100);
    servoWindow.attach(servoPins[1], 900, 2100);
    servoWindow.write(closedPosition);
    servoDoor.write(closedPosition);
    delay(1000);

    pinMode(buzzerPin, OUTPUT);

    Serial.begin(9600);

    int index;
    for (index = 0; index <= 5; index = ++index)
    {
        pinMode(ledPins[index], OUTPUT);
    }
}

void loop()
{
    temperature();
}

void serialEvent()
{
    dataType = bvSerial.getData();

    if (dataType == BV_STR)
    {
        if (bvSerial.strData == "closeDoor")
        {
            if (!doorClosed)
                closeDoor();
        }
        else if (bvSerial.strData == "openDoor")
        {
            if (doorClosed)
            {
                openDoor();
                playMusic();
            }
        }
        else if (bvSerial.strData == "lightsOn")
        {
            if (!lights)
                lightsOn();
        }
        else if (bvSerial.strData == "lightsOff")
        {
            if (lights)
                lightsOff();
        }
        else if (bvSerial.strData == "discoLights")
            disco();
    }
}

void temperature()
{
    double temp = getTemperature();
    setWindow(temp);
}
void closeDoor()
{
    doorClosed = true;
    servoDoor.write(closedPosition);
    delay(1000);
}

void openDoor()
{
    doorClosed = false;
    servoDoor.write(openPosition);
    delay(1000);
}

double getTemperature()
{
    float voltage, degreesC, degreesF;
    voltage = getVoltage(temperaturePin);
    degreesC = (voltage - 0.5) * 100.0;
    degreesF = degreesC * (9.0 / 5.0) + 32.0;

    Serial.print("voltage: ");
    Serial.print(voltage);
    Serial.print("  deg C: ");
    Serial.print(degreesC);
    Serial.print("  deg F: ");
    Serial.println(degreesF);

    return degreesF;
}

void setWindow(double degreesF)
{
    if (degreesF < temperatureThreshold)
    {
        if (!windowClosed)
        {
            windowClosed = true;
            servoWindow.write(closedPosition);
        }
    }
    else
    {
        if (windowClosed)
        {
            windowClosed = false;
            servoWindow.write(openPosition);
        }
    }

    delay(1000);
}

float getVoltage(int pin)
{
    return (analogRead(pin) * 0.004882814);
}

void lightsOff()
{
    lights = false;

    int index;
    for (index = 0; index <= 5; index = ++index)
    {
        digitalWrite(ledPins[index], LOW);
        delay(10);
    }
}

void lightsOn()
{
    lights = true;

    int index;
    for (index = 0; index <= 5; index = ++index)
    {
        digitalWrite(ledPins[index], HIGH);
        delay(10);
    }
}

void disco()
{
    lightsOff();
    int disco = 0;
    while (disco < 500)
    {
        int discoPin = random(2, 7);
        digitalWrite(discoPin, !digitalRead(discoPin));
        delay(25);
        disco++;
    }
    for (int activePin = 0; activePin <= 5; activePin++)
    {
        if (digitalRead(ledPins[activePin]) == 0)
        {
            digitalWrite(ledPins[activePin], HIGH);
            delay(50);
        }
    }
}

void playMusic()
{ // Reused from Sparkfun guide!
    int i, duration;

    for (i = 0; i < songLength; i++)
    {
        duration = beats[i] * tempo;
        if (notes[i] == ' ')
            delay(duration);
        else
        {
            tone(buzzerPin, frequency(notes[i]), duration);
            delay(duration);
        }
        delay(tempo / 10);
    }
}

int frequency(char note)
{
    int i;
    const int numNotes = 8;
    char names[numNotes] = {'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C'};
    int frequencies[numNotes] = {262, 294, 330, 349, 392, 440, 494, 523};

    for (i = 0; i < numNotes; i++)
    {
        if (names[i] == note)
            return (frequencies[i]);
    }
    return (0);
}
