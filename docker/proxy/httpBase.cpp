#include "httpBase.h"
#include <iostream>
#include <sstream>
#include <cassert>
#include "exception.h"
#include <sys/types.h>
#include "widget.h"
//httpBase::httpBase():token(),meta(){}
httpBase::httpBase(std::string & rq):header(rq),payload(),whole_message(),token(),meta(), cache_directives(){}
void httpBase::parse_helper(std::string && line){ 
  size_t pos = line.find_first_of(32);
  //32 is the ascii value of space character
  //what if the first line is malformatted data ?
  meta.push_back(line.substr(0,pos));
  if(pos != std::string::npos){
    parse_helper(line.substr(pos+1));
  }
}
    
void httpBase::parse_first_line(std::string & line){
  if(line[line.size() - 1] != '\r'){
    parse_helper(std::move(line));
  }
  else{
    parse_helper(line.substr(0,line.size() -1));
  }
}

size_t httpBase::get_whole_size() {
  return whole_message.size();
}

void httpBase::get_whole_message(){
  for (auto item : whole_message){
    std::cout<<item;
  }
  std::cout<<std::endl;
}

void httpBase::generate_whole(){
  std::vector<char> temp;
  temp.insert(temp.begin(),header.begin(),header.end());
  temp.insert(temp.end(),payload.begin(),payload.end());
  whole_message = temp;
}


char * httpBase::get_whole_first_addr(){
  return &whole_message[0];
}


void httpBase::parse_and_split(std::string && line){
  if (line.empty()){ //EOF
    return;
  }
  size_t pos = line.find_first_of(58);
  //58 is the ascii value of colon character
  //what if colon doesn't exist ?
  if(pos == std::string::npos){
    token.clear(); //none of the data is valid
    meta.clear();
    std::cerr<<"malformed whole_message"<<std::endl;
    throw malformed();//as long as malformed whole_message exists, throw exception
  }
  std::string key = line.substr(0, pos);
  std::string value;
  if (line[pos + 1] == ' '){
    value = line.substr(pos +2);
  }
  else{
    value = line.substr(pos+1);
  }
  token[key] = value;
}


void httpBase::print_token(){
  for (auto it:token){
    std::cout<<"K- "<<it.first<<":"<<"V- "<<it.second<<std::endl;
  }
}
void httpBase::print_meta(){
  for (auto item:meta){
    std::cout<<item<<std::endl;
  }
}
void httpBase::populate_token(){
  std::stringstream ss;
  ss << header;
  std::string line;
  getline(ss, line, '\n'); //first line
  //std::cout<<"first line: "<<line<<std::endl;
  parse_first_line(line);
  while(!ss.eof()){
    getline(ss, line, '\n');
    if(line.empty()){
      return;
    }
    if(line[line.size() -1] != '\r'){
      parse_and_split(std::move(line));
    }
    else{
      parse_and_split(line.substr(0,line.size()-1));
    }
  }
}
void httpBase::append(char *to_append, size_t size){
  for (size_t i = 0; i < size; ++i){
    payload.push_back(to_append[i]);
  }
}
void httpBase::recv_one_zero(socketkeeper & sock){
  while(1){
    char buffer[255]={0};
    if(sock.recv_message(buffer, 255, 0) == 0){
      break;
    }
    append(buffer,255);
  }
}

std::unordered_map<std::string, std::string> & httpBase::get_token(){
  return token;
}
 


std::string httpBase::how_to_read(){
  if(token.find("Transfer-Encoding") != token.end()){
    return "chunk";
  }
  else if(token.find("Content-Length") != token.end()){
    return "length";
  }
  else{
    return "none";
  }
}

void httpBase::recv_content_length(socketkeeper & sock){
  int length = -1;
  length = std::stoi(token["Content-Length"]);
  if(length == 0){
    return;
  }
  else if(length == -1){
    throw malformed();
  }
  charStr buffer(length+1);
  sock.recv_message(buffer, length, MSG_WAITALL);
  append(buffer,length);
}

void httpBase::general_append(std::vector<char> & src, char * to_append, size_t size){
  for(size_t i = 0; i < size; ++i){
    src.push_back(to_append[i]);
  }
}

