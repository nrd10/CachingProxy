#include "httpRequest.h"
#include <sstream>
#include "exception.h"

#include <iostream>
httpRequest::httpRequest(std::string &head):httpBase(head){
  time_t rawtime;
  time (&rawtime);
  request_time = rawtime;
}
std::string httpRequest::get_method(){
  if(meta[0] != "CONNECT" &&
     meta[0] != "POST" &&
     meta[0] != "GET"){
    throw badMethod();
  }
  return meta[0];
}

std::string httpRequest::get_protocol(){

  return meta[2];

}

int httpRequest::get_protocol_int(){
  if(meta[2] == "HTTP/1.0"){
    return 0;
  }
  else if(meta[2] == "HTTP/1.1"){
    return 1;
  }
  else{
    return -1;
  }
}

std::string httpRequest::get_uri(){
  return meta[1];
}


std::string httpRequest::get_target_domain() {

  return  static_cast<const httpRequest *>(this)->get_target_domain();
}

std::string httpRequest::get_target_domain() const {
  if(token.find("Host") == token.end()){
    return std::string(); //empty string throw exception
  }
  else{
    std::string Host = token.at("Host");
    size_t pos;
    if((pos = Host.find_first_of(':')) == std::string::npos){ //no colon exist
      return Host;
    }
    else{ //"01234:"
      return Host.substr(0, pos);
    }
  }
}


std::string httpRequest::get_target_port_num() {
  if(token.find("Host") != token.end()){
    std::string Host = token["Host"];
    size_t pos;
    if((pos = Host.find_first_of(':')) != std::string::npos){
      return Host.substr(pos+1); 
    }
  }
  if(meta.size() != 3){
    return std::string(); //empty string throw exception
  }
  else{
    if(meta[2].find("HTTPS") != std::string::npos){
      return std::string("443");
    }
    if(meta[2].find("HTTP") != std::string::npos){
      return std::string("80");
    }
    else{
      return std::string();
    }
  }
}

std::string httpRequest::get_time_string() const {
  return std::string(ctime(&request_time));
}


bool httpRequest::has_auth(){
  if(token.find("Authorization") != token.end()){
    return true;
  }
  else{
    return false;
  }
}
