#ifndef _ADDRKEEPER__H
#define _ADDRKEEPER__H
#include <memory>
#include <sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string>
class addrkeeper{
  std::unique_ptr<struct addrinfo, void(*)(struct addrinfo *)> pinfo;
 public:
  addrkeeper();
  addrkeeper(std::string * dest,std::string port);
  const struct addrinfo * get_info() const;
  int create_socket();
  struct addrinfo * operator->();
  addrkeeper & operator=(addrkeeper & rhs);
};
#endif
