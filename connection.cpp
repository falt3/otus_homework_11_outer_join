#include "connection.h"
#include <memory>


void Connection::readyRead(boost::system::error_code err, std::size_t length) 
{
    if (err) { // закрытие сокета
        std::cout << "Connection::error\n";
        return;
    }
    if (length > 1) {
        std::string str {ba::buffer_cast<const char *>(m_buffer.data()), length-1};
        std::cout << std::endl;
        std::cout << std::this_thread::get_id() << " Connection::readyRead: " << str << std::endl;

        m_servStor->command(std::move(str));
    }
    else {
        // write("ERR\n");
    }

    m_buffer.consume(length);
    read();
}

void Connection::update(std::shared_ptr<std::string> &data)
{
    std::cout << std::this_thread::get_id() << " update: " << std::endl;
    write(*data);
}
