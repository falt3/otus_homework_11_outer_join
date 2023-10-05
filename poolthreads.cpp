#include "poolthreads.h"
#include <sstream>
#include <string>
#include <iterator>


ServiceStorage::ServiceStorage(int countThreads, std::shared_ptr<StorageData> storage) :
    m_storage(storage)
{
    m_threads.reserve(countThreads);
    for (int i = 0; i < countThreads; ++i) 
        m_threads.emplace_back(&ServiceStorage::worker, this);
}


void ServiceStorage::worker()
{
    while(true) {        
    std::unique_lock<std::mutex> lock(m_mutex);
    m_cv.wait(lock, [&]() { return !m_commands.empty() || m_flagExit; } );

    if (!m_commands.empty()) 
    {
        std::string cmdLine (std::move(m_commands.front()));
        m_commands.pop();
        lock.unlock();

        executeCommand(cmdLine);
    }
    else if (m_flagExit) break;
    }
}


void ServiceStorage::executeCommand(const std::string& cmdLine)
{
    std::stringstream ss(cmdLine);
    std::istream_iterator<std::string> begin(ss);
    std::istream_iterator<std::string> end;
    std::vector<std::string> params(begin, end);

    std::cout << std::this_thread::get_id() << " executeCommand: " << params[0] << "\n";

    std::shared_ptr<std::string> strRes(new std::string());

    if (params.size() > 0) {
        if (params[0] == "INSERT") {
            if (params.size() == 4) {
                int id = std::stoi(params[2]);
                *strRes = m_storage->insert(params[1], id, params[3]);
            }
            else *strRes = "ERR incorrect parameters\n";
        }
        else if (params[0] == "TRUNCATE") {
            if (params.size() == 2) {
                *strRes = m_storage->truncate(params[1]);
            }
            else *strRes = "ERR incorrect parameters\n";
        }
        else if (params[0] == "INTERSECTION") {
            if (params.size() == 1) {
                *strRes = m_storage->intersection();
            }
            else *strRes = "ERR incorrect parameters\n";
        }
        else if (params[0] == "SYMMETRIC_DIFFERENCE") {
            if (params.size() == 1) {
                *strRes = m_storage->symmetric_difference();
            }
            else *strRes = "ERR incorrect parameters\n";
        }
        else if (params[0] == "SELECT") {
            if (params.size() == 2) {
                *strRes = m_storage->select(params[1]);
            }
            else *strRes = "ERR incorrect parameters\n";
        }
        else 
            *strRes = "ERR there is no such command\n";
    }

    notify(strRes);
}

//-----------------------------------------------------------
void ServiceStorage::command(std::string&& cmdLine)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_commands.push(std::move(cmdLine));
    m_cv.notify_one();
}
