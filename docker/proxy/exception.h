#ifndef _EXCEPTION__H
#define _EXCEPTION__H
#include<exception>
#include <unistd.h>
#include <stdio.h>
#include "exception.h"
#include <stdlib.h>
#include <iostream>
#include <fstream>
class socketError: public std::exception {};

class connectError: public std::exception {};
class bindError: public std::exception{};
class setSockOptError: public std::exception{};
class listenError:public std::exception{};
class acceptError:public std::exception{};
class sockNameError:public std::exception{};
class impossible:public std::exception{};
class recvError:public std::exception{};
//class sendError:public std::exception{};
class getaddrInfoError:public std::exception{};


class insufficientMemory:public std::exception{};
class malformed:public std::exception{};
class badProtocol:public std::exception{};
class badHeader: public std::exception{};
class closeConnection: public std::exception{};
class badMethod: public std::exception{};

#endif
