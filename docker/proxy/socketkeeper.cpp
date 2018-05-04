#include "socketkeeper.h"
#include "exception.h"
#include <iostream>
#include <unistd.h>
#include <errno.h>
socketkeeper::socketkeeper(int toCreate):sock_fd(toCreate),yes(1){}

void socketkeeper::bind_sock(addrkeeper & addr_auto){
  const struct addrinfo * cur = addr_auto.get_info();
  for(;cur!=NULL;cur=cur->ai_next){
    if(bind(sock_fd,cur->ai_addr, cur->ai_addrlen) != -1){
      return;
    }
  }
  perror("");
  std::cerr<<"binding port 8000 failed"<<std::endl;
  throw bindError();
}
void socketkeeper::make_sock_listen(int BACKLOG){
  if(listen(sock_fd, BACKLOG) == -1){
    std::cerr<<"listening to port 8000 failed"<<std::endl;
    throw listenError();
  }
}
void socketkeeper::set_sock_opt(){
  if(setsockopt(sock_fd, SOL_SOCKET,SO_REUSEADDR,&yes, sizeof(yes)) == -1){
    std::cerr<<"setSockOptError!"<<std::endl;
    throw setSockOptError();
  }
}
int socketkeeper::recv_message(void * info, size_t size, int flags ){
  int bytes;
  /*
  while(1){
    int temp_bytes = recv(sock_fd,info,size,0);
    if(temp_bytes == -1){
      throw recvError();
    }
    else if(
 */      
  if((bytes = recv(sock_fd, info, size, flags)) == -1){
    std::cerr<<"Receive Error!"<<std::endl;
    perror("");
    throw recvError();
  }
  return bytes;
}

int socketkeeper::send_message(void *info, size_t size){
  int bytes;
  if((bytes = send(sock_fd, info, size, 0)) == -1){
    perror("");
    std::cerr<<"Send Error!"<<std::endl;
    
    //throw sendError();
  }
  return bytes;
}

int socketkeeper::get_socket_fd() const{
  return sock_fd;
}

void socketkeeper::connect_server(addrkeeper& addr_auto){
  if (connect(sock_fd, addr_auto->ai_addr, addr_auto->ai_addrlen) == -1){
    std::cerr<<"Connect Error!"<<std::endl;
    throw connectError();
  }
}

int socketkeeper::accept_connection(){
  int new_sock_fd;
  struct sockaddr addr;
  socklen_t addrlen = sizeof(struct sockaddr);
  if((new_sock_fd = accept(sock_fd, &addr, &addrlen)) == -1){
    std::cerr<<"Accept Error!"<<std::endl;
    throw acceptError();
  }
  return new_sock_fd;
}

socketkeeper::~socketkeeper(){
  //shutdown(sock_fd, SHUT_RDWR);
  close(sock_fd);
}
