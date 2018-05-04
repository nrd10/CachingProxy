#ifndef _HTTPREQUEST__H
#define _HTTPREQUEST__H
#include "httpBase.h"
class httpRequest:public httpBase{
  //first is method
  //second is URI
  //third is protocol version
  time_t request_time;
 public:
  std::string get_target_domain();
  std::string get_target_domain() const;
  std::string get_target_port_num();
  httpRequest() = default;
  explicit httpRequest(std::string & rq);
  //void manipulate_request();
  std::string get_method();
  std::string get_protocol();
  int get_protocol_int();
  std::string get_uri();
  std::string get_time_string() const;
  bool has_auth();
  
};  
#endif  
