MAIN = 2mm
SRCS = 2mm.c 
OBJS = $(SRCS:.c=.o)
all: $(MAIN)
$(MAIN): $(OBJS) 
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJS) -o $(MAIN)
.c.o:  
	$(CC) $(INCLUDES) -c $< -o $@

nest_tile: $(SRCS) 
	$(NESTTILE)
	$(CC) $(CTILEFLAGS) $(INCLUDES) -c $< -rose:o nest_tile_$< 
	$(1STTILE)
	$(CC) $(CTILEFLAGS) $(INCLUDES) -c $< -rose:o 1st_tile_$< 
	$(2NDTILE)
	$(CC) $(CTILEFLAGS) $(INCLUDES) -c 1st_tile_$< -rose:o 2nd_tile_$< 


fuse_tile: $(SRCS)
	$(CC) $(CMAXFUSEFLAGS) $(INCLUDES) -c $< -rose:o fuse_$< -o $@
	$(1STTILE)
	$(CC) $(CMAXFUSEFLAGS) $(INCLUDES) -c fuse_$< -rose:o tile_$< 
clean: 
	rm *.o $(MAIN)
cleanROSE: 
	rm *.o *_rose_* *_tile_* *_fuse_* $(MAIN)
depend: $(SRCS)
	makedepend $(INCLUDES) $^
