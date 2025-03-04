## Задачи для выполнения
### Блок 1: Простейщие функции, реализация подключения, настройки и инициализации устройств
- [ ] Создать новый проект
- [ ] Подключить вывод информации 
- [ ] Назначить пины датчиков и реле 
- [ ] Придумать структура конфигурационного файла 
- [ ] Написать функцию создания конфигурационного файла 
- [ ] Написать функции для считывания показателей датчиков 
- [ ] Написать функцию переключения реле 
- [ ] Написать функцию для отслеживания состояния реле по номеру 
- [ ] Написать модуль Wi-Fi
	- [ ] Попытка подключения к Wi-Fi 
	- [ ] Открытие точки доступа 
	- [ ] Создание веб-страницы 
	- [ ] Получение данных с веб-форм 
	- [ ] Сохранение данных в конфигигурационные файлы 
- [ ] Написать функцию записи данных в конфигурационный файл
- [ ] Написать функцию чтения данных из конфигурационного файла

##### Правки:
- [ ] Добавить резистор сопротивлением 10кОм на цифровые пины (DATA) для подключения реле (D3, D5, D6, D7, D8) соединив их с линией +5В (VCC)
- [ ] Переписать модуль для получения данных с датчика температуры
- [ ] Пересобрать электрическую схему по новой структуре
	


### Блок 2: Работа с Telegram-ботом, развертывание бота, проверка отправки команд
- [ ] Написать функцию получения значения токена
- [ ] Создать протсейщую структуру бота с выводом показаний датчиков
- [ ] Дополнить структуру функциями включения отдельных реле
- [ ] Подключить кнопку сброса к системе
- [ ] Реализовать функцию сброса системы до заводских настроек
- [ ] Внести правки в макет системы в Figma
- [ ] Написать логику работы Telegram-бота
- [ ] Написать функцию проверки уведомлений и автоматической отправки пользователю
- [ ] Написать функцию вывода сообщений в консоль
- [ ] Написать логику работы функции сохранения событий (для работы уведомлений)

##### Правки:
- [ ] Удалить модуль вывода сообщений на дисплей I2C, ради экономии памяти
- [ ] Перерисовать электрическую схему в Figma на новую структуру, дописать названия модулей и плат

### Блок 3: Сложнейшие таймеры, запуск параллельно выполняющихся задач, построение дерева ожидания
- [ ] Создать функцию считывания таймеров с конфига
- [ ] Написать функцию создания таймеров и присвоению им настроек
- [ ] Написать функцию перезапуска таймеров
- [ ] Написать функцию инициализации триггеров с конфига
- [ ] Написать функцию запуска триггеров
- [ ] Создать функцию запускающую все потоки с нечеткой загрузкой потоков
---
	
	
	
## Логика работы системы:
1. Запуск системы
2. Инициализация датчиков, реле, файловой системы
3. Проверка работы датчиков
4. Поиск системных данных
5. Попытка подключения
	- Открытие точки доступа и локального сервера
 	- Сохранение системных данных в файл
  	- Подключение к Wi-Fi
6. Синхронизация времени + смещение по часовому поясу
7. Чтение данных таймера + проверка валидности
8. Запуск таймеров
9. Чтение данных триггеров + проверка валидности
10. Запуск триггеров
11. Запуск Телеграм бота + настройка приватности
12. Запуск цикла
	- Проверка раз в 3.5 секунды сообщений в боте
 	- Проверка раз в 10 секунд опасных триггеров (сброс, перезапуск)
 	- Проверка раз в 5 секунд таймеров
 	- Проверка раз в 15 секунд триггеров
 	- Проверка раз в 15 секунд уведомлений
---
	
	
	
## Использованные библиотеки:
- **FS.h** – Библиотека для работы с файловой системой, позволяющая читать, 
		записывать и управлять файлами на файловой системе устройства 
		(например, на флеш-памяти ESP8266).
- **DHT.h** – Библиотека для работы с датчиками типа DHT (DHT11, DHT22), 
		предназначенная для измерения температуры и влажности.
- **Arduino.h** – Встроенная библиотека, предоставляющая основные функции 
			для работы с платами Arduino, такие как pinMode(), 
			digitalWrite(), delay() и другие базовые операции.
