/**
 * @file main.cpp
 * @author Lipatkin Dmitry
 * @brief 
 * @version 0.1
 * @date 2023-10-03
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <iostream>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "connection.h"
#include "storage.h"
#include "poolthreads.h"

//-----------------------------------------------------------

void server(tcp::acceptor &acceptor, int& countConnection, std::shared_ptr<ServiceStorage> servStor) 
{
    // std::cout << "accept\n";
    acceptor.async_accept(
        [&acceptor, &countConnection, servStor] 
        (const boost::system::error_code& err,  tcp::socket socket) {
            if (!err) {
                ++countConnection;

                /// ожидание завершения соединения
                socket.async_wait(tcp::socket::wait_error, 
                    [&countConnection](const boost::system::error_code& error) {
                        --countConnection;
                        if (error) {
                            // std::cout << "error: " << error.value() << " " << error.category().name()
                            //     << " " << error.message() << " " << "\n";
                        }
                    }
                );

                // Новое соединение 
                const std::shared_ptr<Connection> connection { 
                    new Connection(std::move(socket), servStor) 
                };
                servStor->addSubscriber(connection);

                /// Обработчик для динамического блока
                connection->read();
            }

            server(acceptor, countConnection, servStor);
        }
    );
}

//-----------------------------------------------------------


int main(int argc, const char* argv[]) 
{
    int port = 19000;
    if (argc > 1)
        port = std::stoi(argv[1]);

    /// Хранилище данных
    std::shared_ptr<StorageData> storage (new StorageData());
    /// Обработчики команд
    
    std::shared_ptr<ServiceStorage> serviceStorage (new ServiceStorage(4, storage));


    /// количество текущих соединений (клиентов)
    int countConnection = 0;   

    ba::io_context io_context;
    tcp::acceptor acceptor {io_context, tcp::endpoint(tcp::v4(), port)};

    server(acceptor, countConnection, serviceStorage);

    io_context.run();    

    return 0;
}

