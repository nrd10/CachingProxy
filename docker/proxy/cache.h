#ifndef _CACHE__H 
#define _CACHE__H
#include<string>
#include<unordered_map>
//#include"addrkeeper.h"
#include"socketkeeper.h"
#include "httpRequest.h"
#include "httpResponse.h"
#include <sys/select.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
class cache{
  //  std::vector<std::string> cache_directives;
 private:
  std::unordered_map<std::string, httpResponse> database;
  std::string read_header(socketkeeper & sock);
  //void visit_server(httpRequest & whole_message);
  int listen_recv_send(int fd, fd_set & set, socketkeeper & to_send,std::string &&who_recv, std::string&& who_send);
  void connect_blind_transmit(socketkeeper & client_sock, socketkeeper & server_sock);
  void generate_revalid_request(httpRequest & info, std::unordered_map<std::string, httpResponse>::iterator to_valid);
  
 public:
  size_t size();
  httpRequest recv_request(socketkeeper & sock);
  httpResponse recv_response(socketkeeper & sock);
  void print_cache_commands();
  void deal_with_connect(socketkeeper & server_sock, socketkeeper & client_sock,httpRequest & request);
  //httpRequest parse_request();
  //httpResponse parse_respose();
  bool  check_cache_response(httpResponse & response, httpRequest & request,int uid);
  void cache_directive_split(std::string mystring, std::string delim);
  void store_response(httpResponse & response, httpRequest& request);
  std::unordered_map<std::string, httpResponse> & get_cache();
  bool need_check_cache(httpRequest & request);
  std::unordered_map<std::string, httpResponse>::iterator in_cache(httpRequest & request);
  const std::unordered_map<std::string, httpResponse>::const_iterator get_db_end();
  httpResponse revalidate(socketkeeper & server_sock, httpRequest & info, std::unordered_map<std::string, httpResponse>::iterator to_valid);
  void write_log(httpRequest & request,httpResponse & response,int uid);
  
};

time_t generate_request_time();
void send_400_bad_request(socketkeeper & client_sock);
void send_502_bad_gateway(socketkeeper & client_sock);
#endif 
