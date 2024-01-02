#include <iostream>
#include <string>
#include <boost/asio.hpp>

using namespace boost::asio;
using ip::tcp;

class HttpServer {
public:
    HttpServer(io_service& io_service, int port) : acceptor(io_service, tcp::endpoint(tcp::v4(), port)), socket(io_service) {
        startAccept();
    }

private:
    void startAccept() {
        acceptor.async_accept(socket, [this](const boost::system::error_code& error) {
            if (!error) {
                handleRequest();
            }

            startAccept();
        });
    }

    void handleRequest() {
        boost::system::error_code error;
        boost::asio::streambuf request;
        boost::asio::read_until(socket, request, "\r\n\r\n", error);

        if (!error) {
            std::istream request_stream(&request);
            std::string request_line;
            getline(request_stream, request_line);
            std::cout << "Request: " << request_line << std::endl;

            // Send a simple response
            std::string response = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, World!";
            boost::asio::write(socket, boost::asio::buffer(response));
        }

        // Close the connection
        boost::system::error_code ignored_error;
        socket.close(ignored_error);
    }

    tcp::acceptor acceptor;
    tcp::socket socket;
};

int main() {
    try {
        boost::asio::io_service io_service;
        HttpServer server(io_service, 8080);
        io_service.run();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
