#pragma once

#include <iostream>
#include <memory>
#include <vector>
#include <boost/asio/read_until.hpp>
#include <boost/asio/write_at.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "poolthreads.h"
#include "observer.h"



namespace ba = boost::asio;
using tcp = ba::ip::tcp;

//-----------------------------------------------------------

/**
 * @brief   Класс сетевого соединения с клиентом
 * 
 */
class Connection : public std::enable_shared_from_this<Connection>, public Subscriber<std::string>
{
public:
    /**
     * @brief Construct a new Connection object
     * 
     * @param socket    Сокет
     * @param servStor  Служба для работы с хранилищем данных
     */
    Connection (tcp::socket&& socket, std::shared_ptr<ServiceStorage> servStor) : 
        m_socket(std::move(socket)),   
        m_servStor(servStor)
    {}

    /**
     * @brief Функция чтения данных из сокета
     * 
     */
    void read() {
        ba::async_read_until(m_socket, m_buffer, "\n",
            [self=shared_from_this()](boost::system::error_code err, std::size_t length) {
                self->readyRead(err, length);
            }   
        );
    }
    /**
     * @brief Функция записи данных в сокет
     * 
     * @param str   Сообщение для записи в сокет
     */
    void write(const std::string& str) {
        ba::async_write(m_socket, ba::buffer(str.data(), str.length()), 
            [](boost::system::error_code /*err*/, std::size_t /*length*/) {
                // std::cout << std::this_thread::get_id() << " writen\n";
            }
        );
    }

private:
    /**
     * @brief Функция обработки данных получееных из сокета
     * 
     * @param err       Ошибка при работе с сокетом
     * @param length    Длина принятых данных из сокета
     */
    void readyRead(boost::system::error_code err, std::size_t length) {
        if (err) { // закрытие сокета
            return;
        }
        if (length > 1) {
            std::string str {ba::buffer_cast<const char *>(m_buffer.data()), length-1};
            m_servStor->command(std::move(str));
        }
        m_buffer.consume(length);
        read();
    }

    /**
     * @brief Функция получения результирующего сообщения на запрос
     * 
     * @param data  Сообщение с результатом выполнения операции
     */
    void update(std::shared_ptr<std::string>& data) { write(*data); }

    tcp::socket m_socket;
    ba::streambuf m_buffer;
    std::shared_ptr<ServiceStorage> m_servStor;
};

