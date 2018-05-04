//
// Created by ethan on 3/2/18.
//

#ifndef PROJECT_LOGGER_H
#define PROJECT_LOGGER_H


#include <fstream>
#include "httpRequest.h"
#include "httpResponse.h"

class logger {
  std::ofstream *file;
public :
  logger() = default;
  void set_file(std::ofstream * file);
  void write_request_first(const httpRequest & request, int uid);
  //void write_response(const httpResponse & response, int uid);
  void write_request_cache(const httpRequest & request, int uid, int condition, std::string && reason);
  void write_requ_response(const httpRequest & request, int uid);
  void write_recv_response(const httpResponse & response,const httpRequest &request, int uid);
  void write_cache_response(const httpResponse & response, int uid, int condition,const char * nothing);
  void write_resp_respond(const httpResponse & response, int uid);
  void write_tunnel(int uid);
};

extern logger logger_instance;
#endif //PROJECT_LOGGER_H
