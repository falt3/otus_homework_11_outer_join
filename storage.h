#pragma once

#include <string>
#include <vector>
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
        std::mutex mutex;
    };
    StorageData() { 
        m_tables["A"] = std::unique_ptr<Table>(new Table());
        m_tables["A"]->table.push_back({0, "lean"});
        m_tables["A"]->table.push_back({1, "sweater"});
        m_tables["A"]->table.push_back({2, "frank"});
        m_tables["A"]->table.push_back({3, "violation"});
        m_tables["A"]->table.push_back({4, "quality"});
        m_tables["A"]->table.push_back({5, "precision"});
        m_tables["A"]->table.push_back({11, "aaaaaaa"});

        m_tables["B"] = std::unique_ptr<Table>(new Table());
        m_tables["B"]->table.push_back({3, "proposal"});
        m_tables["B"]->table.push_back({4, "example"});
        m_tables["B"]->table.push_back({5, "lake"});
        m_tables["B"]->table.push_back({6, "flour"});
        m_tables["B"]->table.push_back({7, "wonder"});
        m_tables["B"]->table.push_back({8, "selection"});
        m_tables["B"]->table.push_back({11, "bbbbbbb"});
    }

    std::string insert(const std::string& table, std::size_t id, const std::string& name);
    std::string truncate(const std::string& table);
    std::string intersection();
    std::string symmetric_difference();
    std::string select(const std::string& table);

private:
    std::map<std::string, std::unique_ptr<Table>> m_tables;
};