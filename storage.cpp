#include "storage.h"
#include <algorithm>
#include <iostream>
#include <sstream>



std::string StorageData::insert(const std::string& table, std::size_t id, const std::string& name)
{
    if (auto iter = m_tables.find(table); iter != m_tables.end()) 
    {
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


std::string StorageData::truncate(const std::string& table)
{
    if (auto iter = m_tables.find(table); iter != m_tables.end()) 
    {
        iter->second->table.clear();
        return std::string("OK\n");
    }
    return std::string("ERR no such table\n");
}


std::string StorageData::intersection()
{
    std::ostringstream sot;
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


std::string StorageData::symmetric_difference()
{
    std::ostringstream sot;
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


std::string StorageData::select(const std::string& table)
{
    if (auto iter = m_tables.find(table); iter != m_tables.end()) 
    {
        std::ostringstream sot;
        for (auto& it : iter->second->table) {
            sot << it.id << "," << it.name << std::endl;
        }
        sot << "OK\n";
        return sot.str();
    }
    return std::string("ERR no such table\n");
}
