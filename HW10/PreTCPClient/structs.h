#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdint.h>
#include <QDateTime>

// Проверяем, компилируется ли проект как DLL
#ifdef BUILDING_MY_DLL // Если проект собирается как DLL
#define DLL_EXPORT __declspec(dllexport)
#else // Если проект использует DLL
#define DLL_EXPORT __declspec(dllimport)
#endif

// Определение идентификаторов и сообщений
#define ID 0xFFFE
#define TYPE_REQ 10
#define TYPE_ANS 20

// Перечисление типов сообщений
enum Messages {
    GET_TIME   = 100,  // Получить время
    GET_SIZE   = 101,  // Получить свободное место
    GET_STAT   = 102,  // Получить статистику
    SET_DATA   = 200,  // Отправить данные
    CLEAR_DATA = 201   // Очистить данные на сервере
};

// Перечисление статусов сообщений
enum StatusMessages {
    STATUS_SUCCES = 1,        // Успешно
    ERR_NO_FREE_SPACE = 10,   // Недостаточно свободного места
    ERR_CONNECT_TO_HOST = 11, // Ошибка подключения к серверу
    ERR_NO_FUNCT = 20         // Функция не реализована
};

// Структура для заголовка пакета данных
#pragma pack(push, 1)
struct ServiceHeader {
    uint16_t id = ID;     // Идентификатор начала пакета
    uint16_t idData = 0;  // Идентификатор типа данных
    uint8_t status = 0;   // Статус сообщения (запрос/ответ)
    uint32_t len = 0;     // Длина данных
};
#pragma pack(pop)

// Структура для статистики сервера
struct StatServer {
    uint32_t incBytes;  // Принято байт
    uint32_t sendBytes; // Отправлено байт
    uint32_t revPck;    // Принято пакетов
    uint32_t sendPck;   // Отправлено пакетов
    uint32_t workTime;  // Время работы сервера в секундах
    uint32_t clients;   // Количество подключенных клиентов

    // Конструктор по умолчанию, инициализируем все поля в 0
    StatServer() {
        memset(this, 0, sizeof(*this));
    }
};

#endif // STRUCTS_H
