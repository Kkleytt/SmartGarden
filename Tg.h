#ifndef TG_H
#define TG_H

#include <Arduino.h>                          // Библиотека для работы с функциями платы
#include <FastBot.h>                          // Библиотека для работы с Telegram API
#include <DHT.h>                              // Библиотека для работы с датчиками температуры серии DHT11/DHT22

extern FastBot bot;                           // Переменная для работы с Telegram-ботом
extern std::vector<String> notifications;     // Переменная для хранения уведомлений
extern bool status_restart;                   // Переменная для отслеживания перезапуска системы
extern bool status_reset;                     // Переменная для отслеживания сброса системы
extern String adminUsers;                     // Переменная для хранения списка доверенных пользователей

void initBot(const String &token);            // Инициализация Telegram-бота и присвоение токена
void handleMessage(FB_msg& msg);              // Функция обработчик сообщений
void sendNotifications();                     // Функция отправки уведомлений
void addNotification(const String& message);  // Функция добавления уведомлений в очередь

#endif