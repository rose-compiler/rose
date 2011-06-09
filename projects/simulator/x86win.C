/* Simulates a Windows executable (PE) using WINE on Linux */
#include "rose.h"
#include "RSIM_Private.h"

#ifdef ROSE_ENABLE_SIMULATOR /* protects this whole file */

#include "RSIM_Linux32.h"
#include "RSIM_Adapter.h"


int
main(int argc, char *argv[], char *envp[])
{
    RSIM_Linux32 sim;


    /* Debugging stuff */
    rose_addr_t bp = 0x68000850;

    RegisterDumper regdump(bp);
    sim.install_callback(&regdump);

    MemoryMapDumper mapdump(bp);
    sim.install_callback(&mapdump);

    MemoryInitializer dd32data("x-real-dd32-data", 0x08054000, bp);
    sim.install_callback(&dd32data);
    MemoryInitializer ldsodata("x-real-ldso-data", 0x6801c000, bp);
    sim.install_callback(&ldsodata);
    MemoryInitializer preloaddata("x-real-preload-data", 0x7c403000, bp);
    sim.install_callback(&preloaddata);
    MemoryInitializer stackdata("x-real-stack", 0xbffeb000, bp);
    sim.install_callback(&stackdata);


    /* Run the simulator */
    int n = sim.configure(argc, argv, envp);
    sim.exec(argc-n, argv+n);
    
    {
        uint8_t buf[3*4096];
        size_t nread = sim.get_process()->mem_read(buf, 0x7c400000, sizeof buf);
        assert(nread==sizeof buf);
        int fd = open("x2", O_CREAT|O_TRUNC|O_RDWR, 0666);
        assert(fd>=0);
        write(fd, buf, sizeof buf);
        close(fd);
    }

    sim.activate();
    sim.main_loop();
    sim.deactivate();
    sim.describe_termination(stderr);
    sim.terminate_self(); // probably doesn't return
    return 0;
}





#else
int main(int, char *argv[])
{
    std::cerr <<argv[0] <<": not supported on this platform" <<std::endl;
    return 0;
}

#endif /* ROSE_ENABLE_SIMULATOR */
