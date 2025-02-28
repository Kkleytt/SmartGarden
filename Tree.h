#ifndef TREE_H
#define TREE_H

#include <ArduinoJson.h>                    // Библиотека для работы с системой хранения данных JSON
#include <FS.h>                             // Библиотека для работы с файловой системой платы Arduino

#define SYS_CONFIG "/sys_config.json"       // Глобальный путь до файла с системными настрйоками (SSID, PASS, TOKEN)
#define TIMERS_CONFIG "/timers.json"        // Глобальный путь до файла с настрйоками таймеров
#define TRIGERS_CONFIG "/trigers.json"     // Глобальный путь до файла с настройками триггеров
#define USERS_LIST "/users.json"            // Глобальный путь до файла с хранением доверенных пользователей

bool initFS();                                                                // Инициализация файловой системы
void resetSystem();                                                           // Сброс системы и удаление пользовательских данных (файлов конфигурации системы)
bool configExists(const char *filename);                                      // Проверка существования файлов конфигурации
bool createConfig(const char *filename, const char *defaultContent);          // Создание файла конфигурации с начальными данными
String readConfig(const char *filename, const char *key);                     // Чтение данных в файлах конфигурации
bool updateConfig(const char *filename, const char *key, const char *value);  // Обновление данных в файлах конфигурации
void setupConfig();                                                           // Проверка и инициализация файловой системы
void saveUsersFile(const char *filename, String input);

#endif
