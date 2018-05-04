#include "addrkeeper.h"
#include "exception.h"
#include "widget.h"
#include <cstring>
#include <iostream>

addrkeeper::addrkeeper():pinfo(NULL,freeaddrinfo){}
addrkeeper::addrkeeper(std::string * dest, std::string port) try: pinfo(NULL,freeaddrinfo){
  struct addrinfo * res;
  struct addrinfo hints;
  int status;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET; //IPV4 protocol
  hints.ai_socktype = SOCK_STREAM; //TCP
  charStr port_num;
  charStr hostname;
  port_num = &port;
  if (dest == NULL){ //we are creating our own
    hints.ai_flags = AI_PASSIVE;
  }
  else{
    hostname = dest;
  }
  if((status = getaddrinfo(hostname, port_num, &hints, &res)) != 0){
    if(dest == NULL){
      std::cerr<<"Error: can't get proxy's address information: "<<gai_strerror(status)<<std::endl;
    }
    else{
      std::cerr<<"Error: can't get server's address information: "<<gai_strerror(status)<<std::endl;
    }
    throw getaddrInfoError();
  }
  pinfo.reset(res);
								  }catch(getaddrInfoError & e){}


const struct addrinfo * addrkeeper::get_info() const{
  return pinfo.get(); //return raw resource
}

struct addrinfo* addrkeeper::operator ->(){
  return pinfo.get();
}
    
int addrkeeper::create_socket(){
  return socket(pinfo->ai_family, pinfo->ai_socktype, pinfo->ai_protocol);
}

addrkeeper & addrkeeper::operator=(addrkeeper & rhs){
  if(this != &rhs){
    pinfo = std::move(rhs.pinfo);
  }
  return *this;
}
