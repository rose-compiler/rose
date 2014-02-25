MAIN = jacobi-2d-imper
SRCS = jacobi-2d-imper.c 
OBJS = $(SRCS:.c=.o)
all: $(MAIN)
$(MAIN): $(OBJS) 
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJS) -o $(MAIN)
.c.o:  
	$(CC) $(CMAXFUSEFLAGS) $(INCLUDES) -c $< -o $@
	mv rose_$< fused_rose_$<
	$(CC) $(CTILEFLAGS) $(INCLUDES) -c fused_rose_$< -o $@
	mv rose_fused_rose_$< tiled_fused_rose_$<
clean: 
	rm *.o $(MAIN)
cleanROSE: 
	rm *.o *_rose_* $(MAIN)
depend: $(SRCS)
	makedepend $(INCLUDES) $^
