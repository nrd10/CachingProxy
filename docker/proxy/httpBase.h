#ifndef _HTTPBASE__H
#define _HTTPBASE__H

#include <unordered_map>
#include <string>
#include <vector>
#include "socketkeeper.h"

class httpBase {
protected:
    std::string header;
    std::vector<char> payload;
    std::vector<char> whole_message;
    std::unordered_map<std::string, std::string> token;
    std::vector<std::string> meta;
    std::vector<std::string> cache_directives;    

    //std::string all_info;
    void parse_first_line(std::string &line);

    void parse_helper(std::string &&line);

    void parse_and_split(std::string &&line);
    void recv_content_length(socketkeeper &sock);
    void recv_chunked_encoding(socketkeeper &sock);
    int find_chunk_length(std::vector<char> &chunk_size);
    std::string how_to_read();
    void append(char *to_append, size_t size);
    
    
public:
    httpBase() = default;

    explicit httpBase(std::string &rq);

       
    void populate_token();

    void print_meta();

    void print_token();

    void generate_whole();

    void get_whole_message();

    //virtual std::string get_protocol() = 0;

    size_t get_whole_size();

    char *get_whole_first_addr();

    char *get_request_first_addr();


    size_t get_header_size();
    std::string get_header();

    void recv_one_zero(socketkeeper &sock);

    void general_append(std::vector<char> &src, char *to_append, size_t size);

    void recv_one_one(socketkeeper &sock);

    std::string get_meta();

    std::vector<std::string> get_directives();

    void cache_directive_split(std::string mystring, std::string delim);

    void print_cache_commands();
  
    void make_cache_vector();

    void generate_new_header(const std::string &to_replace);
    std::unordered_map<std::string,std::string> & get_token();
    void add_token(const char * key, std::string value);
    bool check_cache_control(const char * name);
    std::string get_whole_meta() const;
};

void send_400_bad_request(socketkeeper & client_sock);
void send_502_bad_gateway(socketkeeper & client_sock);

#endif
