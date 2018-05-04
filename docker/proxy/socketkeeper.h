#ifndef _SOCKETKEEPER__H
#define _SOCKETKEEPER__H
#include <sys/socket.h>
#include <sys/types.h>
#include "addrkeeper.h"
class socketkeeper{
  int sock_fd;
  int yes;
public:
  socketkeeper() = default;
  socketkeeper(int toCreate);
  void bind_sock(addrkeeper & addr_auto);
  void make_sock_listen(int BACKLOG);
  int accept_connection();
  void connect_server(addrkeeper& addr_auto);
  int recv_message(void * info, size_t size, int flags);
  int send_message(void * info, size_t size);
  void create_fd();
  //void set_socket_opt();
  int get_socket_fd() const;
  void set_sock_opt();
  ~socketkeeper();
};
#endif
