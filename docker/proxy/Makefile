CXX = g++ -std=gnu++11 -Wall -Werror -pedantic -pthread -ggdb3 -c
SRCS = $(wildcard *.cpp)
OBJ = $(patsubst, %.cpp, %.o, $(SRCS))
TARGET = socketkeeper.o addrkeeper.o widget.o cache.o httpRequest.o httpBase.o httpResponse.o logger.o
.PHONY: clean depend

simple_test: simple_test.o $(TARGET)
	g++ -std=gnu++11 -pthread -o $@ $^

%.o: %.cpp
	$(CXX) $<

depend:
	makedepend $(SRCS)
clean:
	rm -f *# *~ *.o *.bak simple_test
# DO NOT DELETE

widget.o: widget.h exception.h /usr/include/unistd.h /usr/include/features.h
widget.o: /usr/include/stdc-predef.h /usr/include/getopt.h
widget.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h
widget.o: /usr/include/wchar.h /usr/include/stdlib.h /usr/include/alloca.h
simple_test.o: addrkeeper.h /usr/include/netdb.h /usr/include/features.h
simple_test.o: /usr/include/stdc-predef.h /usr/include/netinet/in.h
simple_test.o: /usr/include/stdint.h /usr/include/endian.h
simple_test.o: /usr/include/rpc/netdb.h cache.h socketkeeper.h httpRequest.h
simple_test.o: httpBase.h httpResponse.h /usr/include/time.h
simple_test.o: /usr/include/xlocale.h /usr/include/unistd.h
simple_test.o: /usr/include/getopt.h exception.h /usr/include/stdio.h
simple_test.o: /usr/include/libio.h /usr/include/_G_config.h
simple_test.o: /usr/include/wchar.h /usr/include/stdlib.h
simple_test.o: /usr/include/alloca.h widget.h logger.h
socketkeeper.o: socketkeeper.h addrkeeper.h /usr/include/netdb.h
socketkeeper.o: /usr/include/features.h /usr/include/stdc-predef.h
socketkeeper.o: /usr/include/netinet/in.h /usr/include/stdint.h
socketkeeper.o: /usr/include/endian.h /usr/include/rpc/netdb.h exception.h
socketkeeper.o: /usr/include/unistd.h /usr/include/getopt.h
socketkeeper.o: /usr/include/stdio.h /usr/include/libio.h
socketkeeper.o: /usr/include/_G_config.h /usr/include/wchar.h
socketkeeper.o: /usr/include/stdlib.h /usr/include/alloca.h
socketkeeper.o: /usr/include/errno.h
httpResponse.o: httpResponse.h httpBase.h socketkeeper.h addrkeeper.h
httpResponse.o: /usr/include/netdb.h /usr/include/features.h
httpResponse.o: /usr/include/stdc-predef.h /usr/include/netinet/in.h
httpResponse.o: /usr/include/stdint.h /usr/include/endian.h
httpResponse.o: /usr/include/rpc/netdb.h httpRequest.h /usr/include/time.h
httpResponse.o: /usr/include/xlocale.h /usr/include/assert.h widget.h
httpResponse.o: exception.h /usr/include/unistd.h /usr/include/getopt.h
httpResponse.o: /usr/include/stdio.h /usr/include/libio.h
httpResponse.o: /usr/include/_G_config.h /usr/include/wchar.h
httpResponse.o: /usr/include/stdlib.h /usr/include/alloca.h
httpRequest.o: httpRequest.h httpBase.h socketkeeper.h addrkeeper.h
httpRequest.o: /usr/include/netdb.h /usr/include/features.h
httpRequest.o: /usr/include/stdc-predef.h /usr/include/netinet/in.h
httpRequest.o: /usr/include/stdint.h /usr/include/endian.h
httpRequest.o: /usr/include/rpc/netdb.h exception.h /usr/include/unistd.h
httpRequest.o: /usr/include/getopt.h /usr/include/stdio.h
httpRequest.o: /usr/include/libio.h /usr/include/_G_config.h
httpRequest.o: /usr/include/wchar.h /usr/include/stdlib.h
httpRequest.o: /usr/include/alloca.h
addrkeeper.o: addrkeeper.h /usr/include/netdb.h /usr/include/features.h
addrkeeper.o: /usr/include/stdc-predef.h /usr/include/netinet/in.h
addrkeeper.o: /usr/include/stdint.h /usr/include/endian.h
addrkeeper.o: /usr/include/rpc/netdb.h exception.h /usr/include/unistd.h
addrkeeper.o: /usr/include/getopt.h /usr/include/stdio.h /usr/include/libio.h
addrkeeper.o: /usr/include/_G_config.h /usr/include/wchar.h
addrkeeper.o: /usr/include/stdlib.h /usr/include/alloca.h widget.h
logger.o: logger.h httpRequest.h httpBase.h socketkeeper.h addrkeeper.h
logger.o: /usr/include/netdb.h /usr/include/features.h
logger.o: /usr/include/stdc-predef.h /usr/include/netinet/in.h
logger.o: /usr/include/stdint.h /usr/include/endian.h
logger.o: /usr/include/rpc/netdb.h httpResponse.h /usr/include/time.h
logger.o: /usr/include/xlocale.h
httpBase.o: httpBase.h socketkeeper.h addrkeeper.h /usr/include/netdb.h
httpBase.o: /usr/include/features.h /usr/include/stdc-predef.h
httpBase.o: /usr/include/netinet/in.h /usr/include/stdint.h
httpBase.o: /usr/include/endian.h /usr/include/rpc/netdb.h exception.h
httpBase.o: /usr/include/unistd.h /usr/include/getopt.h /usr/include/stdio.h
httpBase.o: /usr/include/libio.h /usr/include/_G_config.h
httpBase.o: /usr/include/wchar.h /usr/include/stdlib.h /usr/include/alloca.h
httpBase.o: widget.h
cache.o: cache.h socketkeeper.h addrkeeper.h /usr/include/netdb.h
cache.o: /usr/include/features.h /usr/include/stdc-predef.h
cache.o: /usr/include/netinet/in.h /usr/include/stdint.h
cache.o: /usr/include/endian.h /usr/include/rpc/netdb.h httpRequest.h
cache.o: httpBase.h httpResponse.h /usr/include/time.h /usr/include/xlocale.h
cache.o: /usr/include/unistd.h /usr/include/getopt.h exception.h
cache.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h
cache.o: /usr/include/wchar.h /usr/include/stdlib.h /usr/include/alloca.h
cache.o: widget.h /usr/include/assert.h /usr/include/errno.h
