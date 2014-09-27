TARGET=main
SRCS=$(wildcard *.c ./*/*.c)
OBJS=$(patsubst %.c,%.o,$(SRCS) ) 

CC=gcc 

$(TARGET):$(OBJS) 
	$(CC) -o $(TARGET) $(OBJS)  -lpthread
%.o:%.c 
	$(CC) -c  $< -o $@ 
#；设置为目标
.PHONY:clean  echo

echo:
	@echo "source files: $(SRCS)"
	@echo "object files: $(OBJS)"

clean: 
	-rm $(OBJS) $(TARGET) 



