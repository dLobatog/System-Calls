# Generated by configure from .in at Sun Jan 23 19:29:20 CET 2005

CC	= gcc
LD	= gcc

CFLAGS	= -g 
CFLAGS	+= -I. 
LDFLAGS	= 

OBJS	= 

LIBS	= 

SRCS	= $(patsubst %.o,%.c,$(OBJS))

PRGS	= bin2text combine filter split statistics text2bin text2bin2


all: $(PRGS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $*.c $(INCLUDE) -o $@

$(PRGS): $(OBJS)
$(PRGS): $(LIBS)
$(PRGS): % : %.o
	$(CC) $(CFLAGS) -o $@ $< $(OBJS) $(LDFLAGS) $(LIBS)

clean:
	-rm -f *.o  *~ $(PRGS)