- **ArduinoJson.h** – Библиотека для работы с JSON-данными, которая позволяет
			легко сериализовать и десериализовать JSON-объекты в памяти, 
			а также работать с их содержимым.
- **ESP8266WiFi.h** – Библиотека для работы с Wi-Fi модулем на платах ESP8266, 
			которая предоставляет функции для подключения к сети, 
			управления подключением и работы с сетью.
- **ESP8266WebServer.h** – Библиотека для управления локальным сервером на 
			ESP8266. Она позволяет создать веб-сервер для обработки 
			HTTP-запросов, например, для взаимодействия с браузером или 
			другим устройством.
- **WiFiClientSecure.h** – Библиотека для работы с безопасными HTTPS-соединениями
			на ESP8266, которая предоставляет возможности для работы с 
			зашифрованными протоколами, обеспечивая безопасную передачу данных.
- **WiFiUdp.h** – Библиотека для работы с UDP-протоколом по Wi-Fi. Используется 
			для отправки и получения данных через UDP-соединения.
- **NTPClient.h** – Библиотека для синхронизации времени с сервером времени (NTP). 
			Это позволяет устройствам на базе ESP8266 получать точное время 
			через интернет.
- **FastBot.h** – Библиотека для создания Telegram-ботов на ESP8266. Она 
			предоставляет все необходимые инструменты для отправки и 
			получения сообщений, а также для взаимодействия с пользователями 
			через Telegram API.
- **time.h** – Стандартная библиотека для работы с временем и датой. Она 
			предоставляет функции для получения текущего времени, работы 
			с таймерами и временными метками.
- **ctime.h** – Библиотека для работы с временем в C++, предоставляет функционал 
			для работы с датами и временем в формате структуры tm, а также 
			функции для преобразования времени в строковые представления и обратно.
- **map.h** – Библиотека для работы с ассоциативными массивами (или словарями), 
			которые позволяют хранить пары ключ-значение.
- **vector.h** – Библиотека для работы с динамическими массивами (векторами), 
			которые автоматически увеличивают свой размер по мере добавления элементов.
---

## Распиновка системы:
| Pin | Назначение |
| :------------- | :------------- |
| A0 | Датчик влажности почвы |
| D0 | Реле фито-лампы 1 |
| D1 | Реле фито-лампы 2 |
| D2 | Реле фито-лампы 3 |
| D3 | Реле подсветки |
| D4 | Реле помпы |
| D5 | Реле вентилятора |
| D6 | Тактовая кнопка |
| D7 | Датчик температуры |
| D8 | Датчик воды |
| GND | Питание -5В |
| VIN | Питание +5В |
---


## Команды Telegram-бота:
| Команда | Описание |
| :------- | :------|
| /start | Первый запуск бота |
| /commands | Получить информацию о всех коммандах|
| /get_data | Получение кратких данных о системе |
| /get_all_data | Получение всех данных о системе (Датчики, реле, Интернет) |
| /devices | Управление устройствами |
| /on_pump | Включить полив (На время указанное в настройках) |
| /on_fan | Включить вентиляцию |
| /on_lamp | Включить подсветку боксу |
| /on_fito_1 | Включить фито-лампу 30% |
| /on_fito_2 | Включить фито-лампу 60% |
| /on_fito_3 | Включить фито-лампу 100% |
| /off_fan | Выключить вентиляцию |
| /off_lamp | Выключить подсветку бокса |
| /off_fito | Выключить фито-лампы |
| /settings | Настройки системы |
| /test | Тестирование датчиков в системе |
| /set_utc | Установка часового пояса для корректного определения времени |
| /set_timers | Установить таймеры (Отправив файл или JSON-сообщение) |
| /set_trigers | Установить триггеры (Отправив файл или JSON-сообщение) |
| /set_users | Список доверенных пользователей |
| /critical | Опасные настройки системы |
| /off_notify | Выключить функцию получения уведомлений |
| /on_notify | Включить функцию получения уведомлений (СТАНДАРТ) |
| /reset_system | Подтверждение сброса системы |
| /reset_system_true | Сброс системы до заводских настроек |
| /restart | Перезапуск системы |
---



