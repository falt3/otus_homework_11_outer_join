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

class Connection : public std::enable_shared_from_this<Connection>, public Subscriber<std::string>
{
public:
    Connection (tcp::socket&& socket, std::shared_ptr<ServiceStorage> servStor) : 
        m_socket(std::move(socket)),   
        m_servStor(servStor)
    {}
    ~Connection () {};

    void read() {
        ba::async_read_until(m_socket, m_buffer, "\n",
            [self=shared_from_this()](boost::system::error_code err, std::size_t length) {
                self->readyRead(err, length);
            }   
        );
    }
    void write(const std::string& str) {
        ba::async_write(m_socket, ba::buffer(str.data(), str.length()), 
            [](boost::system::error_code /*err*/, std::size_t /*length*/) {
                std::cout << std::this_thread::get_id() << " writen\n";
            }
        );
    }
private:
    void readyRead(boost::system::error_code err, std::size_t length);
    void update(std::shared_ptr<std::string>& data);

    tcp::socket m_socket;
    ba::streambuf m_buffer;
    std::shared_ptr<ServiceStorage> m_servStor;
};

