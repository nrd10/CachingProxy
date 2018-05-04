#include "httpResponse.h"
#include "httpRequest.h"
#include <sstream>
#include <assert.h>
#include <cstring>
#include "widget.h"
#include "exception.h"
#include <iostream>
#include <string>
#include <locale>
#include "logger.h"
httpResponse::httpResponse(std::string && head):httpBase(head){}

int httpResponse::get_protocol() {
  if(meta[0] == "HTTP/1.0"){
    return 0;
  }
  else if(meta[0] == "HTTP/1.1"){
    return 1;
  }
  else{
    return -1;
  }
}

std::string httpResponse::get_status_code(){
  return meta[1];
}


bool httpResponse::check_cacheage(int uid) {
  std::vector<std::string> C = get_directives();
  //loop through cache directives and check for age
  //fields
  for (auto i = C.begin(); i != C.end(); ++i) {
    if ((*i).find("max-age") != std::string::npos) {
      std::cout<<"Max age found!"<<std::endl;
      return true;
    }
    if ((*i).find("s-maxage") != std::string::npos) {
      std::cout<<"S-maxage found!"<<std::endl;
      return true;
    }
  }
  //loop through meta field to check for Expiration header
  std::unordered_map<std::string, std::string> mytoken = get_token();

  std::unordered_map<std::string, std::string>::const_iterator auth = mytoken.find("Expires");
  if (auth != token.end()) {
    std::cout<<"Expiration found!"<<std::endl;
    return true;
  }
  //none of the above conditions are satisfied -->return false
  logger_instance.write_cache_response(*this,uid,1," Respones doesn't have any age deduction field");
  return false;
}


//checks that it does not contain directives that say you can't cache
bool httpResponse::check_nocache(httpRequest &myrequest,int uid) {
  std::vector<std::string> A = get_directives();
  //check response for these two headers
  for (auto i = A.begin(); i != A.end(); ++i) {
    std::cout<<"My cache directive is:"<<*i<<std::endl;

    //private cache directive
    if ((*i).compare("private")==0) {
      std::cout<<"Private!"<<std::endl;
      logger_instance.write_cache_response(*this,uid,1," response has private header");
      return false;
    }
    //no-store directive
    if ((*i).compare("no-store")==0) {
      std::cout<<"No store!"<<std::endl;
      logger_instance.write_cache_response(*this,uid,1," response has no-store header");
      return false;
    }
  }

  //check request for no-store directive
  std::vector<std::string> B = myrequest.get_directives();
  for (auto j = B.begin(); j != B.end(); ++j) {
    if ((*j).compare("no-store")==0) {
      logger_instance.write_cache_response(*this,uid,1," request has no-store header");
      std::cout<<"Request no store!"<<std::endl;
      return false;
    }
  }
  
  //check for Authentication header
  //only cacheable with this header if
  //s-maxage, max-age or must-revalidate
  std::unordered_map<std::string, std::string> mytoken = myrequest.get_token() ;

  std::unordered_map<std::string, std::string>::const_iterator auth = mytoken.find("Authorization");
  if (auth != token.end()) {
    std::cout<<"Authorization here: check for s-maxage, max-age, reval!"<<std::endl;

    //loop through cache directives to check for these directives
    for (auto i = A.begin(); i != A.end(); ++i) {
      if (((*i).find("s-maxage") != std::string::npos)||((*i).find("max-age") != std::string::npos) || ((*i).find("must-revalidate") != std::string::npos)) {

        //if it has these directives with AUTH header: return true
        return true;
      }
    }
    //if the for loop ended that means it has the AUTH header but not the above
    //directives --> boolean is FALSE
    logger_instance.write_cache_response(*this,uid,1," request has Authorization field but response doesn't have fields for age deduction");
    return false;
  }
  // if it didn't return before that means it has none of the above conditions --> return true
  return true;
}

std::string httpResponse::auto_end_sequence(const char * signal){
  std::string name;
  if(strcmp(signal,"\r")==0){
    return "\r\n";
  }
  else if (strcmp(signal, "\n")==0){
    return "\n";
  }
  return name;
}
void httpResponse::replace_header(const char * signal){
  std::string ans;
  auto mit = meta.begin();
  while(mit != (meta.end() - 1)){
    ans.append(*mit);
    ans.append(" ");
    ++mit;
  }
  ans.append(*mit);
  ans.append(auto_end_sequence(signal));
  auto tit = token.begin();
  while(tit != token.end()){
    ans.append(tit->first);
    ans.append(": ");
    ans.append(tit->second);
    ans.append(auto_end_sequence(signal));
    ++tit;
  }
  ans.append(auto_end_sequence(signal));
  header = ans;
}



void httpResponse::update_header(httpResponse & src){
  //this may lead to problem

  for (auto item : src.token){
    token[item.first] = item.second;
  }
  if(header.find("\r\n\r\n") != std::string::npos){
    replace_header("\r"); //using new token
  }
  else{
    replace_header("\n");
  }
  make_cache_vector();
}

/*not sure*/
/*void httpResponse::update_age(){
  current_age = initial_age + time(NULL) - response_time;
  }*/



bool httpResponse::is_expired(int uid,httpRequest & request){
  time_t temp =  time(nullptr);
  struct tm * ptm; 
  ptm = gmtime(&temp); //how the hell are you supposed to free this? No one will help you free that
  time_t now = mktime(ptm);
  double current = initial_age + now - response_time;
  if(current >= max_age){
    logger_instance.write_request_cache(request,uid,2,when_expired());
    return true;
  }
  else{
    return false;
  }
}

