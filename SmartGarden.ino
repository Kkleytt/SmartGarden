#include "Sensors.h"
#include "Host.h"
#include "Tree.h"
#include "Tg.h"
#include "Timers.h"

bool test = false;                            // Переменная для хранения статуса тестирования
bool init_system = true;                      // Инициализация системы при запуске

TimerManager timerManager;                    // Инициализация менеджера таймеров

unsigned long timersPreviousMillis = 0;       // Переменная для хранения времени последнего запуска
const unsigned long timersinterval = 5000;    // Интервал (5 секунд)

unsigned long trigersPreviousMillis = 0;      // Переменная для хранения времени последнего запуска
const unsigned long trigersinterval = 15000;  // Интервал (15 секунд)

unsigned long notifyPreviousMillis = 0;       // Переменная для хранения времени последнего запуска
const unsigned long notifyinterval = 15000;   // Интервал (15 секунд)

unsigned long botPreviousMillis = 0;          // Переменная для хранения времени последнего запуска
const unsigned long botinterval = 10000;      // Интервал (10 секунд)

std::vector<String> notifications;            // Глобальный список уведомлений


void setup() {
  // Создаем обмен сообщениями между системой и ПК
  delay(500);
  Serial.begin(115200);
    
  // Ожидаем пока порт общения откроется с минимальной зедержкой
  while (!Serial){
    delay(10);
  }
  delay(2000);
  printMessage("Запуск системы...");

  // Запуск инициализации системы
  initSensors(); // Инициализация датчиков
  initRelays(); // Инициализация реле
  setupConfig(); // Инициализация файловой системы
  printMessage("Модули инициализированы.\n");
    
  // Запускаем проверку датчиков
  printMessage("Проверка датчиков:");
  test = TestSystem();
  printMessage("");

  // Проверка данных на ПЗУ и попытка подключения к WIFI
  if (init_system) {
    // Попытка чтения данных
    wifiSettings[0] = readConfig(SYS_CONFIG, "ssid").c_str();
    wifiSettings[1] = readConfig(SYS_CONFIG, "pass").c_str();
    wifiSettings[2] = readConfig(SYS_CONFIG, "token").c_str();
    wifiSettings[3] = readConfig(SYS_CONFIG, "admin").c_str();
    wifiSettings[4] = readConfig(SYS_CONFIG, "notify").c_str();
    wifiSettings[5] = readConfig(SYS_CONFIG, "utc").c_str();

    // Проверка доступности данных, их корректности и попытка подключения к WIFI сети
    if (wifiSettings[0] == "None" || wifiSettings[1] == "None" || wifiSettings[2] == "None" || !checkConnection()) {
      startAccessPoint(); // Запуск точки доступа, локального сервера, получение данных, тестирование соединение и перезапись системного конфига
      delay(1000);
    } else {
      printMessage("Проверка доступа пройдена успешно\n");
      printMessage("Имя - " + wifiSettings[0]);
      printMessage("Пароль - " + wifiSettings[1]);
      printMessage("Токен - " + wifiSettings[2]);
      printMessage("Админ - " + wifiSettings[3]);
      printMessage("Уведомления - " + wifiSettings[4] + "\n");
    }
    init_system = false;
  }

  // Финальное подключение к Wi-Fi после проверок
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifiSettings[0], wifiSettings[1]);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    if (millis() > 15000) ESP.restart();
  }

  // Синхронизация времени
  syncTime(wifiSettings[5].toInt() * 3600);
  printMessage("");

  // Проверка файла таймеров и их подключение
  File file = SPIFFS.open(TIMERS_CONFIG, "r"); // Проверка файла таймеров
  if (file) {
    String jsonTimersData = file.readString();
    file.close();

    // Проверяем корректность JSON
    if (validateTimersConfig(jsonTimersData)) { 
      timerManager.loadTimers(jsonTimersData);

    } else {
      printMessage("Не удалось запустить таймеры");
      test = false;
    }

  } else {
    printMessage("Не удалось открыть файл таймеров");
    test = false;
  }

  // Проверка файла таймеров и их подключение
  file = SPIFFS.open(TRIGERS_CONFIG, "r"); // Проверка файла таймеров
  if (file) {
    String jsonTimersData = file.readString();
    file.close();

    // Проверяем корректность JSON
    if (!validateTrigersConfig(jsonTimersData)){
      printMessage("Не удалось запустить тригеры");
      test = false;
    } 

  } else {
    printMessage("Не удалось открыть файл таймеров");
    test = false;
  }

  // Проверка файла пользователей
  file = SPIFFS.open(USERS_LIST, "r"); // Проверка файла таймеров
  if (file) {
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error) {
        Serial.println("Ошибка парсинга JSON!");
    } else {
      // Извлекаем массив пользователей
      JsonArray users = doc["users"];

      // Склеиваем все ID в одну строку
      String result = String(wifiSettings[3].c_str()) + ",";
      for (String user : users) {
          result += user + ",";
      }

      // Удаляем последнюю запятую
      if (result.endsWith(",")) {
          result.remove(result.length() - 1);
      }

      adminUsers = result;
    }
  } else {
    printMessage("Не удалось открыть файл таймеров");
    test = false;
  }


  // Инициализация Telegram-бота
  printMessage("\nЗапуск Telegram-бота.");
  initBot(wifiSettings[2]);
  printMessage("Telegram-бот успешно запущен.\n");
}

void loop() {
  test = true;
  if (test) {
    unsigned long currentMillis = millis();  // Текущее время

    bot.tick(); // Проверка новых сообщений в боте

    // Проверка бота
    if (currentMillis - botPreviousMillis >= botinterval) {  // Проверяем, прошло ли 10 секунд
      botPreviousMillis = currentMillis;  // Обновляем время последнего запуска

      // Проверка на перезапуск системы
      if (status_restart){
        bot.tickManual(); // Чтобы отметить сообщение прочитанным
        delay(5000); // Задержка для корректно работы
        ESP.restart(); // Перезапуск платв
      }

      // Проверка на сброс системы до заводских настроек
      if (status_reset){
        bot.tickManual(); // Чтобы отметить сообщение прочитанным
        delay(60000); // Задержка для безопасности
        resetSystem(); // Полный сброс системы
      }
    }

    // Проверка таймеров
    if (currentMillis - timersPreviousMillis >= timersinterval) {  // Проверяем, прошло ли 5 секунд
      timersPreviousMillis = currentMillis;  // Обновляем время последнего запуска

      updateTime();  // Обновление времени
      delay(200);
      timerManager.checkTimers();  // Проверка таймеров
    }

    // Проверка тригеров
    if (currentMillis - trigersPreviousMillis >= trigersinterval) {  // Проверяем, прошло ли 15 секунд
      trigersPreviousMillis = currentMillis;  // Обновляем время последнего запуска

      checkTrigers(); // Проверяем состояние тригеров
    }

    // Проверка уведомлений
    if (currentMillis - notifyPreviousMillis >= notifyinterval) {  // Проверяем, прошло ли 15 секунд
      notifyPreviousMillis = currentMillis;  // Обновляем время последнего запуска

      // Проверяем есть ли новые уведомления
      if (!notifications.empty()) {
              sendNotifications();
          }

    }
  }
}
