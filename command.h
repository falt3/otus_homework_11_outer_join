#pragma once

#include <string>

struct Command {
    enum class Type {
        insert, 
        truncate,
        intersection,
        symmetric_difference,
        select,
    };
    // Command (TypeCommand type) : m_type(type) {}
    Type m_type;
    std::vector<std::string> params;
};


// struct CommandInsert : public Command {
//     CommandInsert(std::string table, std::size_t id, std::string name) : 
//         Command(TypeCommand::insert),
//         m_id(id),
//         m_table(table),
//         m_name(name) {}
//     std::size_t m_id;
//     std::string m_table;
//     std::string m_name;
// };


// struct CommandTruncate : public Command {
//     CommandTruncate(std::string table) : 
//         Command(TypeCommand::truncate), 
//         m_table(table) {}
//     std::string m_table;
// };


// struct CommandSelect : public Command {
//     CommandSelect(std::string table) : 
//         Command(TypeCommand::select), 
//         m_table(table) {}
//     std::string m_table;
// };
