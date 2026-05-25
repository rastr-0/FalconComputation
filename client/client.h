#ifndef H_CLIENT
#define H_CLIENT
#include <sys/types.h>

class Client {
private:
  char buffer[1024];
  int sock;

public:
  void make_connection(const char* address, const short port);
  // main loop of handling protocol messages
  void run();
  ssize_t receive_data();
  ssize_t send_data(const void* data, size_t size);
};

#endif // !H_CLIENT