int httpBase::find_chunk_length(std::vector<char> & chunk_size){
  std::string temp(chunk_size.begin(),chunk_size.end());
  size_t sp_pos = temp.find_first_of(32); //32 is the ascii value of space character
  std::stringstream ss;
  int dec_value = -1;
  if(sp_pos == std::string::npos){ //no extension
    size_t rc_pos = temp.find_first_of('\r');

    if(rc_pos == std::string::npos){ //no RC
      ss<<temp;
    }
    else{ //RC exists
      assert(rc_pos==(temp.size() -2));
      ss<<temp.substr(0,temp.size() -1);
    }
    ss>>std::hex>>dec_value; //what if this is malformed??
    if(dec_value == -1){
      throw malformed();
    }
    return dec_value;
  }
  else{ //extension exists
    ss<<temp.substr(0,sp_pos);
    ss>>std::hex>>dec_value;
    if(dec_value == -1){
      throw malformed();
    }
    return dec_value;
  }
}
void httpBase::recv_chunked_encoding(socketkeeper & sock){
  //we have already read the line feed, next line is the first chunk's size
  std::vector<char> temp;
  while(1){
    //read until \r\n
    char buffer[2] = {0};
    sock.recv_message(buffer,1,0);
    general_append(temp,buffer,1); //normal function
    append(buffer,1); //httpResponse method
    if(!strcmp(buffer,"\n")){
      int length = find_chunk_length(temp);
      if(length == 0){
        return;
      }
      else{
        charStr fancy_buffer(length+1+1+1); //receive \r\n
        sock.recv_message(fancy_buffer,length+1+1,MSG_WAITALL);
        append(fancy_buffer,length+1+1);
        temp.clear();
      }
    }
  }
}


void httpBase::recv_one_one(socketkeeper & sock){
  if(how_to_read() == "chunk"){
    recv_chunked_encoding(sock);
  }
  else if(how_to_read() == "length"){
    recv_content_length(sock);
  }
  else{
    std::cerr<<"Bad Header!"<<std::endl;
    throw badHeader();
  }
}


std::string httpBase::get_header(){
  return header;
}

size_t httpBase::get_header_size(){
  return header.size();
}

std::string httpBase::get_meta() {
  return meta[1];
}

std::vector<std::string> httpBase::get_directives() {
  return cache_directives;
}

void httpBase::cache_directive_split(std::string mystring, std::string delim){
 size_t pos = 0;
 while ((pos = mystring.find(delim)) != std::string::npos) {
   std::string tok = mystring.substr(0, pos);
   std::locale loc;
   std::cout<<"My cache directive to add is:"<<tok<<std::endl;
   cache_directives.push_back(tok);
   mystring.erase(0, pos + delim.length()+1);
 }
 std::vector<std::string> A = get_directives();
 std::cout<<"A is:"<<mystring<<std::endl;
 cache_directives.push_back(mystring);
 //A.push_back(mystring);
 
}

void httpBase::print_cache_commands(){
  std::vector<std::string> A = get_directives();
  for (auto i = A.begin(); i != A.end(); ++i) {
    std::cout<<"My cache directive is:"<<*i<<std::endl;
  }

}


void httpBase::make_cache_vector() {
  cache_directives.clear();
  std::unordered_map<std::string, std::string> mytoken = get_token();
  std::unordered_map<std::string, std::string>::const_iterator token_finder = mytoken.find("Cache-Control");
    if (token_finder == mytoken.end() ) {
      std::cout<<"No Cache-Control token found!"<<std::endl;
    }
  else {
      std::cout<<"The Cache-Control value is: "<<token_finder->second<<std::endl;
      //add directives to vector of cache directives with HttpResponse object
      cache_directive_split(token_finder->second, ",");

      //print out cache commands
      print_cache_commands();
     
    } 
}

void httpBase::generate_new_header(const std::string &to_replace){
  size_t pos;
  if((pos = header.find("\r\n\r\n")) != std::string::npos){
    std::string temp("\r\n");
    temp.append(to_replace);
    header.replace(pos, 4, temp);
    header.append("\r\n\r\n");
  }
  else if((pos = header.find("\n\n")) != std::string::npos){
    std::string temp("\n");
    temp.append(to_replace);
    header.replace(pos,2,temp);
    header.append("\n\n");
  }
}


bool httpBase::check_cache_control(const char * name){
  std::vector<std::string> A = get_directives();
  //check response for these two headers
  for (auto i = A.begin(); i != A.end(); ++i) {
    std::cout<<"My cache directive is:"<<*i<<std::endl;
    if ((*i).compare(name)==0) {
      std::cout<<name<<"!"<<std::endl;
      return true; //this means it has no_cache
    }
  }
  return false;
}


void httpBase::add_token(const char * key, std::string value){
  token[std::string(key)] = value;
}

std::string httpBase::get_whole_meta() const{
  std::string temp = meta[0];
  for (size_t i = 1; i < meta.size(); i++) {
    temp += " " + meta[i];
  }
  return temp;
}


  
