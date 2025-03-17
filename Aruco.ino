#define PIN_IN1 6 // Вывод управления правого колеса 
#define PIN_IN2 7 // Вывод управления правого колеса 
#define PIN_IN3 8 // Вывод управления левого колеса 
#define PIN_IN4 9 // Вывод управления левого колеса 
#define PIN_ENA 1 // Вывод управления скоростью правого колеса 
#define PIN_ENB 4 // Вывод управления скоростью левого колеса 
#define SPEED 255 // Скорость (0-255) 

void setup()
{
  Serial.begin(9600);
  // Настройка на выход всех управляющих пинов Arduino 
  pinMode(PIN_IN1, OUTPUT); 
  pinMode(PIN_IN2, OUTPUT); 
  pinMode(PIN_IN3, OUTPUT); 
  pinMode(PIN_IN4, OUTPUT); 
  pinMode(PIN_ENA, OUTPUT); 
  pinMode(PIN_ENB, OUTPUT); 
  // Остановка моторов 
  digitalWrite(PIN_IN1, LOW); 
  digitalWrite(PIN_IN2, LOW); 
  digitalWrite(PIN_IN3, LOW); 
  digitalWrite(PIN_IN4, LOW); 
  analogWrite(PIN_ENA, SPEED); 
  analogWrite(PIN_ENB, SPEED); 
}

void loop()
{

  //powerOffAllLEDs();
  if (Serial.available() > 0) {
    int ledNumber = Serial.read() - '0';  

    switch (ledNumber) {
      case 1:
      //Serial.println(ledNumber);
      digitalWrite(PIN_IN1, HIGH); 
      digitalWrite(PIN_IN2, LOW); 
      digitalWrite(PIN_IN3, HIGH); 
      digitalWrite(PIN_IN4, LOW); 
      delay(100);
      break;
      case 0:
      //Serial.println(ledNumber);
      digitalWrite(PIN_IN1, LOW); 
      digitalWrite(PIN_IN2, LOW); 
      digitalWrite(PIN_IN3, LOW); 
      digitalWrite(PIN_IN4, LOW); 
      break;
      default: 
      // если неправильный пин, то ничего не делает
      // все светодиоды будут выключены
      break;
    }
  }
  }