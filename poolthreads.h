#pragma once

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <vector>
#include <queue>

#include "observer.h"
#include "storage.h"


/**
 * @brief Класс организующий многопоточную обработку запросов к хранилищу данных
 * 
 */
class ServiceStorage : public Publisher<std::string> {
public:
    ServiceStorage(int countThreads, std::shared_ptr<StorageData> storage);
    ~ServiceStorage();
    void command(std::string&& cmdLine);

private:
    std::mutex m_mutex;                         // мьютекс для синхронизации доступа к очереди запросов
    std::condition_variable m_cv;               // 
    std::vector<std::thread> m_threads;         // очередь потоков
    std::queue<std::string> m_commands;         // очередь запросов
    std::shared_ptr<StorageData> m_storage;     // Хранилище данных
    int m_flagExit  = 0;                        // Флаг для завершения работы потоков

    void worker();
    void executeCommand(const std::string& cmdLine);
};
