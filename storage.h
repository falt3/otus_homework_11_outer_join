#pragma once

#include <string>
#include <vector>
#include <shared_mutex>
#include <mutex>
#include <map>
#include <memory>


class StorageData {
public:
    struct TypeDataTable {
        std::size_t id;
        std::string name;
    };
    struct Table {
        std::vector<TypeDataTable> table;
        std::shared_mutex mutex;
    };
    StorageData() { 
        m_tables["A"] = std::unique_ptr<Table>(new Table());
        m_tables["B"] = std::unique_ptr<Table>(new Table());
    }

    std::string insert(const std::string& table, std::size_t id, const std::string& name);
    std::string truncate(const std::string& table);
    std::string intersection();
    std::string symmetric_difference();
    std::string select(const std::string& table);
    std::string fill();

private:
    std::map<std::string, std::unique_ptr<Table>> m_tables;
};