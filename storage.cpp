#include "storage.h"
#include <algorithm>
#include <sstream>


/**
 * @brief Функция вставки данных в таблицу
 * 
 * @param table     Название таблицы
 * @param id        Идентификатор 
 * @param name      Название
 * @return std::string  Сообщение с результатом выполнения операции
 */
std::string StorageData::insert(const std::string& table, std::size_t id, const std::string& name)
{
    if (auto iter = m_tables.find(table); iter != m_tables.end()) 
    {
        std::unique_lock<std::shared_mutex> lock(iter->second->mutex);
        if (auto it = std::find_if(iter->second->table.begin(), iter->second->table.end(), 
                [&id](auto& el) { return el.id == id; }); it == iter->second->table.end()) 
        {
            iter->second->table.push_back({id, name});
            std::sort(iter->second->table.begin(), iter->second->table.end(), 
                    [](auto& it1, auto& it2){ return (it1.id < it2.id); });
            return std::string("OK\n");
        }
        else {
            std::ostringstream sot;
            sot << "ERR duplicate " << id << "\n";
            return sot.str();        
        }
    }
    return std::string("ERR no such table\n");
}


/**
 * @brief Функция удаления данных из таблицы
 * 
 * @param table     Название таблицы
 * @return std::string  Сообщение с результатом выполнения операции
 */
std::string StorageData::truncate(const std::string& table)
{
    if (auto iter = m_tables.find(table); iter != m_tables.end()) 
    {
        std::unique_lock<std::shared_mutex> lock(iter->second->mutex);
        iter->second->table.clear();
        return std::string("OK\n");
    }
    return std::string("ERR no such table\n");
}


/**
 * @brief Функция нахождения пересечения данных таблиц "A" и "B"
 * 
 * @return std::string  Сообщение с результатом выполнения операции
 */
std::string StorageData::intersection()
{
    std::ostringstream sot;
    std::shared_lock<std::shared_mutex> lockA(m_tables["A"]->mutex);
    std::shared_lock<std::shared_mutex> lockB(m_tables["B"]->mutex);
    auto it1 = m_tables["A"]->table.cbegin();
    auto it2 = m_tables["B"]->table.cbegin();
    while (it1 != m_tables["A"]->table.cend() && it2 != m_tables["B"]->table.cend()) {
        if (it1->id == it2->id) {
            sot << it1->id << "," << it1->name << "," << it2->name << std::endl;
            it1++;
            it2++;
        }
        else if (it1->id > it2->id) {
            it2++;
        }
        else {
            it1++;
        }
    }
    sot << "OK\n";
    return sot.str();
}


/**
 * @brief Функция нахождения симметрической разности данных из таблиц "A" и "B"
 * 
 * @return std::string  Сообщение с результатом выполнения операции
 */
std::string StorageData::symmetric_difference()
{
    std::ostringstream sot;

    std::shared_lock<std::shared_mutex> lockA(m_tables["A"]->mutex);
    std::shared_lock<std::shared_mutex> lockB(m_tables["B"]->mutex);

    auto it1 = m_tables["A"]->table.cbegin();
    auto it2 = m_tables["B"]->table.cbegin();
    while (it1 != m_tables["A"]->table.cend() && it2 != m_tables["B"]->table.cend()) {
        if (it1->id == it2->id) {
            it1++;
            it2++;
        }
        else if (it1->id > it2->id) {
            sot << it2->id << ",," << it2->name << std::endl;
            it2++;
        }
        else {
            sot << it1->id << "," << it1->name << "," << std::endl;
            it1++;
        }
    }
    while (it1 != m_tables["A"]->table.cend()) {
        sot << it1->id << "," << it1->name << "," << std::endl;
        it1++;       
    }
    while (it2 != m_tables["B"]->table.cend()) {
        sot << it2->id << ",," << it2->name << std::endl;
        it2++;        
    }

    sot << "OK\n";
    return sot.str();
}


/**
 * @brief Функция вывода данных таблицы
 * 
 * @param table     Название таблицы
 * @return std::string  Сообщение с результатом выполнения операции
 */
std::string StorageData::select(const std::string& table)
{
    if (auto iter = m_tables.find(table); iter != m_tables.end()) 
    {
        std::shared_lock<std::shared_mutex> lock(iter->second->mutex);
        std::ostringstream sot;
        for (auto& it : iter->second->table) {
            sot << it.id << "," << it.name << std::endl;
        }
        sot << "OK\n";
        return sot.str();
    }
    return std::string("ERR no such table\n");
}


/**
 * @brief Функция заполнения таблиц тестовыми данными
 * 
 * @return std::string Сообщение с результатом выполнения операции
 */
std::string StorageData::fill()
{
    {
        std::unique_lock<std::shared_mutex> lockA(m_tables["A"]->mutex);

        m_tables["A"]->table.clear();    
        m_tables["A"]->table.push_back({0, "lean"});
        m_tables["A"]->table.push_back({1, "sweater"});
        m_tables["A"]->table.push_back({2, "frank"});
        m_tables["A"]->table.push_back({3, "violation"});
        m_tables["A"]->table.push_back({4, "quality"});
        m_tables["A"]->table.push_back({5, "precision"});
        m_tables["A"]->table.push_back({11, "aaaaaaa"});
    }

    {
        std::unique_lock<std::shared_mutex> lockB(m_tables["B"]->mutex);
        
        m_tables["B"]->table.clear();
        m_tables["B"]->table.push_back({3, "proposal"});
        m_tables["B"]->table.push_back({4, "example"});
        m_tables["B"]->table.push_back({5, "lake"});
        m_tables["B"]->table.push_back({6, "flour"});
        m_tables["B"]->table.push_back({7, "wonder"});
        m_tables["B"]->table.push_back({8, "selection"});
        m_tables["B"]->table.push_back({11, "bbbbbbb"});
    }
    return std::string("OK\n");
}
