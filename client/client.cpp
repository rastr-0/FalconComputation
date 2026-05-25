#include "client.h"

#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdexcept>
#include <iostream>

struct client_exception : std::runtime_error { // inheriting runtime_error
  using std::runtime_error::runtime_error;
};


ssize_t Client::receive_data() {
  ssize_t bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
  if (bytes > 0) {
    buffer[bytes] = '\0'; // termination of string
  }
  return bytes;
}

ssize_t Client::send_data(const void* data, size_t size) {
  return send(sock, data, size, 0);
}

void Client::make_connection(const char* address, const short port) {
  this->sock = socket(AF_INET, SOCK_STREAM, 0);
  if (this->sock == -1)
    throw client_exception("Cannot create a listening socket");
  
  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = inet_addr(address);

  if (connect(this->sock, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    throw client_exception("Cannot connect to a given address:port");
  
  std::cout << "Successfully connected to: " << address << ":" << port << std::endl;
}

void Client::run() {
  /*
  `run` method does following things:
    1) reads from socket
    2) passes data to protocol parser
    3) reacts
  */
  std::cout << "The server is now running and ready!\n";
  std::string line;

  while(true) {
    std::cout << "> ";
    if (!std::getline(std::cin, line))
      break;

    if (line == "quit" || line == "exit")
      break;

    line += "\n";
    send_data(line.c_str(), line.size());

    ssize_t bytes = receive_data();
    
    if (bytes == 0) {
      std::cout << "Server disconnected\n";
      close(sock);
      break;
    }
    if (bytes < 0)
      throw client_exception("Failed getting data from server");
    
    std::cout << buffer << "\n";
  }

  close(sock);
}


int main() {
  try {
    Client client;
    client.make_connection("127.0.0.1", 5678);
    client.run();
  } catch (const client_exception& e) {
    throw std::runtime_error("Client error: " + std::string(e.what()));
  }
}
