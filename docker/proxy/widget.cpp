#include "widget.h"
#include "exception.h"
#include <iostream>

charStr::charStr(std::string && toMove):str(){
  char * ans = NULL;
  ans = new char[toMove.size()+1]();
  if(ans == NULL){
    std::cerr<<"construction of object of type charStr failed"<<std::endl;
    throw insufficientMemory();
  }
  strcpy(ans, toMove.c_str());
  str.reset(ans);
  sz = toMove.size();
}

charStr::charStr(int num):str(new char[num]()),sz(num){}

charStr::operator char *(){
  return str.get();
}


charStr & charStr::operator=(std::string * to_convert){
  if(to_convert != NULL){
    char * ans = NULL;
    ans = new char[to_convert->size() + 1]();
    if (ans  == NULL){
      std::cerr<<"construction of object of type charStr failed"<<std::endl;
      throw insufficientMemory();
    }
    strcpy(ans, to_convert->c_str());
    str.reset(ans);//this is guaranteed not to throw exception
    sz = to_convert->size();
  }
  return *this;
}

char & charStr::operator[](size_t index){
  return const_cast<char &>(static_cast<const charStr *>(this)->operator[](index));

}
const char & charStr::operator[](size_t index) const{
  return str[index];
}

size_t charStr::size() const{
  return sz;
}

void charStr::print(){
  std::cout<<str.get()<<std::endl;
}

