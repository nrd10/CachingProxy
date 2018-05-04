#ifndef _WIDGET__H
#define _WIDGET__H
#include <cstring>
#include <string>
#include <memory>

//used to hold the buffer of bytes that we will be interpreting from HTTP
//messages sent by the server
//used to gaurantee exception safety since our buffers will be allocated 
//on the heap
class charStr{
  std::unique_ptr<char[]> str;
  size_t sz;
 public:
  charStr() = default;
  charStr(std::string && toMove);
  explicit charStr(int num);
  operator char *(); //implicit user-defined conversion operator
  char & operator [](size_t index);
  const char & operator [](size_t index) const;
  charStr & operator= (std::string * to_convert);
  size_t size() const;
  void print();
};

#endif
