#include "Tg.h"
#include "Sensors.h"
#include "Host.h"
#include "Tree.h"
#include "Timers.h" 

FastBot bot;  // Перемнная для работы с Telegram-ботом
bool status_restart = false;
bool status_reset = false;

bool send_timers = false;
bool send_trigers = false;

extern std::vector<String> notifications; // Переменная для хранения уведомлений
String adminUsers;


// Функция инифиализаци бота и установка ТОКЕНА
void initBot(const String &token){
    bot.setToken(token.c_str()); // Подключаем токен к переменной bot 
    delay(100); 
    bot.attach(handleMessage);
    delay(100);
    bot.setChatID(adminUsers);
    delay(100);
    printMessage(adminUsers);
}

// Функция обработчик сообщений
void handleMessage(FB_msg& msg) {
  if (String(msg.text).length() <= 20) {
    printMessage("📩 Новое сообщение: " + msg.text);
  }
  
  String text;
  text = "";

  if (msg.text == "/start"){
    text = "Привет! Это твой Telegram-бот для управления боксом SmartGarden v1.0a.\n"
    "📊 /commands - Получить сводку о всех командах для управления системой\n";
  }
  else if (msg.text == "/commands") {
    text = "Список всех комманд:\n"
    "/get_data - Краткая информации о системе\n"
    "/get_all_data - Вся информации о системе\n"
    "/devices - Управление устройствами\n"
    "/settings - Настройка бокса\n";
  }
  else if (msg.text == "/devices"){
    text = "Включить:\n"
    "/on_pump - Включить полив на 5 секунд\n"
    "/on_fan - Включить вентиляцию\n"
    "/on_lamp - Включить подсветку\n"
    "/on_fito_1 - Включить фито-лампу на 30%\n"
    "/on_fito_2 - Включить фито-лампу на 60%\n"
    "/on_fito_3 - Включить фито-лампу на 100%\n"
    "Выключить:\n"
    "/off_fan - Выключить вентиляцию\n"
    "/off_lamp - Выключить подсветку\n"
    "/off_fito - Выключить фито-лампы\n"
    "/commands - Перейти назад\n";
  }
  else if (msg.text == "/get_data"){
    float temperature = readTemperature();
    int soil = readSoil();
    bool waterBox = readWater();
    bool irrigation = relayState[0];
    bool ventilation = relayState[1];

    text = "🌡 Температура: " + String(temperature, 1) + "°C\n" +
    "💧 Влажность почвы: " + String(soil) + "%\n" +
    "🚰 Вода: " + String(waterBox ? "Есть" : "Нет") + "\n" +
    "🌿 Полив: " + String(irrigation ? "ВКЛ" : "ВЫКЛ") + "\n" +
    "💨 Вентиляция: " + String(ventilation ? "ВКЛ" : "ВЫКЛ");
  }
  else if (msg.text == "/get_all_data"){
    // Чтение данных с датчиков
    float temperature = readTemperature();
    int soil = readSoil();
    bool waterBox = readWater();

      // Состояния реле
    String irrigation  = relayState[0] ? "ВКЛ" : "ВЫКЛ";
    String ventilation = relayState[1] ? "ВКЛ" : "ВЫКЛ";
    String fito_1      = relayState[2] ? "ВКЛ" : "ВЫКЛ";
    String fito_2      = relayState[3] ? "ВКЛ" : "ВЫКЛ";
    String fito_3      = relayState[4] ? "ВКЛ" : "ВЫКЛ";
    String lamp        = relayState[5] ? "ВКЛ" : "ВЫКЛ";
    String wifi_status = wifiSettings[0];  // Используем String напрямую

    // Формируем сообщение
    text = "🌡 Температура: " + String(temperature, 1) + "°C\n" +
    "💧 Влажность почвы: " + String(soil) + "%\n" +
    "🚰 Вода: " + String(waterBox ? "Есть" : "Нет") + "\n" +
    "💦 Полив: " + irrigation + "\n" +
    "🌬 Вентиляция: " + ventilation + "\n" +
    "🔆 Фито-лампа 1: " + fito_1 + "\n" +
    "🔆 Фито-лампа 2: " + fito_2 + "\n" +
    "🔆 Фито-лампа 3: " + fito_3 + "\n" +
    "💡 Подсветка: " + lamp + "\n" +
    "📶 Wi-Fi: " + wifi_status;
  }
  else if (msg.text == "/on_pump"){
    setRelay(RELAY_PUMP, true);
    text = "🟢 Успех: Помпа включена на 5 секунд";
    delay(5000);
    setRelay(RELAY_PUMP, false);
  }
  else if (msg.text == "/on_fan"){
    setRelay(RELAY_FAN, true);
    text = "🟢 Успех: Вентиляция включена";
  }
  else if (msg.text == "/on_lamp"){
    setRelay(RELAY_LAMP, true);
    text = "🟢 Успех: Подсветка бокса включена";
  }
  else if (msg.text == "/on_fito_1"){
    setRelay(RELAY_FITO_1, true);
    text = "🟢 Успех: Включена фито-лампа на 30% мощности";
  }
  else if (msg.text == "/on_fito_2"){
    setRelay(RELAY_FITO_1, true);
    setRelay(RELAY_FITO_2, true);
    text = "🟢 Успех: Включена фито-лампа на 60% мощности";
  }
  else if (msg.text == "/on_fito_3"){
    setRelay(RELAY_FITO_1, true);
    setRelay(RELAY_FITO_2, true);
    setRelay(RELAY_FITO_3, true);
    text = "🟢 Успех: Включена фито-лампа на 100% мощности";
  }
  else if (msg.text == "/off_fan"){
    setRelay(RELAY_FAN, false);
    text = "🟢 Успех: Вентиляция выключена";
  }
  else if (msg.text == "/off_lamp"){
    setRelay(RELAY_LAMP, false);
    text = "🟢 Успех: Подсветка бокса отключена";
  }
  else if (msg.text == "/off_fito"){
    setRelay(RELAY_FITO_1, false);
    setRelay(RELAY_FITO_2, false);
    setRelay(RELAY_FITO_3, false);
    text = "🟢 Успех: Фито-лампы выключены";
  }
  else if (msg.text == "/settings"){
    text = "Список дополнительных команд:\n"
    "/set_users - Установка прав-доступа\n"
    "/set_timers - Инструкция по установке таймеров\n"
    "/set_triggers - Инструкция по установке триггеров\n"
    "/test - Тестирование датчиков системы\n"
    "/critical - Опасные настройки\n"
    "/commands - Перейти назад\n";
  }
  else if (msg.text == "/test"){
    text = "";

    DHT dht(SENSOR_TEMP, SENSOR_TEMP_TYPE);

    dht.begin();
    delay(2000);
    float temperature = dht.readTemperature();
    if (isnan(temperature)) text += "Ошибка в работе датчика температуры\n";

    int soilReading = analogRead(SENSOR_SOIL);
    if (soilReading < 0 || soilReading > 1023 || soilReading < 100) text += "Ошибка в работе датчика влажности почвы\n";

    int waterReading = digitalRead(SENSOR_WATER); 
    if (waterReading != HIGH) text += "Ошибка в работе датчика воды в резервуаре\n";
  }
  else if (msg.text == "/critical" && msg.chatID == String(wifiSettings[3])){
    text = "!Будьте очень осторожны!\n"
    "/reset_system - Полный сброс системы\n"
    "/restart - Перезапуск системы\n"
    "/set_notify_off - Отключить уведомления о системе\n"
    "/set_notify_on - Включить уведомления о системе\n"
    "/settings - Перейти назад\n";
  }
  else if (msg.text == "/reset_system" && msg.chatID == String(wifiSettings[3])){
    text = "🟡 Инструкция: Для подтверждения операции отправьте команду /reset_system_true "
    "и через 60 секунд система будет сброшена. Ессли не хотите сбрасывать систему "
    "выполните команду /critical и вернитесь назад в меню.";
  }
  else if (msg.text == "/reset_system_true" && msg.chatID == String(wifiSettings[3])){
    text = "🟢 Успех: Начался процесс сброс устройства, через 60 секунд система будет сброшена";
    status_reset = true;
  }
  else if (msg.text == "/restart" && msg.chatID == String(wifiSettings[3])){
    text = "🟢 Успех: Начат процесс перезапуск устройства, система выключиться через 5 секунд";
    status_restart = true;
  }
  else if (msg.text == "/set_notify_off" && msg.chatID == String(wifiSettings[3])){
    updateConfig(SYS_CONFIG, "notify", "false");
    wifiSettings[4] = "false";
    text = "🟢 Успех: Уведомления о системе - выключены";
  }
  else if (msg.text == "/set_notify_on" && msg.chatID == String(wifiSettings[3])){
    updateConfig(SYS_CONFIG, "notify", "true");
    wifiSettings[4] = "true";
    text = "🟢 Успех: Уведомления о системе - включены";
  }
  else if (msg.text == "/set_timers" && msg.chatID == String(wifiSettings[3])){
    text = "🟡 Инструкция: Отправьте настройки таймеров описанных в данной инструкции - None";
    send_timers = true;
  }
  else if (msg.text == "/set_trigers" && msg.chatID == String(wifiSettings[3])){
    text = "🟡 Инструкция: Отправьте настройки тригеров описанных в данной инструкции - None";
    send_trigers = true;
  }
  else if (String(msg.text).startsWith("/set_users") && msg.chatID == String(wifiSettings[3])){
    String users = String(msg.text).substring(11); // Получаем все значение после /set_users
    users.trim();
    if (users.length() == 0){
      text = "🟡 Инструкция: Чтобы установить список пользователей которые могут пользоваться системой, "
    "отправьте мне команду /set_users [userId, userId, userId]";
    } else {
      users.replace("[", "");
      users.replace("]", "");
      users.replace(" ", "");

      adminUsers = String(wifiSettings[3]) + "," + String(users);

      initBot(wifiSettings[2]);

      saveUsersFile(USERS_LIST, String(adminUsers));
      text = "🟢 Успех: Список пользователей успешно применен, теперь они смогут пользоваться системой";
      
    }
    
  }
  else if (String(msg.text).startsWith("/set_utc") && msg.chatID == String(wifiSettings[3])) {  // Проверяем команду
    String offsetStr = String(msg.text).substring(9);  // Получаем значение после "/set_utc "
    offsetStr.trim();  // Убираем лишние пробелы

    if (offsetStr.length() == 0 || !offsetStr.toInt()) {  // Проверяем, введено ли число
      text = "🟡 Инструкция: Укажите корректное значение UTC смещения, например: /set_utc 3";
      return;
    }

    int utcOffset = offsetStr.toInt();  // Преобразуем в int
    int timeOffsetSeconds = utcOffset * 3600;  // Переводим в секунды

    // Синхронизация времени
    syncTime(timeOffsetSeconds);

    text = "🟢 Успех: Часовой пояс успешно изменен на UTC " + String(utcOffset);

    wifiSettings[5] = offsetStr;
    updateConfig(SYS_CONFIG, "utc", wifiSettings[5].c_str());
  }
  else if (String(msg.text).startsWith("{json}") && msg.chatID == String(wifiSettings[3])) {
    String user_msg = String(msg.text);
    user_msg.replace("\n", "");
    user_msg.replace("\t", "");
    user_msg.replace("\r", "");
    user_msg.replace(" ", "");

    if (user_msg.substring(6, 14) == "{timers}"){
      printMessage("📩 Новое сообщение: /send_timers");
      String data = user_msg.substring(14, user_msg.length() - 7); // Обрезаем из строки экранирование
      printMessage(data);

      // Проверяем, является ли строка корректным JSON
      bool status = validateTimersConfig(data); // Второй этап проверки настроек (Проверка корректной структуры)
      if (status){

        // Открываем файл и записываем JSON
        File file = SPIFFS.open(TIMERS_CONFIG, "w");

        // Проверка на корректно открытый файл
        if (!file) {
          text = "🔴 Ошибка: Не удалось открыть файл для записи!";
        } else {
          file.print(data);  // Записываем данные
          file.close(); // Закрываем файл

          timerManager.loadTimers(data); // Загружаем таймеры в систему
          text = "🟢 Успех: Таймеры успешно сохранены и запущены в работу";
        }

      } else {
        text = "🔴 Ошибка: Некорректное форматирование настроек таймеров";
      }

    } 
    else if (user_msg.substring(6, 15) == "{trigers}"){
      printMessage("📩 Новое сообщение: /send_trigers");
      String data = user_msg.substring(15, user_msg.length() - 7);

      bool status = validateTrigersConfig(data);
      if (status) {
        // Открываем файл и записываем JSON
        File file = SPIFFS.open(TRIGERS_CONFIG, "w");

        if (!file) {
          text = "🔴 Ошибка: Не удалось открыть файл для записи!";
        } else {
          file.print(data);  // Записываем данные
          file.close(); // Закрываем файл
          text = "🟢 Успех: Тригеры успешно сохранены и запущены в работу";
        }
      } else {
        text = "🔴 Ошибка: Некорректное форматирование настроек тригеров";
      }
    }
  }
  else {
    text = "⚠ Неизвестная команда! Проверьте правильность введенных данных";    
  }

  bot.sendMessage(text, msg.chatID); // Отправляем новое сообщение
}

// Функция для отправки уведомлений пользователю
void sendNotifications() {
    while (!notifications.empty()) {
      if (wifiSettings[4] == "true"){
        String message = notifications.front();  // Берём первое сообщение
        bot.sendMessage("🔔 Уведомление: " + message, String(wifiSettings[3])); // Отправляем сообщение пользователю
        notifications.erase(notifications.begin()); // Удаляем из списка
      }
    }
}

// Функция добавления уведомления в очередь
void addNotification(const String& message) {
  notifications.push_back(message);
}