#ifndef _HTTPRESPONSE__H
#define _HTTPRESPONSE__H

#include "httpBase.h"
#include "socketkeeper.h"
#include "httpRequest.h"
#include <time.h>
class httpResponse : public httpBase {
  time_t initial_age;
  time_t current_age;
  time_t max_age;
  time_t response_time;
  bool find_age();
  //void recv_chunked_encoding(socketkeeper & sock);
  //void recv_content_length(socketkeeper & sock);
  //int find_chunk_length(std::vector<char> & chunk_size);
  //std::string how_to_read();
  //void append(char *to_append, size_t size);
public:
  httpResponse()=default;
  explicit httpResponse(std::string &&head);
  
  int get_protocol();
  std::string get_status_code();
  bool check_nocache(httpRequest & myrequest, int uid);
  bool check_cacheage(int uid);
  void update_header(httpResponse & src);
  //void update_age();
  std::string auto_end_sequence(const char * signal);
  void replace_header(const char * signal);
  bool is_expired(int uid,httpRequest & request);
  bool empty();
  void calculate_initial_age(time_t request_time);
  void update_max_age();
  void generate_response_time();

  bool need_revalidate(httpRequest &to_check, int uid);
  std::string when_expired();
  bool has_valid_control();
  bool has_must_revalid();
};
 

#endif
