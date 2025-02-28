#include "Host.h"
#include "Tree.h"
#include "Sensors.h"

ESP8266WebServer server(80); // Открываем локальный сервер на порту 80 (http)
bool apModeActive = true; // Флаг работы точки доступа

WiFiUDP ntpUDP; // Подключения протокола UDP
NTPClient timeClient(ntpUDP, "pool.ntp.org", 3 * 3600, 60000); // Запрос времени через инетрнет (протокол, сайт, часовой пояс, частота обновления)

String wifiSettings[6]; // Глобальный массив для хранения данных Wi-Fi и токена

// Отправка html-страницы по ссылке http://192.168.4.1
void handleRoot() {
  String html = R"rawliteral(
  <!DOCTYPE html>
  <html lang="ru">
  <head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Настройки SmartGarden</title>
    <style>
      body {font-family: Arial, sans-serif; text-align: center; background: #f4f4f4; margin: 0; padding: 0; transition: background 0.3s, color 0.3s;}
      .container {width: 90%%; max-width: 400px; background: white; padding: 20px; margin: 50px auto; border-radius: 20px; box-shadow: 0 0 10px rgba(0,0,0,0.1); transition: background 0.3s, color 0.3s; position: relative; }
      .input-container { align-items: center; position: relative; }
      input {width: calc(100%% - 40px); padding: 10px; margin: 10px 0; border: 1px solid #ccc; border-radius: 5px; font-size: 16px; text-align: left; }
      .toggle-btn, .theme-btn {background: none; border: none; cursor: pointer; position: absolute; right: 10px; top: 50%%; transform: translateY(-50%%); }
      .theme-btn {right: 30px; }
      .toggle-btn {top: 20px;}
      button img {width: 24px; height: 24px;}
      .save-btn {width: 100%%; padding: 12px; background: #28a745; color: white; border: none; border-radius: 30px; font-size: 18px; cursor: pointer; margin-top: 10px;}
      .save-btn:hover {background: #218838;}
      .dark-mode {background: #222; color: #ddd;}
      .dark-mode .container {background: #333; color: #ddd;}
    </style>
    <script>
      function togglePassword() {
        var passwordField = document.getElementById("password");
        var toggleButton = document.getElementById("toggleBtn");
        if (passwordField.type === "password") {
          passwordField.type = "text";
          toggleButton.src = "https://img.icons8.com/material-sharp/50/visible.png";
        } else {
          passwordField.type = "password";
          toggleButton.src = "https://img.icons8.com/material-sharp/50/invisible.png";
        }
      }

      function toggleTheme() {
        document.body.classList.toggle("dark-mode");
        var themeButton = document.getElementById("themeBtn");
        themeButton.src = document.body.classList.contains("dark-mode") 
          ? "https://img.icons8.com/ios-filled/50/ffffff/light-off.png"
          : "https://img.icons8.com/ios-filled/50/000000/light-on.png";
        var visibleButton = document.getElementById("toggleBtn");
        visibleButton.src = document.body.classList.contains("dark-mode") 
          ? "https://img.icons8.com/material-sharp/50/ffffff/invisible.png"
          : "https://img.icons8.com/material-sharp/50/000000/invisible.png";
      }
    </script>
    </head>
    <body>
      <div class="container">
        <form action="/save" method="POST">
          <h2>Настройки сети 
            <button class="theme-btn" onclick="toggleTheme()">
              <img id="themeBtn" src="https://img.icons8.com/ios-filled/50/000000/light-on.png">
            </button>
          </h2>
          <div class="input-container">
            <input type="text" name="ssid" placeholder="Логин Wi-Fi" required>
          </div>
          <div class="input-container">
            <input type="password" name="password" id="password" placeholder="Пароль Wi-Fi" required>
            <button type="button" class="toggle-btn" onclick="togglePassword()">
              <img id="toggleBtn" src="https://img.icons8.com/material-sharp/50/invisible.png">
            </button>
          </div>
          <div class="input-container">
            <input type="text" name="token" placeholder="Токен Telegram-бота" required>
          </div>
          <div class="input-container">
            <input type="text" name="admin" placeholder="Telegram userID" required>
          </div>
          <button type="submit" class="save-btn">Сохранить</button>
        </form>
      </div>
    </body>
  </html>
  )rawliteral";

  server.send(200, "text/html", html); // Отправляем html-разметку
}

// Получение данных со страницы при получение метода POST http://192.168.4.1/save
void handleSave() {
  // Парсинг данных со страницы в глобальный массив
  wifiSettings[0] = server.arg("ssid");
  wifiSettings[1] = server.arg("password");
  wifiSettings[2] = server.arg("token");
  wifiSettings[3] = server.arg("admin");
  wifiSettings[4] = true;
    

  printMessage("==== Получен новые данные ====");
  printMessage("WI-FI SSID: " + wifiSettings[0]);
  printMessage("WI-FI PASS: " + wifiSettings[1]);
  printMessage("Telegram Token: " + wifiSettings[2]);
  printMessage("Admin: " + wifiSettings[3]);

  server.send(200, "text/html", "<h2>Данные сохранены! Проверка работы системы...</h2>"); // Отправка положительного ответа клиенту
    
  delay(1000); // Задержка для корректности работы

  // Проверка подключения к сети
  if (checkConnection()){
    apModeActive = false; // Перевод режима точки доступа в неактивный, для продолжения работы системы
  } else {
    // Повторное открытие точки доступа
    WiFi.mode(WIFI_AP);  // Переводим в режим точки доступа
    WiFi.softAP("SmartGarden", "12345678");  // Создаём Wi-Fi AP
  
    // Создание веб-страницы и триггеров отправки данных
    server.on("/", handleRoot);
    server.on("/save", HTTP_POST, handleSave);
    server.begin();
  }
    
}

// Создание точки доступа и запуск локального сервера
void startAccessPoint() {
  WiFi.mode(WIFI_AP);  // Переводим в режим точки доступа
  WiFi.softAP("SmartGarden", "12345678");  // Создаём Wi-Fi AP

  // Создание веб-страницы и триггеров отправки данных
  server.on("/", handleRoot);
  server.on("/save", HTTP_POST, handleSave);
  server.begin();

  printMessage("Запущена точка доступа SmartGarden, пароль - 12345678");
  printMessage("Адрес: http://" + WiFi.softAPIP().toString());

  // Пока не поступил запрос "/save", цикл приостанавливается
  while (apModeActive) {
    server.handleClient();
    delay(50); // Небольшая задержка для стабильности
  }

  // Обновление данных в конфиге
  updateConfig(SYS_CONFIG, "ssid", wifiSettings[0].c_str());
  updateConfig(SYS_CONFIG, "pass", wifiSettings[1].c_str());
  updateConfig(SYS_CONFIG, "token", wifiSettings[2].c_str());
  updateConfig(SYS_CONFIG, "admin", wifiSettings[3].c_str());

  printMessage("Выход из режима точки доступа.");
}

// Проверка подключения к WI-FI сети
bool checkConnection(){   
  WiFi.mode(WIFI_STA); 
  WiFi.begin(wifiSettings[0], wifiSettings[1]); // Запуск подключения к введенной WIFI-сети
  printMessage("Проверка WI-FI подключения");
  for (int i = 0; i < 10; i++) {
    if (WiFi.status() == WL_CONNECTED){
      printMessage("Wi-FI подключение установлено успешно");
      return true;
    }
    delay(1000);
  }
  printMessage("Wi-FI подключение не удалось");
  return false;
}

// Функция получения актуальной даты и времени с учетом смещения UTC
void syncTime(int utcOffset) {
    timeClient.end();  // Останавливаем текущий клиент
    timeClient = NTPClient(ntpUDP, "pool.ntp.org", utcOffset, 60000); // Пересоздаем объект
    timeClient.begin();  // Перезапускаем клиент с новым смещением

    printMessage("Синхронизация времени...");

    // Пытаемся обновить время, но не более 10 попыток
    int attempts = 0;
    while (!timeClient.update() && attempts < 10) {
        timeClient.forceUpdate();
        delay(500);
        attempts++;
    }

    // Если не удалось получить время, выводим ошибку
    if (attempts >= 10) {
        printMessage("Ошибка: не удалось синхронизировать время!");
        return;
    }

    time_t currentTime = timeClient.getEpochTime();
    struct timeval tv = { currentTime, 0 };
    settimeofday(&tv, NULL);

    printMessage("Синхронизировано время: " + String(currentTime) + " (UTC " + String(utcOffset) + ")");
}


// Функция локального обновления времени
void updateTime(){
  time_t now = time(nullptr);
  struct tm timeinfo;
  localtime_r(&now, &timeinfo);
}
