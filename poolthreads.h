#pragma once

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <vector>
#include <queue>

#include "observer.h"
#include "command.h"
#include "storage.h"


class ServiceStorage : public Publisher<std::string> {
public:
    ServiceStorage(int countThreads, std::shared_ptr<StorageData> storage);
    void command(std::string&& cmdLine);

protected:
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::vector<std::thread> m_threads;
    std::queue<std::string> m_commands;    ///< очередь команд
    std::shared_ptr<StorageData> m_storage;
    int m_flagExit  = 0;

private:
    void worker();
    void executeCommand(const std::string& cmdLine);
};