## Инструкция взаимодействия для пользователя:
1. Вставить штекер питания в розетку
2. Запустить систему используя кнопку запуска на лицевой панели
3. Подождать +- 10 секунд до появления Wi-Fi сети "SmartGarden"
4. Подключиться к данной сети используя пароль "12345678"
5. Зайти в браузер и перейти по ссылке "http://192.168.4.1/"
6. Заполнить данные на веб странице:
	- SSID - Имя вашей Wi-Fi сети (Обязательно использовать сеть с частотой 2.4гГц)
	- PASS - Пароль от вашей Wi-Fi сети
	- TOKEN - Токен Телеграмм бота, созданного вами
	- UserID - Идентификатор пользователя в Telegram, чтобы он имел права админа (@myidbot)
7. Нажать кнопку сохранить
8. Подождать +- 30 секунд
9. Проверить работу системы. Зайти в Телеграмм-бота и отправить комманду /start
	Если бот отправил вам ответ - значит все работает
10. Советы:
	- Первым делом запустите команду /test для тестирования всей системы
	- Далее стоит проверить корреткность данных, отправьте команду /get_all_data
	- После настройте систему под ваше усмотрение
		- /on_notify - Включить уведомления, /off_notify - Выключить уведомления
		- /set_utc - Установить часовой пояс для корректного определения времени
		- /set_notify - Указать идентификатор чата куда будут отправляться уведомления
		- /set_user - Указать людей которые будут иметь доступ к боту (Пример: /set_users [656576324, 5642375625])
		- /set_timers - Отправьте боту JSON-сообщение с настройкой всех таймеров в системе (Пример сообщения ниже)
		- /set_triggers - Отправьте боту JSON-сообщение с настройкой всех триггеров в системе (Пример сообщения ниже)
		- /restart - Отправьте боту для перезапуска системы и применению всех изменений
---
		

## Пример сообщения для установки таймеров:
```
{json}{timers}
{
	"fito_lamp": { 
		"on": "20:14:00", 
		"off": "20:15:00", 
		"device": "FITO3", 
		"week": {
			"1": true, 
			"2": true,
			"3": true,
			"4": true,
			"5": true,
			"6": false,
			"7": false
		}
	},
	"pump": { 
		"on": "20:15:00", 
		"off": "20:15:30", 
		"device": "PUMP", 
		"week": { 
			"1": true, 
			"2": true,
			"3": true,
			"4": true,
			"5": true,
			"6": false,
			"7": false
		}
	}
}
{/json}
```
---

## Пример сообщения для установки триггеров:
```
{json}{trigers}
{
	"min": 20, 
	"max": 80,
	"time_on": 5 
}
{/json}
```
---



## Функционал реализованный в системе:
1. Инициализация файловой системы (датчиков, реле, файловой системы)
2. Тестирование системы (проверка датчиков на наличие и корректную информацию) 
3. Чтение, запись, обновление конфигурационных файлов
4. Подключение к WiFi
5. Открытие точки доступа
6. Создание веб-сервера и ответы на HTTP-запросы
7. Создание HTML-страницы с стилями CSS и скриптами JS 
8. Синхронизация времени через NTP + сдвиг часового пояса в реальном времени 
9. Двухуровневая валидация файла с настройками таймеров 
10. Двухуровневая валидация файла с настройками тригеров 
11. Создание динамических таймеров 
12. Создание полу-динамических тригеров
13. Инициализация Телеграмм-бота
14. Разделение потоков, для нечеткой загрузки ядра 
15. Проверка новых сообщений
16. Ведение списка уведомлений о системе 
17. Возможность установить доверенных пользователей (имеют теже права что у админа, кроме загрузки JSON) 
18. Имеет возможность удаленного перезапуска и полного сброса системы
19. Проверка статуса таймеров
20. Проверка статуса тригеров
21. Ведение логов в консоль Serial
22. Проверка пользователя на права администратора (для работы функции настроек)
23. [Альбом фотографий создания проекта](https://disk.yandex.ru/a/Ze5LA8RnSf4fwQ)
