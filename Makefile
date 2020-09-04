PROG = daqmw-tc-server
CFLAGS += -g -O2 -Wall
CFLAGS += -std=gnu99
# CFLAGS += -pthread
# LDLIBS += -L/usr/local/lib -lmylib
LDLIBS += -lm
# LDFLAGS += -pthread

all: $(PROG)
OBJS += $(PROG).o
OBJS += my_socket.o
OBJS += my_signal.o
OBJS += create_data.o
$(PROG): $(OBJS)

clean:
	rm -f *.o $(PROG)
