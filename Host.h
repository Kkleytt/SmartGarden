#ifndef HOST_H
#define HOST_H

#include <ESP8266WiFi.h>        // Библиотека для взаимодействия с WIFI модулем
#include <ESP8266WebServer.h>   // Библиотека для развертывания локального сервера
#include <FastBot.h>            // Библиотека для работы с Telegram API
#include <WiFiUdp.h>            // Поддержка UDP-соединений (используется для NTP)
#include <NTPClient.h>          // Клиент для получения времени с NTP-серверов
#include <time.h>               // Стандартная библиотека для работы со временем

extern String wifiSettings[6];  // Глобальный массив для хранения системных данных

void handleRoot();              // Отправка веб страницы на локальный сервер
void handleSave();              // Получение данных и их сохранения при переходе на страницу http://x.x.x.x/save
void startAccessPoint();        // Создание точки доступа и локального сервера
bool checkConnection();         // Проверка WI-FI соединения
void syncTime(int utcOffset);   // Функция синхронизации времени через интернет-ресурсы
void updateTime();              // Функция локального обновления времени

#endif
