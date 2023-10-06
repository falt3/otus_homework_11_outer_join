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
#include <boost/asio/signal_set.hpp>

#include "connection.h"
#include "storage.h"
#include "poolthreads.h"

//-----------------------------------------------------------
/**
 * @brief Функция ожидания подключения и создания нового соединения
 * 
 * @param acceptor          Объект ожидающий подключение клиентов
 * @param countConnections  Текущее количество соединений с клиентами
 * @param servStor          Хранилище данных
 */
void server(tcp::acceptor &acceptor, int& countConnections, std::shared_ptr<ServiceStorage> servStor) 
{
    acceptor.async_accept(
        [&acceptor, &countConnections, servStor] 
        (const boost::system::error_code& err,  tcp::socket socket) {
            if (!err) {
                ++countConnections;

                // ожидание завершения соединения
                socket.async_wait(tcp::socket::wait_error, 
                    [&countConnections](const boost::system::error_code& /*error*/) {
                        --countConnections;
                        // if (error) {
                        //     // std::cout << "error: " << error.value() << " " << error.category().name()
                        //     //     << " " << error.message() << " " << "\n";
                        // }
                    }
                );

                /// Новое соединение 
                const std::shared_ptr<Connection> connection { 
                    new Connection(std::move(socket), servStor) 
                };
                servStor->addSubscriber(connection);

                connection->read();
            }

            server(acceptor, countConnections, servStor);
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
    
    /// Обработчик команд
    std::shared_ptr<ServiceStorage> serviceStorage (new ServiceStorage(4, storage));

    /// количество текущих соединений (клиентов)
    int countConnections = 0;   

    ba::io_context io_context;
    tcp::acceptor acceptor {io_context, tcp::endpoint(tcp::v4(), port)};

    server(acceptor, countConnections, serviceStorage);

    ba::signal_set signals{io_context, SIGINT, SIGTERM};
    signals.async_wait([&](auto, auto) { 
        io_context.stop(); 
    });

    std::vector<std::thread> threads;
    // const auto nThreads = std::thread::hardware_concurrency();
    const auto nThreads = 4;
    threads.reserve(nThreads);
    for (unsigned int i = 0; i < nThreads; ++i) {
        threads.emplace_back([&io_context]() { io_context.run(); });
    }
    for (auto &th : threads) {
        th.join();
    }

    std::cout << std::this_thread::get_id() << " Server stopped." << std::endl;    

    return 0;
}

