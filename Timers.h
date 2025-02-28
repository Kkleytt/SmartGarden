#ifndef TIMERS_H
#define TIMERS_H

#include <ArduinoJson.h>              // Библиотека для работы с JSON-объектами (парсинг, сериализация)
#include <Arduino.h>                  // Основная библиотека для работы с микроконтроллером NodeMCU (ESP8266/ESP32)
#include <ctime>                      // Библиотека для работы с временем (структуры времени, функции времени)
#include <map>                        // STL-контейнер "слоарь данных" (ключ-значение)
#include <vector>                     // STL-контейнер "динамический массив" (гибкая работа с массивами)
#include <time.h>                     // Стандартная библиотека для работы со временем


struct Timer { 
  String name;                        // Название таймера
  String device;                      // Устройство для управления
  int onHour, onMinute, onSecond;     // Время включения
  int offHour, offMinute, offSecond;  // Время выключения
  bool week[7];                       // Дни недели (0 - воскресенье, 6 - суббота)
  bool isActive = false;              // Активен ли сейчас таймер
  time_t lastOnTriggered = 0;         // Время последнего включения (инициализируется нулем)
  time_t lastOffTriggered = 0;        // Время последнего включения (инициализируется нулем)
};


class TimerManager { 
public:
  void loadTimers(const String& jsonConfig);  // Загрузка таймеров из JSON
  void checkTimers();                         // Проверка состояния таймеров
private:
  std::map<String, Timer> timers; // Хранение таймеров
  void parseTime(const String& timeStr, int& hour, int& minute, int& second);                         // Функция получения времени включения и выключения таймера
  bool isTimeToSwitch(const Timer& timer, bool isOn, int currHour, int currMinute, int currSecond);   // Функция проверки действия с таймером (ВКЛ / ВЫКЛ)
  int getCurrentWeekday();                                                                            // Функция получения текущего дня недели
};

extern TimerManager timerManager;                     // Объект для работы с функциями таймеров
extern std::vector<String> notifications;             // Переменная для хранения уведомлений
extern String trigersConfig[3];                       // Массив для хранения настроек тригеров
extern unsigned long lastTriggerTime;                 // Переменная для хранения последнего запуска тригеров

bool validateTimersConfig(const String& jsonConfig);  // Функция валидации файла timers.json
bool isValidTimeFormat(const String& timeStr);        // Функция проверки времени включения и выключения в файле timers.json
bool validateTrigersConfig(const String& jsonConfig); // Функция валидации файла trigers.json
void checkTrigers();                                  // Функция для проверки состояния тригеров
void setDevice(const String& deviceName, bool state); // Управление устройствами через название (PUMP, FAN, FITO1, FITO2, FITO3, LAMP)

#endif
