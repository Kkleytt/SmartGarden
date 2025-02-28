#include "Tree.h"
#include "Sensors.h"

// Инициализация файловой системы
bool initFS() {
  if (!SPIFFS.begin()) {
    printMessage("Ошибка монтирования файловой системы.");
    return false;
  }
  printMessage("Файловая система успешно смонтирована.");
  return true;
}

// Функция сброса системы (удаление файлов конфигурации)
void resetSystem() {
  if (SPIFFS.exists(SYS_CONFIG)) SPIFFS.remove(SYS_CONFIG);
  if (SPIFFS.exists(TRIGERS_CONFIG)) SPIFFS.remove(TRIGERS_CONFIG);
  if (SPIFFS.exists(TIMERS_CONFIG)) SPIFFS.remove(TIMERS_CONFIG);
  if (SPIFFS.exists(USERS_LIST)) SPIFFS.remove(USERS_LIST);
  printMessage("Конфигурационные файлы удалены! Через 5 секунд система будет перезапущена");
  delay(5000);
  ESP.restart();
}

// Функция проверки существования файлов конфигурации
bool configExists(const char *filename) {
  return SPIFFS.exists(filename);
}

// Функция создания файла конфигурации с начальными значениями
bool createConfig(const char *filename, const char *defaultContent) {
  File file = SPIFFS.open(filename, "w");
  if (!file) {
    printMessage("Ошибка создания файла: " + String(filename));
    return false;
  }
  file.print(defaultContent);
  file.close();
  printMessage("Файл " + String(filename) + " успешно создан.");
  return true;
}

// Функция чтения параметров конфигурации
String readConfig(const char *filename, const char *key) {
  if (!SPIFFS.exists(filename)) return "";

  File file = SPIFFS.open(filename, "r");
  if (!file) return "";

  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, file);
  file.close();

  if (error) {
    printMessage("Ошибка чтения JSON.");
    return "";
  }

  if (doc.containsKey(key)) {
    return doc[key].as<String>();
  } else {
    return "";
  }
}

// Функция изменения параметра в конфигурационном файле
bool updateConfig(const char *filename, const char *key, const char *value) {
  if (!SPIFFS.exists(filename)) {
    printMessage("Ошибка отсутствия файла");
    return false;
  }

  File file = SPIFFS.open(filename, "r");
  if (!file) {
    printMessage("Ошибка открытия файла");
    return false;
  } 

  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, file);
  file.close();

  if (error) {
    printMessage("Ошибка обновления файла");
    return false;
  } 

  doc[key] = value;

  file = SPIFFS.open(filename, "w");
  if (!file) {
    printMessage("Неудачное обновление файла ");
    return false;
  }

  serializeJson(doc, file);
  file.close();
  printMessage("Файл " + String(filename) + " обновлен: " + String(key) + " = " + String(value));
  return true;
}

// Функция инициализации файловой системы и проверки конфигураций
void setupConfig() {
  if (!initFS()) return;

  // Начальные данные для СИСТЕМНОГО конфигурационного файла
  const char *defaultSysConfig = R"({
    "version": "1.0a",
    "ssid": "None",
    "pass": "None",
    "token": "None",
    "admin": "None",
    "notify": "true",
    "utc": "0"
  })";

  // Начальные данные для ПОЛЬЗОВАТЕЛЬСКИХ файлов конфигурации
  const char *defaultTimersConfig = R"({})";
  const char *defaultTrigersConfig = R"({
    "min": "None",
    "max": "None",
    "time_on": "None"
  })";
  const char *defaultUsersList = R"({
    "users": []
  })";

  if (!configExists(SYS_CONFIG)) createConfig(SYS_CONFIG, defaultSysConfig);
  if (!configExists(TIMERS_CONFIG)) createConfig(TIMERS_CONFIG, defaultTimersConfig);
  if (!configExists(TRIGERS_CONFIG)) createConfig(TRIGERS_CONFIG, defaultTrigersConfig);
  if (!configExists(USERS_LIST)) createConfig(USERS_LIST, defaultUsersList);
}

void saveUsersFile(const char *filename, String input) {
    if (!SPIFFS.begin()) {
        printMessage("Ошибка монтирования файловой системы!");
        return;
    }

    // Разбираем строку на числа
    DynamicJsonDocument doc(1024);
    JsonArray users = doc.createNestedArray("users");

    int start = 0;
    while (true) {
        int commaIndex = input.indexOf(',', start);
        if (commaIndex == -1) {
            users.add(input.substring(start).toInt()); // Добавляем последнее число
            break;
        }
        users.add(input.substring(start, commaIndex).toInt()); // Добавляем число
        start = commaIndex + 1;
    }

    // Открываем файл для записи
    File file = SPIFFS.open(filename, "w");
    if (!file) {
      printMessage("Ошибка открытия файла!");
      return;
    }

    // Записываем JSON в файл
    serializeJson(doc, file);
    file.close();

    printMessage("Данные сохранены в JSON:");
    return;
}