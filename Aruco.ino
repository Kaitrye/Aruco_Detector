#define LED_1_PIN 2
#define LED_2_PIN 4
#define LED_3_PIN 6
#define LED_4_PIN 10
#define BUTTON_PIN 8

void powerOffAllLEDs()
{
digitalWrite(LED_1_PIN, LOW);
digitalWrite(LED_2_PIN, LOW);
digitalWrite(LED_3_PIN, LOW);
digitalWrite(LED_4_PIN, LOW);
}

void setup()
{
  Serial.begin(9600);
  pinMode(LED_1_PIN, OUTPUT);
  pinMode(LED_2_PIN, OUTPUT);
  pinMode(LED_3_PIN, OUTPUT);
  pinMode(LED_4_PIN, OUTPUT);
  powerOffAllLEDs();
}

void loop()
{

  if (Serial.available() > 0) {
    int ledNumber = Serial.read() - '0';
    powerOffAllLEDs();

    switch (ledNumber) {
      case 1:
      digitalWrite(LED_1_PIN, HIGH);
      break;
      case 2:
      digitalWrite(LED_2_PIN, HIGH);
      break;
      case 3:
      digitalWrite(LED_3_PIN, HIGH);
      break;
      case 4:
      digitalWrite(LED_4_PIN, HIGH);
      break;
      default: 
      // если неправильный пин, то ничего не делает
      // все светодиоды будут выключены
      break;
    }
  }
  }