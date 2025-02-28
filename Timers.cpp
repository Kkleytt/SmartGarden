#include "Timers.h"
#include "Sensors.h"
#include "Tree.h"
#include "Host.h"
#include "Tg.h"

extern std::vector<String> notifications; // Переменная для хранения уведомлений
String trigersConfig[3];                  // Переменная для хранения настроек тригеров
unsigned long lastTriggerTime = 0;        // Переменная для хранения времени последнего срабатывания триггера


// Загружаем и запускаем таймеры
void TimerManager::loadTimers(const String& jsonConfig) {
  StaticJsonDocument<1024> doc;
  DeserializationError error = deserializeJson(doc, jsonConfig);
  if (error) {
    printMessage("Ошибка разбора JSON!");
    return;
  }

  timers.clear(); // Очистка предыдущих таймеров

  for (JsonPair item : doc.as<JsonObject>()) {
    Timer timer;
    timer.name = item.key().c_str();
    timer.device = item.value()["device"].as<String>();

    parseTime(item.value()["on"], timer.onHour, timer.onMinute, timer.onSecond);
    parseTime(item.value()["off"], timer.offHour, timer.offMinute, timer.offSecond);

    JsonObject weekObj = item.value()["week"].as<JsonObject>();
    for (int i = 1; i <= 7; i++) {
      int internalIndex = (i % 7); // Преобразуем 1 (понедельник) в 0, 7 (воскресенье) в 6
      timer.week[internalIndex] = weekObj[String(i)] == true;
    }

    timer.isActive = false; // Таймер выключен при загрузке
    timers[timer.name] = timer;
  }
  printMessage("Таймеры загружены!");
}

// Проверяем статусы таймеров
void TimerManager::checkTimers() {
  time_t now = time(nullptr);
  struct tm timeinfo;
  localtime_r(&now, &timeinfo);  

  int currHour = timeinfo.tm_hour;
  int currMinute = timeinfo.tm_min;
  int currSecond = timeinfo.tm_sec;
  int currWeekday = getCurrentWeekday();

  for (auto& kv : timers) {
    Timer& timer = kv.second;

    if (!timer.week[currWeekday]) continue; // Пропуск, если день не активен

    time_t now = time(nullptr);
    struct tm* currentTime = localtime(&now);

    time_t lastOnTime = timer.lastOnTriggered;
    time_t lastOffTime = timer.lastOffTriggered;
      
    struct tm lastOnDate, lastOffDate;
    localtime_r(&lastOnTime, &lastOnDate);
    localtime_r(&lastOffTime, &lastOffDate);

    // Проверка включения
    bool canTurnOn = (lastOnDate.tm_yday == currentTime->tm_yday); // Включался ли уже сегодня?
    bool canTurnOff = (lastOffDate.tm_yday == currentTime->tm_yday); // Выключался ли уже сегодня?

    // Проверка на состояние (ВЫКЛ) и последнее время запуска
    if (!timer.isActive && !canTurnOn) { 
      if (isTimeToSwitch(timer, true, currHour, currMinute, currSecond)) {
        timer.isActive = true; // Установка состояния таймера (ВКЛ : ВЫКЛ)
        timer.lastOnTriggered = now; // Установка последнего времени включения (день с начала года)
        setDevice(String(timer.device), true); // Включение устройства
        addNotification("Сработал таймер " + String(timer.name) + ". Включено устройство - " + String(timer.device));
      }
    }

    // Проверка на состояние (ВКЛ) и последнее время запуска
    if (timer.isActive && !canTurnOff) {
      if (isTimeToSwitch(timer, false, currHour, currMinute, currSecond)) {
        timer.isActive = false; // Установка состояния таймера (ВКЛ : ВЫКЛ)
        timer.lastOffTriggered = now; // Установка последнего времени включения (день с начала года)
        setDevice(String(timer.device), false); // Выключение устройства
        addNotification("Сработал таймер " + String(timer.name) + ". Выключено устройство - " + String(timer.device));
      }
    }
  }
}

// Получаем актуальное время в формате (ЧЧ:ММ:СС)
void TimerManager::parseTime(const String& timeStr, int& hour, int& minute, int& second) {
  sscanf(timeStr.c_str(), "%d:%d:%d", &hour, &minute, &second);
}

// Сверяем текущее время и время в таймере
bool TimerManager::isTimeToSwitch(const Timer& timer, bool isOn, int currHour, int currMinute, int currSecond) {
  int targetHour = isOn ? timer.onHour : timer.offHour;
  int targetMinute = isOn ? timer.onMinute : timer.offMinute;
  int targetSecond = isOn ? timer.onSecond : timer.offSecond;

  int targetTimeSec = targetHour * 3600 + targetMinute * 60 + targetSecond;
  int currTimeSec = currHour * 3600 + currMinute * 60 + currSecond;

  return (currTimeSec >= targetTimeSec) && (currTimeSec <= targetTimeSec + 300);;
}

// Получаем текущий день недели
int TimerManager::getCurrentWeekday() {
  time_t now = time(nullptr);
  struct tm* timeinfo = localtime(&now);
  return (timeinfo->tm_wday == 0) ? 6 : timeinfo->tm_wday - 1; // Преобразование в 1-7
}

