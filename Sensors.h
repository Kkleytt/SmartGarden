#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>            // Библиотека для взаимодействия с пинами платы Arduino
#include <DHT.h>                // Библиотека для работы с датчиками температуры серии DHT11/DHT22

#define SENSOR_SOIL A0          // Датчик влажности почвы АНАЛОГОВЫЙ (TZT 5V)
#define SENSOR_TEMP D1          // Датчик температуры серии DHT
#define SENSOR_TEMP_TYPE DHT11  // Тип датчика температуры DHT11/DHT22
#define SENSOR_WATER D2         // Датчик воды в резервуаре

#define RELAY_FITO_1 0          // Индекс реле фито-лампы 1
#define RELAY_FITO_2 1          // Индекс реле фито-лампы 2
#define RELAY_FITO_3 2          // Индекс реле фито-лампы 3
#define RELAY_LAMP 3            // Индекс реле подсветки
#define RELAY_PUMP 4            // Индекс реле помпы
#define RELAY_FAN 5             // Индекс реле вентилятора

extern bool relayState[6];                          // Глобальный список состояния модулей реле
const int relayPins[6] = {D0, D1, D2, D3, D4, D5};  // Список всех реле модулей

void initSensors();                                 // Инициализация сенсоров (Перевод пинов в режим ВХОД)
void initRelays();                                  // Инициализация реле-модулей (Перевод пинов в режим ВЫХОД, Выключение реле при старте)
bool TestSystem();                                  // Тестирование датчиков в системе
float readTemperature();                            // Функция чтения температуры
bool readWater();                                   // Функция чтения уровня воды в резервуаре
int readSoil();                                     // Функция чтения влажности почвы
void setRelay(int relayIndex, bool state);          // Функция управления реле-модулями
void printMessage(const String &text);              // Функция вывода сообщений на диисплей и в консоль

#endif
