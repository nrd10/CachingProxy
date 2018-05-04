#include "addrkeeper.h"
#include "cache.h"
#include "exception.h"
#include "widget.h"
#include "logger.h"
#include <thread>
#include <mutex>
#include <unistd.h>
#include <fcntl.h>
//#include <iostream>
logger logger_instance;
std::mutex mtx;
time_t generate_request_time(){
  time_t temp =  time(nullptr);
  struct tm * ptm; 
  ptm = gmtime(&temp); //how the hell are you supposed to free this? No one will help you free that
  return mktime(ptm);
}

void send_400_bad_request(socketkeeper & client_sock){
  std::string error("HTTP/1.1 400 Bad Request\r\nContent-Type:text/html\r\nContent-Length: 15\r\n\r\n400 Bad Request");
  client_sock.send_message(const_cast<char *>(error.c_str()), error.size());
}

void send_502_bad_gateway(socketkeeper & client_sock){
  std::string error("HTTP/1.1 502 Bad Gateway\r\nContent-Type:text/html\r\nContent-Length: 15\r\n\r\n502 Bad Gateway");
  client_sock.send_message(const_cast<char *>(error.c_str()), error.size());
}


void thread_worker(int fd, cache &my_cache, int uid) {
  std::cout << "Hi!I'm a thread!" << std::endl;
  try{
    std::unique_lock<std::mutex> lck(mtx, std::defer_lock);
    socketkeeper client_sock(fd);
    httpResponse response;
    httpRequest request;
    addrkeeper server_addr;
    try{
      request = my_cache.recv_request(client_sock);
    }
    catch (badProtocol & e){
       send_400_bad_request(client_sock);
      return;
    }
    catch (badMethod & e){
      send_400_bad_request(client_sock);
      return;
    }
    catch (recvError &e){
      return;
    }
    
    time_t request_time;
    //bool Need_Update = false;
    //bool sent_message = false;
    request.make_cache_vector();
    request.get_whole_message();
    std::string domain = request.get_target_domain();
    std::string port_num = request.get_target_port_num();
    try{
      addrkeeper server_temp(&domain, port_num);
      server_addr = server_temp;
      logger_instance.write_request_first(request, uid);
    }
    catch(getaddrInfoError & e){
      send_400_bad_request(client_sock);
      return;
    }

    socketkeeper server_sock(server_addr.create_socket());
    try{
      server_sock.connect_server(server_addr);
    }
    catch(connectError & e){
      send_400_bad_request(client_sock);
      return;
    }
    if (request.get_method() == "CONNECT") {
      my_cache.deal_with_connect(server_sock, client_sock, request);
      logger_instance.write_tunnel(uid);
    //std::cout << "connect completed." << std::endl;
    }
  //if within cache don't do anythign
    else {
      //CACHEABLE
      if (my_cache.need_check_cache(request)) {
        std::cout << "INSIDE CACHEABLE check!" << std::endl;
        /*
        std::unordered_map<std::string, httpResponse>::iterator it = Read_cache_fresh(my_cache, request, client_sock, sent_message, Need_Update);
        if (Need_Update) {
          response = Update_Response(my_cache, server_sock, request, client_sock, sent_message, it);
        }
            }
        */
	lck.lock();
        auto db_it = my_cache.in_cache(request);
        if (db_it != my_cache.get_db_end()) {
          //check if expired or need revalidate
          if ((db_it->second).is_expired(uid,request) || (db_it->second).need_revalidate(request,uid)) {
            response = my_cache.revalidate(server_sock, request, db_it); //revalidate
            if (response.get_status_code() == "304") {
              (db_it->second).update_header(response);
              (db_it->second).generate_whole();
              (db_it->second).update_max_age();
              client_sock.send_message((db_it->second).get_whole_first_addr(), (db_it->second).get_whole_size());
              return;
            }
            //it gets a full response,go the last
          } else {
            //it's in the cache and is valid
            //we send back to client directly
	    logger_instance.write_request_cache(request,uid,4,std::string());
            client_sock.send_message((db_it->second).get_whole_first_addr(), (db_it->second).get_whole_size());
            return;
          }
        }
	
	//unlock
	else{
	  logger_instance.write_request_cache(request,uid,1,std::string());	  
	}
	lck.unlock();
      }
      //not in the cache or don't need to check cache
      if (response.empty()) { //we haven't got a full response
        server_sock.send_message(request.get_whole_first_addr(), request.get_whole_size());

        logger_instance.write_requ_response(request, uid);

        request_time = generate_request_time();
        //response
	try{
	  response = my_cache.recv_response(server_sock);
	}
	catch (badProtocol & e){
	  send_502_bad_gateway(client_sock);
	  return;
	}
	catch (recvError &e){
	  return;
	}
	logger_instance.write_recv_response(response, request, uid);

        response.calculate_initial_age(request_time);
        std::cout << "RECEIVING NEW RESPONSE: " << std::endl;
        response.get_whole_message();
      }
      //response.make_cache_vector();
      std::cout << "printing all tokens!" << std::endl;
      response.print_token();
      int send_bytes;
      send_bytes = client_sock.send_message(response.get_whole_first_addr(), response.get_whole_size());
      logger_instance.write_resp_respond(response, uid);

      std::cout << "SENT BYTES: " << send_bytes << std::endl;
      if (!my_cache.need_check_cache(request)) {
        return;
      }
      bool cacheable = my_cache.check_cache_response(response, request,uid);
      if (cacheable) {
        //LOCK WRITE
	
	std::cout<< "MOTHER FUCKER!!!!!!!!"<<std::endl;
	lck.lock();
	my_cache.write_log(request,response,uid);
	std::cout<< "FUCK YOU I AM BACK!!!!"<<std::endl;
        my_cache.store_response(response, request);
	lck.unlock();
        //UNLOCK WRITE
      }
      std::cout << "Size of cache is:" << my_cache.size() << std::endl;
    }
  }
  catch (std::exception &e) {
  }
  std::cout<<"THIS THREAD IS OVER\n"<<std::endl;
}


int main(int argc, char **argv) {

/*Proxy does two things: 
 *1. establish connection to server
 *2.accepts connection from client for HTTP requests
 */
  int fd = open("/dev/null",O_RDWR);
  dup2(fd,0);
  dup2(fd,1);
  dup2(fd,2);
  std::ofstream outfile ("/var/log/erss/proxy.log");
  setuid(65534);
  logger_instance.set_file(&outfile);
  cache my_cache;
  try{
    addrkeeper my_addr(NULL, std::string("12345"));
    socketkeeper my_own_sock(my_addr.create_socket());
    my_own_sock.set_sock_opt();
    my_own_sock.bind_sock(my_addr);
    int uid = 0;
    while (true) {
      my_own_sock.make_sock_listen(1);
      std::cout << "I'am back!" << std::endl;
      int new_fd = my_own_sock.accept_connection();
      std::thread proxy_thread(thread_worker, new_fd, std::ref(my_cache), uid);
      //proxy_thread.join();
      proxy_thread.detach();
      uid++;
    }
  }
  catch(std::exception & e){}
  std::cout<<"MAIN THREAD SHOULDN'T BE ABLE TO EXIT"<<std::endl;
  return (EXIT_SUCCESS);
}

  
  
