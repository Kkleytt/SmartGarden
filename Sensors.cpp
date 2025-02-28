#include "Sensors.h"

// Инициализация датчика температуры
DHT dht(SENSOR_TEMP, SENSOR_TEMP_TYPE);

const int MIN_SOIL = 600; // Максимально значение влажности почвы
const int MAX_SOIL = 1024; // Минимальное значение влажности почвы

// Глобальный массив состояний реле
bool relayState[6] = {false, false, false, false, false, false};

// Инициализация сенсоров
void initSensors(){
  pinMode(SENSOR_TEMP, INPUT);
  pinMode(SENSOR_SOIL, INPUT);
  pinMode(SENSOR_WATER, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
}

// Инициализация реле (установка режима работы и отключение реле)
void initRelays() {
  for (int i = 0; i < 6; i++) {
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], LOW);  // Отключаем все реле при старте
    relayState[i] = false;  // Обновляем состояние
  }
}


// Тестирование датчиков в системе
bool TestSystem() {
  bool status = true;
    
  // Проверка наличии и корректности работы датчика температуры
  dht.begin();
  delay(2000);
  float temperature = dht.readTemperature();
  if (isnan(temperature)) {
    printMessage("Ошибка: Датчик DHT11 не найден или неисправен.");
    //status = false;
  }
    
  // Проверка наличия и корректности работы датчика влажности почвы
  int soilReading = analogRead(SENSOR_SOIL);  // Считываем любое значение с пина
  if (soilReading < 0 || soilReading > 1023 || soilReading < 100) {
    printMessage("Ошибка: Датчик влажности почвы не найден или неисправен.");
    //status = false;
  }

  // Проверка наличия и корректности работы датчика наличия воды
  int waterReading = digitalRead(SENSOR_WATER);  // Считываем состояние
  if (waterReading != HIGH) {
    printMessage("Ошибка: Датчик уровня воды не работает или резервуар пуст.");
  }

  if (status) {  // Если проверка датчиков прошла неудачно
    printMessage("Все датчики работают корректно.");
  }

  return status;  // Возвращаем статус проверки
}

// Получение данных с датчика температуры
float readTemperature() {
  float temperature = dht.readTemperature();
  if (isnan(temperature)) {
    return -1;
  }
  return temperature;
}

// Получение данных с датчика воды в резервуаре
bool readWater() {
  int level = digitalRead(SENSOR_WATER);
  return level;
}

// Получение данных с датчика влажности почвы и преобразование в проценты
int readSoil() {
  int val = analogRead(SENSOR_SOIL);

  // Проверка показаний (находятся ли они в пределах корректировки)
  if (val < MIN_SOIL) {
    val = 100;
    return val;
  }
  val = map(val, MIN_SOIL, MAX_SOIL, 100, 0); // Перевод показаний в диапазоне от MAX_SOIL до MIN_SOIL до понятных от 0 до 100 с инвертированными значениями
  return val;
}

// Переключение реле по входным данным (индекс, состояние ВКЛ или ВЫКЛ)
void setRelay(int relayIndex, bool state){
  if (relayIndex >= 0 && relayIndex < 6) {
    digitalWrite(relayPins[relayIndex], state ? HIGH : LOW);
    relayState[relayIndex] = state;
  }
}

// Функция вывода сообщений в консоль
void printMessage(const String &text) {
  Serial.println(text);
}
