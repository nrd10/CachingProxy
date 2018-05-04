//
// Created by ethan on 3/2/18.
//

#include <sstream>
#include <cstring>
#include <iostream>
#include "logger.h"

void logger::set_file(std::ofstream *file) {
  logger::file = file;
}

void logger::write_request_first(const httpRequest &request, int uid) {
  struct addrinfo hints, *infoptr;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET; // AF_INET means IPv4 only addresses
  hints.ai_socktype = SOCK_STREAM;
  int result = getaddrinfo(request.get_target_domain().c_str() , NULL, &hints, &infoptr);
  if (result) {
    std::cerr << "Error: cannot get address info for host" << std::endl;
    return;
    //exit(1); // throw exception?
  }
  char host[256];
  getnameinfo(infoptr->ai_addr, infoptr->ai_addrlen, host, sizeof(host), NULL, 0, NI_NUMERICHOST);
  freeaddrinfo(infoptr);
  std::string ip(host);
  std::stringstream ss;
  ss << uid << ": " << "\"" + request.get_whole_meta() + "\"" << " from " << ip << " @ " << request.get_time_string();
  // lock
  *file << ss.str();
  // unlock
}

//void logger::write_response(const httpResponse &response, int uid) {
//  std::stringstream ss;
//
//}

void logger::write_requ_response(const httpRequest &request, int uid) {
  std::stringstream ss;
  ss << uid << ": Requesting " << "\"" + request.get_whole_meta() + "\""<< " from " << request.get_target_domain();
  // lock
  *file << ss.str() << std::endl;
  // unlock
}

void logger::write_recv_response(const httpResponse &response, const httpRequest &request, int uid) {
  std::stringstream ss;
  ss << uid << ": Received " << "\"" + response.get_whole_meta() + "\""<< " from " << request.get_target_domain();
  // lock
  *file << ss.str() << std::endl;
  // unlock

}

void logger::write_request_cache(const httpRequest &request, int uid, int condition, std::string && reason) {
  std::stringstream ss;
  ss << uid << ": ";
  switch (condition) {
    case 1:
      ss << "not in cache";
      break;
    case 2:
      ss << "in cache, but expired at " << reason; // need to think
      break;
    case 3:
      ss << "in cache, requires validation";
      break;
    case 4:
      ss <<  "in cache, valid";
      break;
    default:
      break;
  }
  // lock
  *file << ss.str() << std::endl;
  // unlock
}



void logger::write_cache_response(const httpResponse &response, int uid, int condition,const char * nothing) {
  std::stringstream ss;
  ss << uid << ": ";
  switch (condition) {
  case 1:
    ss << "not cacheable because" << nothing; //no s-maxage max-age and Expires / no store / private
    break;
  case 2:
    ss << "cached, expires at " << nothing; //
    break;
  case 3:
    ss << "cached, but requires re-validation"; //no cache/revalidate/
    break;
  default:
    break;
  }
  // lock
  *file << ss.str() << std::endl;
  // unlock
}

void logger::write_resp_respond(const httpResponse &response, int uid) {
  std::stringstream ss;
  ss << uid << ": Responding " << response.get_whole_meta();
  // lock
  *file << ss.str() << std::endl;
  // unlock
}

void logger::write_tunnel(int uid) {
  std::stringstream ss;
  ss << uid << ": Tunnel closed";
  // lock
  *file << ss.str() << std::endl;
  // unlock
}




