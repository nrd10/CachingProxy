#include "cache.h"
#include<cstring>
#include <iostream>
#include "exception.h"
#include "widget.h"
#include <assert.h>
#include <errno.h>
#include <sys/types.h>
#include <assert.h>
#include <sstream>
#include <string>
#include <unordered_map>
#include "logger.h"
#include <utility>
//we only deal with \n\n and \r\n\r\n
std::string cache::read_header(socketkeeper & sock){
  std::string rq;
  while(1){
    char buffer[2]={0};
    //std::cout<<"receiving head!";
    int bytes = sock.recv_message(buffer, 1, 0);
    if(bytes == 0){
      std::cerr<<"Close Connection!"<<std::endl;
      throw closeConnection();
    }
    
    if(!strncmp(buffer,"\n",1)){
      rq.append(buffer);
      char check_buffer[2]={0};
      sock.recv_message(check_buffer,1,0);
      rq.append(check_buffer);
      if(!strncmp(check_buffer,"\n",1)){
	break;
      }
      else{
	continue;
      }
    }
    if(!strncmp(buffer,"\r",1)){
      rq.append(buffer);
      char check_buffer[4]={0};
      int bytes = sock.recv_message(check_buffer,3,0);
      assert(bytes == 3); //if assert fails that will cause problem
      rq.append(check_buffer);
      if(!strncmp(check_buffer,"\n\r\n",3)){
	break;
      }
      else{
	continue;
      }
    }
    rq.append(buffer);
  }
  return rq;
}

size_t cache::size(){
  return database.size();
}
httpRequest cache::recv_request(socketkeeper & sock){
  std::string rq;
  try{
    rq = read_header(sock);
  }
  catch(recvError & e){
    send_400_bad_request(sock);
    throw;
  }
  httpRequest request(rq);
  //you will need to populate payload if you are dealing with POST
  try{
    request.populate_token();
  }
  catch (malformed & e){
    send_400_bad_request(sock);
    throw;
  }
  //std::cout<<"HEAD SIZE IS: "<<request.get_header_size()<<std::endl;
  //std::cout<<"HEAD IS: "<<request.get_header()<<std::endl;
  if(request.get_method() == "POST") {
    int protocol = request.get_protocol_int();
    if (protocol == 0) {
      request.recv_one_zero(sock);
    }
    else if (protocol == 1) {
      request.recv_one_one(sock);
    }
    else {
      std::cerr << "Error: bad protocol" << std::endl;
      throw badProtocol();
    }
  }
  request.generate_whole();
  return request;
}


httpResponse cache:: recv_response(socketkeeper & sock){
  std::string rq;
  try{
    rq = read_header(sock);
  }
  catch(recvError & e){
    send_502_bad_gateway(sock);
    throw;
  }
  httpResponse response(std::move(rq));
  //std::cout<<"Response Head size is: "<<response.get_header_size()<<std::endl;
  //std::cout<<"Head is: "<<response.get_header()<<std::endl;
  try{
    response.populate_token();
  }
  catch(malformed & e){
    send_502_bad_gateway(sock);
    throw;
  }
  //std::cout<<"whole_message is :\n";
  //whole_message.print_meta();
  //whole_message.print_token();
  if((response.get_status_code() != "304")&&(response.get_status_code() != "204")){
    int protocol = response.get_protocol();
    if(protocol == 0){
      response.recv_one_zero(sock);
    }
    else if(protocol == 1){
      response.recv_one_one(sock);
    }
    else{
      std::cerr<<"Error: bad protocol"<<std::endl;
      throw badProtocol();
    }
  }
  response.generate_whole();
  response.make_cache_vector();
  std::cout<<"whole response is: "<<std::endl;
  response.get_whole_message();
  response.generate_response_time();
  response.update_max_age();
  return response;
}

void print_buffer(char * buffer, size_t size){
  for (size_t i = 0; i < size; ++i){
    std::cout<<buffer[i];
  }
  std::cout<<std::endl;
}
  
int cache::listen_recv_send(int fd, fd_set & set, socketkeeper & to_send,std::string&& who_recv,std::string&& who_send){
  if(FD_ISSET(fd,&set)){
      char buffer[10001]={0};
      int bytes;
      bytes = recv(fd,buffer,10000,0);
      std::cout<<"CONNECTING. Recieved "<<bytes<<" from "<<who_recv<<"\n";
      //print_buffer(buffer,10000);
      if(bytes == 0 || bytes == -1){
	std::cout<<"When I try to receive, "<<who_recv<<" I was receiving close the connection"<<std::endl;
	return 0;
      }
      /*else if(bytes == -1){
	std::cout<<"Recv Error"<<std::endl;
	perror("");
	throw recvError();
	}
      */
      // std::cout<<"HEY!"<<std::endl;
      int send_bytes = to_send.send_message(buffer,bytes);
      std::cout<<"CONNECTING. Sent "<<send_bytes<<" to "<<who_send<<"\n";
      return 1;
  }
  std::cout<<who_recv<<" is not ready to send data.\n";
  return -1;
}

