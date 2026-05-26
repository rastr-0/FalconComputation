#include "server.h"
#include "ProtocolHandler.h"

#include <asm-generic/socket.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdexcept>
#include <iostream>

struct server_exception : std::runtime_error {
  using std::runtime_error::runtime_error;
};

ssize_t Server::receive_data() {
  ssize_t bytes = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
  if (bytes > 0)
    buffer[bytes] = '\0';
  return bytes;
}

ssize_t Server::send_data(const void* data, size_t size) {
  return send(client_sock, data, size, 0);
}

void Server::make_connection(const char* address, short port) {
  listening_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (listening_sock == -1)
    throw server_exception("Cannot create a listening socket");

  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = inet_addr(address);

  int opt = 1;
  setsockopt(listening_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  if (bind(listening_sock, (sockaddr*)&addr, sizeof(addr)) == -1)
    throw server_exception("Cannot bind socket");
  if (listen(listening_sock, 10) == -1)
    throw server_exception("Cannot listen on socket");

  std::cout << "Listening on " << address << ":" << port << "\n";
}

void Server::run(CommandRegistry& registry) {
  while (true) {
    sockaddr_in client_addr{};
    socklen_t client_len = sizeof(client_addr);
    client_sock = accept(listening_sock, (sockaddr*)&client_addr, &client_len);
    if (client_sock < 0)
      continue;

    ProtocolHandler handler(registry); // creating ProtocolHandler whose task is to manage CommandRegistry and Session
      // it's the only function is handle that takes buffer and returns ready response for client 

    while (true) {
      ssize_t bytes = receive_data();
      if (bytes == 0) {
        std::cout << "Client disconnected\n";
        close(client_sock);
        break;
      }
      if (bytes < 0)
        throw server_exception("recv failed");

      std::string response = handler.handle(std::string(buffer));
      send_data(response.c_str(), response.size());

      if (handler.closed()) {
        close(client_sock);
        break;
      }
    }
  }
  close(listening_sock);
}

