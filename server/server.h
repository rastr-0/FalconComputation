#ifndef H_SERVER
#define H_SERVER

#include "CommandRegistry.h"
#include <sys/types.h>

class Server {
  char buffer[1024];
  int listening_sock;
  int client_sock;
public:
  void make_connection(const char* address, short port);
  void run(CommandRegistry& registry);
  ssize_t receive_data();
  ssize_t send_data(const void* data, size_t size);
};

#endif // H_SERVER