void cache::connect_blind_transmit(socketkeeper & client_sock, socketkeeper & server_sock){
  int client_fd = client_sock.get_socket_fd();
  int server_fd = server_sock.get_socket_fd();
  int nfds = std::max(client_fd, server_fd) + 1;
  while(1){
    fd_set set;		  
    FD_ZERO(&set);
    FD_SET(server_fd, &set);
    FD_SET(client_fd, &set);
    /*struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 500000;
    */
    std::cout<<"I'm going to select!"<<std::endl;
    select(nfds,&set,NULL,NULL,NULL);
    std::cout<<"select returned!"<<std::endl;
    int side_one;
    int side_two;
    if((side_one =listen_recv_send(client_fd,set,server_sock,std::string("client"),std::string("server")))==0){
      break;
    }
    if((side_two = listen_recv_send(server_fd,set,client_sock,std::string("server"),std::string("client")))==0){
      break;
    }
    /*
    if(side_one == -1 && side_two == -1){
      std::cout<<"Neither side is going to send data to me! I will just break!"<<std::endl;
      break;
    }
    */
    
  }
}
void cache::deal_with_connect(socketkeeper & server_sock, socketkeeper & client_sock, httpRequest & request){
  /*std::string domain = request.get_target_domain();
  //std::cout<<"domain is: "<<domain<<std::endl;
  std::string port_num = request.get_target_port_num();
  //std::cout<<"port number is: "<<port_num<<std::endl;
  addrkeeper server_addr(&domain, port_num);
  socketkeeper server_sock(server_addr.create_socket());
  server_sock.connect_server(server_addr); 
  */
  //normally you should check authentication
  std::string my_response;
  my_response.append(request.get_protocol());
  my_response.append(" 200 OK\r\n\r\n");
  //std::cout<<my_response<<std::endl;
  charStr my_response_str(std::move(my_response));
  client_sock.send_message(my_response_str, my_response_str.size());
  connect_blind_transmit(client_sock, server_sock);  
}



//goes through conditions to check if we can store a cacheable page
bool cache::check_cache_response(httpResponse & response, httpRequest & init_req,int uid) {
  std::cout<<"Inside check_cache_response!"<<std::endl;
  bool myanswer = false;
  //check if it is a GET request and status code is 200 OK
  if ((init_req.get_method() == "GET") && (response.get_status_code() == "200")) {
    std::cout<<"I have a GET 200!"<<std::endl;

    //check if it is cacheable
    myanswer = (response.check_nocache(init_req,uid))&&(response.check_cacheage(uid));
    std::string meaning = "True";
    if (myanswer == 0) {
      meaning = "False";
    }
    std::cout<<"Check Cacheable is:"<<meaning<<std::endl;
  }
  return myanswer;
}
void cache::store_response(httpResponse &response, httpRequest & request) {
  std::cout<<"We can CACHE!"<<std::endl;
  std::cout<<"Placing key:"<<request.get_meta()<<" with response with header:"<<response.get_header()<<std::endl;
  //place into cache!
  std::unordered_map<std::string, httpResponse>& db = get_cache();
  db.insert(std::pair<std::string, httpResponse>(request.get_meta(), response));
  std::cout<<"Cache size is:"<<db.size()<<std::endl;

}
  
std::unordered_map<std::string, httpResponse> & cache::get_cache() {
  return database;
}



void cache::generate_revalid_request(httpRequest & info,
				     std::unordered_map<std::string,httpResponse>::iterator to_valid){
  if((to_valid->second).get_token().find("ETag") != (to_valid->second).get_token().end()){
    //we have an ETag
    info.add_token("If-None-Match",(to_valid->second).get_token()["ETag"]);
    std::string temp("If-None-Match: ");
    temp.append((to_valid->second).get_token()["ETag"]);
    info.generate_new_header(temp);
  }
  if((to_valid->second).get_token().find("Last-Modified") != (to_valid->second).get_token().end()){
    info.add_token("If-Modified-Since",(to_valid->second).get_token()["Last-Modified"]);
    std::string temp("If-Modified-Since: ");
    temp.append((to_valid->second).get_token()["Last-Modified"]);
    info.generate_new_header(temp);
  }
  info.generate_whole();
}


httpResponse cache::revalidate(socketkeeper & server_sock,
			       httpRequest & info,
			       std::unordered_map<std::string,httpResponse>::iterator to_valid){
  generate_revalid_request(info, to_valid);
  server_sock.send_message(info.get_whole_first_addr(), info.get_whole_size());
  time_t request_time = generate_request_time();
  httpResponse ans = recv_response(server_sock);
  ans.calculate_initial_age(request_time);
  return ans;
}



bool cache::need_check_cache(httpRequest & request){
  if(request.get_method() != "GET"){
    return false;
  }
  else if(request.check_cache_control("no-store")){
    return false;
  }
  else{
    return true;
  }
}


//user may modify the object that the iterator
//points at
//so we should use a normal iterator instead of
//a const_iterator
std::unordered_map<std::string, httpResponse>::iterator cache::in_cache(httpRequest & request){
  std::string key = request.get_uri();
  auto db_it = database.find(key);
  return db_it;
}

const std::unordered_map<std::string, httpResponse>::const_iterator cache::get_db_end(){
  const auto db_end = database.cend();
  return db_end;
}
void cache::write_log(httpRequest & request,httpResponse & response,int uid){
  if(response.has_must_revalid() || (request.has_auth() && response.has_valid_control())){
    logger_instance.write_cache_response(response, uid, 3,nullptr);
  }
  else{
    logger_instance.write_cache_response(response,uid,2,response.when_expired().c_str());
  }
}
