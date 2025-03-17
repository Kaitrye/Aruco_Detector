#define PIN_RECV 11 // Вывод, к которому подключен ИК-приёмник 
#define PIN_IN1 6 // Вывод управления правого колеса 
#define PIN_IN2 7 // Вывод управления правого колеса 
#define PIN_IN3 8 // Вывод управления левого колеса 
#define PIN_IN4 9 // Вывод управления левого колеса 
#define PIN_ENA 10 // Вывод управления скоростью правого колеса 
#define PIN_ENB 5 // Вывод управления скоростью левого колеса 
#define SPEED 255 // Скорость (0-255) 
// Коды с пульта управления
#define BUTTON_FORWARD 0xFF18E7 // Код кнопки ВПЕРЁД 
#define BUTTON_LEFT 0xFF10EF // Код кнопки ВЛЕВО 
#define BUTTON_RIGHT 0xFF5AA5 // Код кнопки ВПРАВО 
#define BUTTON_BACK 0xFF4AB5 // Код кнопки НАЗАД 
#define OK 0xFF38C7 // Код кнопки ОК

#include <IRremote.h> // Библиотека для работы с ИК-приёмником 

IRrecv irrecv(PIN_RECV); // Создание объекта работы с ИК-приёмником 
decode_results results; // Переменная для хранения результата декодирования 

void setup() 
{ 
  Serial.begin(9600);
  irrecv.enableIRIn(); // Инициализация ИК-приёмника 
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

void loop() { 
  // Ждём поступления сигнала с пульта ДУ 
  if (irrecv.decode(&results)) 
  { 
    // Анализируем полученный результат 
    switch(results.value) 
    { 
      case BUTTON_FORWARD: // Движение ВПЕРЁД 
      digitalWrite(PIN_IN1, HIGH); 
      digitalWrite(PIN_IN2, LOW); 
      digitalWrite(PIN_IN3, HIGH); 
      digitalWrite(PIN_IN4, LOW); 
      Serial.println(results.value, HEX);
      Serial.println(1);
      break; 
      case BUTTON_LEFT: // Поворот ВЛЕВО 
      digitalWrite(PIN_IN1, LOW); 
      digitalWrite(PIN_IN2, HIGH); 
      digitalWrite(PIN_IN3, HIGH); 
      digitalWrite(PIN_IN4, LOW); 
      Serial.println(results.value, HEX);
      Serial.println(2);
      break; 
      case BUTTON_RIGHT: // Поворот ВПРАВО 
      digitalWrite(PIN_IN1, HIGH); 
      digitalWrite(PIN_IN2, LOW); 
      digitalWrite(PIN_IN3, LOW); 
      digitalWrite(PIN_IN4, HIGH); 
      Serial.println(results.value, HEX);
      Serial.println(3);
      break; 
      case BUTTON_BACK: // Движение НАЗАД 
      digitalWrite(PIN_IN1, LOW); 
      digitalWrite(PIN_IN2, HIGH); 
      digitalWrite(PIN_IN3, LOW); 
      digitalWrite(PIN_IN4, HIGH); 
      Serial.println(results.value, HEX);
      Serial.println(4);
      break; 
      case OK:
      digitalWrite(PIN_IN1, LOW); 
      digitalWrite(PIN_IN2, LOW); 
      digitalWrite(PIN_IN3, LOW); 
      digitalWrite(PIN_IN4, LOW); 
      Serial.println(results.value, HEX);
    } 
    irrecv.resume(); 
  } 
}