// Производим валидацию конфига для таймеров
bool validateTimersConfig(const String& jsonConfig) {
  StaticJsonDocument<1024> doc;
  DeserializationError error = deserializeJson(doc, jsonConfig);

  if (error) {
    printMessage("Ошибка парсинга JSON: " + String(error.c_str()));
    return false;
  }

  for (JsonPair kv : doc.as<JsonObject>()) {
    JsonObject timer = kv.value();

    if (!timer.containsKey("on") || !timer.containsKey("off") ||
      !timer.containsKey("device") || !timer.containsKey("week")) {
        printMessage("Ошибка: Таймер " + String(kv.key().c_str()) + " имеет неверную структуру.");
        return false;
      }

      // Проверка формата времени
      String onTime = timer["on"].as<String>();
      String offTime = timer["off"].as<String>();
      if (!isValidTimeFormat(onTime) || !isValidTimeFormat(offTime)) {
        printMessage("Ошибка: Неверный формат времени в таймере " + String(kv.key().c_str()));
        return false;
      }

      // Проверка дней недели
      JsonObject week = timer["week"];
      if (week.size() != 7) {
        printMessage("Ошибка: Поле 'week' у таймера " + String(kv.key().c_str()) + " имеет неверное количество дней");
        return false;
      }
      for (int i = 1; i <= 7; i++) {
        String key = String(i);
        if (!week.containsKey(key) || (week[key] != true && week[key] != false)) {
          printMessage("Ошибка: Неверные данные в 'week' у таймера " + String(kv.key().c_str()));
          return false;
        }
      }
  }

  printMessage("JSON-данные успешно прошли валидацию!");
  return true;
}

// Производим валидацию таймеров времени
bool isValidTimeFormat(const String& timeStr) {
  if (timeStr.length() != 8 || timeStr[2] != ':' || timeStr[5] != ':') return false;
  int hh = timeStr.substring(0, 2).toInt();
  int mm = timeStr.substring(3, 5).toInt();
  int ss = timeStr.substring(6, 8).toInt();
  return (hh >= 0 && hh < 24) && (mm >= 0 && mm < 60) && (ss >= 0 && ss < 60);
}

// Производим валидацию конфига тригеров и загружаем их в массив
bool validateTrigersConfig(const String& jsonConfig) {
  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, jsonConfig);

  if (error) {
    printMessage("Ошибка парсинга JSON: " + String(error.c_str()));
    return false;
  }

  // Проверка наличия обязательных ключей
  if (!doc.containsKey("min") || !doc.containsKey("max") || !doc.containsKey("time_on")) {
    printMessage("Ошибка: JSON не содержит обязательные поля (min, max, time_on)");
    return false;
  }

  // Извлечение значений
  int minValue = doc["min"];
  int maxValue = doc["max"];
  int timeOn = doc["time_on"];

  // Проверка диапазонов значений
  if (minValue < 0 || minValue > 100) {
    printMessage("Ошибка: Значение 'min' должно быть в диапазоне 0-100");
    return false;
  }
  if (maxValue < 0 || maxValue > 100 || maxValue <= minValue) {
    printMessage("Ошибка: Значение 'max' должно быть больше 'min' и в диапазоне 0-100");
    return false;
  }
  if (timeOn <= 0) {
    printMessage("Ошибка: Значение 'time_on' должно быть больше 0");
    return false;
  }

  printMessage("JSON-данные успешно прошли валидацию!");
  trigersConfig[0] = minValue;
  trigersConfig[1] = maxValue;
  trigersConfig[2] = timeOn;
  printMessage("Тригеры успешно загружены");
  return true;
}

// Проверяем тригеры
void checkTrigers(){
  int soil = readSoil(); // Получаем текущую влажность почвы
  bool water = readWater(); // Получаем количество воды в резервуаре 
  time_t now = time(nullptr); // Получаем текущее время в миллисекундах

  // Проверяем, прошло ли 10 минут с последнего уведомления
  if (now - lastTriggerTime < 600) {
    return;  // Если прошло меньше 10 минут, выходим из функции
  }

  // Проверка на высокое значение влажности почвы
  if (soil >= trigersConfig[1].toInt()){
    setRelay(RELAY_PUMP, false);
    addNotification("Сработал тригер: Высокая влажность почвы (" + String(soil)  + "%). Помпа временно отключена");
    printMessage("Запущен тригер: Высокая влажность почвы");
  }

  // Проверка на низкое значение влажности почвы
  if (soil <= trigersConfig[0].toInt()){
    if (water){
      setRelay(RELAY_PUMP, true);
      delay(trigersConfig[2].toInt() * 1000);
      setRelay(RELAY_PUMP, false);
      addNotification("Сработал тригер: Низкая влажность почвы (" + String(soil)  + "%). Помпа была включен на " + trigersConfig[2] + " сек.");
      printMessage("Запущен тригер: Низкая влажность почвы");
    }
  }

  // Проверка на наличие воды в резервуаре
  if (!water){
    setRelay(RELAY_PUMP, false);
    addNotification("Сработал тригер: Недостаточно воды в резервуаре, полив временно отключен");
    printMessage("Запущен тригер: Мало воды в резервуаре");
  }

  lastTriggerTime = now;  // Обновляем время последнего срабатывания
}

// Включаем | Выключаем устройства по названию
void setDevice(const String& deviceName, bool state){
  if (deviceName == "PUMP"){
    setRelay(RELAY_PUMP, state);
  }
  else if (deviceName == "FAN"){
    setRelay(RELAY_FAN, state);
  }
  else if (deviceName == "FITO1"){
    setRelay(RELAY_FITO_1, state);
  }
  else if (deviceName == "FITO2"){
    setRelay(RELAY_FITO_1, state);
    setRelay(RELAY_FITO_2, state);
  }
  else if (deviceName == "FITO3"){
    setRelay(RELAY_FITO_1, state);
    setRelay(RELAY_FITO_2, state);
    setRelay(RELAY_FITO_3, state);
  }
  else if (deviceName == "LAMP"){
    setRelay(RELAY_LAMP, state);
  }
  else {
    printMessage("Неизвестное устройство для управления");
    return;
  }
  printMessage("Устройство " + String(deviceName) + (state ? " Включено" : " Выключено"));
}