bool httpResponse::need_revalidate(httpRequest &to_check, int uid){
  if(to_check.check_cache_control("no-cache")||
     check_cache_control("must-revalidate")){
    logger_instance.write_request_cache(to_check,uid,3,std::string());
    return true;
  }
  else{
    return false;
  }
}

bool httpResponse::empty(){
  return token.empty() && header.empty() && payload.empty() && meta.empty() && cache_directives.empty();
}

void httpResponse::calculate_initial_age(time_t request_time){
  /*you are going to calculate the initial_age
   *of the response. The initial_age of a response
   *is the time duration from when it was created by
   *the original server to when it is received by
   *your cache proxy. You will find RFC7234 useful
   *to help you calculate this:
   !*apparent_age = max(0, response_time - date_value); from response token
   !*response_delay = response_time - request_time;
   !*corrected_age_value = age_value + response_delay;
   *These are combined as
   *corrected_initial_age = max(apparent_age, correcte
   *d_age_value);
   *Note that response_time is a field stored inside
   *httpResponse object
   *Your function should receive a request_time
   *parameter and then return a calculated initial_age
   *, and store it into initial_age field
   */
  struct tm data_value_tm;
  memset(&data_value_tm, 0, sizeof(struct tm));
  strptime(token["Date"].c_str(), "%a, %d %b %Y %H:%M:%S %Z", &data_value_tm);
  time_t data_value = mktime(&data_value_tm);

  double apparent_age = std::max(0.0, difftime(response_time, data_value));
  double response_delay = difftime(response_time, request_time);
  double age_value;
  auto tit = token.find("Age");
  if(tit != token.end()){
    std::string age_value_string = token["Age"];
    std::stringstream ss(age_value_string);
    ss >> age_value;
  }
  else{
    age_value = 0;
  }
  double corrected_age_value = age_value + response_delay;
  initial_age = std::max(apparent_age, corrected_age_value);
}

void httpResponse::update_max_age(){
  /*you are going to take use of
   *expires/max-age/smax-age/
   *to calculate the max-age
   *you will need to store the result into
   *the max-age field of httpResponse object
   *the priority is smax-age > expires > max-age
   *You will need to translate time string stored in
   *these three fields into time_t field, which is
   *just a signed integer. You MAY find methods in
   *c++ ctime library useful
   *NOTE: if there is not validator to tell the
   *max-age, you SHOULD populate that field with
   *-1
   */
  double max_age_double = 0;
  bool found_max_age = false;
  for (size_t i = 0; i < cache_directives.size(); i++) {
    std::string temp = cache_directives[i];
    std::size_t found = temp.find("s-maxage");
    if (found!=std::string::npos) {
      std::string smax_age = temp.substr(found + 9);
      max_age = strtol(smax_age.c_str(), nullptr, 10);
      return;
    }
    found = temp.find("max-age");
    if (found!=std::string::npos) {
      std::string smax_age = temp.substr(found + 8);
      max_age_double = strtol(smax_age.c_str(), nullptr, 10);
      found_max_age = true;
      break;
    }
  }
  auto search = token.find("Expires");
  if(search != token.end()) {
    struct tm expires_tm;
    memset(&expires_tm, 0, sizeof(struct tm));
    strptime(token["Expires"].c_str(), "%a, %d %b %Y %H:%M:%S %Z", &expires_tm);
    time_t expires = mktime(&expires_tm);
    
    struct tm data_value_tm;
    memset(&data_value_tm, 0, sizeof(struct tm));
    strptime(token["Date"].c_str(), "%a, %d %b %Y %H:%M:%S %Z", &data_value_tm);
    time_t data_value = mktime(&data_value_tm);
    //time_t current;
    //time(&current);
    //struct tm * gm_t = gmtime(&current);
    //current = mktime(gm_t);
    max_age = difftime(expires, data_value);
    return;
  }
  if (found_max_age) {
    max_age = max_age_double;
  } else {
    max_age = -1;
  }
}

void httpResponse::generate_response_time(){
  time_t temp =  time(nullptr);
  struct tm * ptm; 
  ptm = gmtime(&temp); //how the hell are you supposed to free this? No one will help you free that
  response_time = mktime(ptm);
}

std::string httpResponse::when_expired(){
  struct tm data_value_tm;
  memset(&data_value_tm, 0, sizeof(struct tm));
  strptime(token["Date"].c_str(), "%a, %d %b %Y %H:%M:%S %Z", &data_value_tm);
  time_t data_value = mktime(&data_value_tm);
  time_t expired_time = data_value + max_age;
  struct tm * ex_t = gmtime(&expired_time);
  char ex_time[255]={0};
  strftime(ex_time, 254,"%a, %d %b %Y %H:%M:%S %Z", ex_t);
  return std::string(ex_time);
}

bool httpResponse::find_age(){
  for(auto item : cache_directives){
    if(item.find("max-age")||
       item.find("s-maxage")||
       item.find("must-revalidate")){
      return true;
    }
  }
  return false;
}

bool httpResponse::has_valid_control(){
  bool has_expire = (token.find("Expires") != token.end());
  bool at_least_one_age = find_age();
  return (has_expire || at_least_one_age);
}

bool httpResponse::has_must_revalid(){
  for(auto item: cache_directives){
    if((item).find("must-revalidate")){
      return true;
    }
  }
  return false;
}
