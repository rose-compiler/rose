/* Demonstrates how to perform concrete analysis on the "updcrc" function and provide memory values on the fly.  We allow the
 * specimen (compiled from gzip_sample_1.c) to run until main() is entered, then allocate some memory, initialize it with a
 * string, and analyze the updcrc() function to obtain an output value.
 *
 * Since this is a concrete analysis, most of it could be done in GDB as well.  difficult to adjust memory
 * mapping of the specimen in GDB.
 *
 * Documentation can be found by running:
 *    $ doxygen RSIM.dxy
 *    $ $WEB_BROWSER docs/html/index.html
 */

#include "rose.h"
#include "RSIM_Private.h"

#ifdef ROSE_ENABLE_SIMULATOR /* protects this whole file */

#include "RSIM_Linux32.h"


// Class to provide values for reading from the crc_32_tab specimen array.
class CrcTable: public RSIM_Callbacks::MemoryCallback {
public:
    RTS_Message *trace;                 // used for output since memory access isn't (yet) associated with a thread
    rose_addr_t table_va;               // specimen address of the crc_32_tab array
    uint32_t table[256];                // values we will provide ourselves when specimen reads from its crc_32_tab
    std::vector<size_t> seen;           // analysis result keeps track of how many times each table entry was read

    // Nothing says we have to initialize the table in the simulator the same way it was initialized in the specimen. In fact,
    // we don't even need to compute the values statically
    CrcTable(RTS_Message *trace, rose_addr_t table_va)
        : trace(trace), table_va(table_va) {
        seen.resize(256, 0);
        table[0]   = 0x00000000L;  table[1]   = 0x77073096L;  table[2]   = 0xee0e612cL;  table[3]   = 0x990951baL;
        table[4]   = 0x076dc419L;  table[5]   = 0x706af48fL;  table[6]   = 0xe963a535L;  table[7]   = 0x9e6495a3L;
        table[8]   = 0x0edb8832L;  table[9]   = 0x79dcb8a4L;  table[10]  = 0xe0d5e91eL;  table[11]  = 0x97d2d988L;
        table[12]  = 0x09b64c2bL;  table[13]  = 0x7eb17cbdL;  table[14]  = 0xe7b82d07L;  table[15]  = 0x90bf1d91L;
        table[16]  = 0x1db71064L;  table[17]  = 0x6ab020f2L;  table[18]  = 0xf3b97148L;  table[19]  = 0x84be41deL;
        table[20]  = 0x1adad47dL;  table[21]  = 0x6ddde4ebL;  table[22]  = 0xf4d4b551L;  table[23]  = 0x83d385c7L;
        table[24]  = 0x136c9856L;  table[25]  = 0x646ba8c0L;  table[26]  = 0xfd62f97aL;  table[27]  = 0x8a65c9ecL;
        table[28]  = 0x14015c4fL;  table[29]  = 0x63066cd9L;  table[30]  = 0xfa0f3d63L;  table[31]  = 0x8d080df5L;
        table[32]  = 0x3b6e20c8L;  table[33]  = 0x4c69105eL;  table[34]  = 0xd56041e4L;  table[35]  = 0xa2677172L;
        table[36]  = 0x3c03e4d1L;  table[37]  = 0x4b04d447L;  table[38]  = 0xd20d85fdL;  table[39]  = 0xa50ab56bL;
        table[40]  = 0x35b5a8faL;  table[41]  = 0x42b2986cL;  table[42]  = 0xdbbbc9d6L;  table[43]  = 0xacbcf940L;
        table[44]  = 0x32d86ce3L;  table[45]  = 0x45df5c75L;  table[46]  = 0xdcd60dcfL;  table[47]  = 0xabd13d59L;
        table[48]  = 0x26d930acL;  table[49]  = 0x51de003aL;  table[50]  = 0xc8d75180L;  table[51]  = 0xbfd06116L;
        table[52]  = 0x21b4f4b5L;  table[53]  = 0x56b3c423L;  table[54]  = 0xcfba9599L;  table[55]  = 0xb8bda50fL;
        table[56]  = 0x2802b89eL;  table[57]  = 0x5f058808L;  table[58]  = 0xc60cd9b2L;  table[59]  = 0xb10be924L;
        table[60]  = 0x2f6f7c87L;  table[61]  = 0x58684c11L;  table[62]  = 0xc1611dabL;  table[63]  = 0xb6662d3dL;
        table[64]  = 0x76dc4190L;  table[65]  = 0x01db7106L;  table[66]  = 0x98d220bcL;  table[67]  = 0xefd5102aL;
        table[68]  = 0x71b18589L;  table[69]  = 0x06b6b51fL;  table[70]  = 0x9fbfe4a5L;  table[71]  = 0xe8b8d433L;
        table[72]  = 0x7807c9a2L;  table[73]  = 0x0f00f934L;  table[74]  = 0x9609a88eL;  table[75]  = 0xe10e9818L;
        table[76]  = 0x7f6a0dbbL;  table[77]  = 0x086d3d2dL;  table[78]  = 0x91646c97L;  table[79]  = 0xe6635c01L;
        table[80]  = 0x6b6b51f4L;  table[81]  = 0x1c6c6162L;  table[82]  = 0x856530d8L;  table[83]  = 0xf262004eL;
        table[84]  = 0x6c0695edL;  table[85]  = 0x1b01a57bL;  table[86]  = 0x8208f4c1L;  table[87]  = 0xf50fc457L;
        table[88]  = 0x65b0d9c6L;  table[89]  = 0x12b7e950L;  table[90]  = 0x8bbeb8eaL;  table[91]  = 0xfcb9887cL;
        table[92]  = 0x62dd1ddfL;  table[93]  = 0x15da2d49L;  table[94]  = 0x8cd37cf3L;  table[95]  = 0xfbd44c65L;
        table[96]  = 0x4db26158L;  table[97]  = 0x3ab551ceL;  table[98]  = 0xa3bc0074L;  table[99]  = 0xd4bb30e2L;
        table[100] = 0x4adfa541L;  table[101] = 0x3dd895d7L;  table[102] = 0xa4d1c46dL;  table[103] = 0xd3d6f4fbL;
        table[104] = 0x4369e96aL;  table[105] = 0x346ed9fcL;  table[106] = 0xad678846L;  table[107] = 0xda60b8d0L;
        table[108] = 0x44042d73L;  table[109] = 0x33031de5L;  table[110] = 0xaa0a4c5fL;  table[111] = 0xdd0d7cc9L;
        table[112] = 0x5005713cL;  table[113] = 0x270241aaL;  table[114] = 0xbe0b1010L;  table[115] = 0xc90c2086L;
        table[116] = 0x5768b525L;  table[117] = 0x206f85b3L;  table[118] = 0xb966d409L;  table[119] = 0xce61e49fL;
        table[120] = 0x5edef90eL;  table[121] = 0x29d9c998L;  table[122] = 0xb0d09822L;  table[123] = 0xc7d7a8b4L;
        table[124] = 0x59b33d17L;  table[125] = 0x2eb40d81L;  table[126] = 0xb7bd5c3bL;  table[127] = 0xc0ba6cadL;
        table[128] = 0xedb88320L;  table[129] = 0x9abfb3b6L;  table[130] = 0x03b6e20cL;  table[131] = 0x74b1d29aL;
        table[132] = 0xead54739L;  table[133] = 0x9dd277afL;  table[134] = 0x04db2615L;  table[135] = 0x73dc1683L;
        table[136] = 0xe3630b12L;  table[137] = 0x94643b84L;  table[138] = 0x0d6d6a3eL;  table[139] = 0x7a6a5aa8L;
        table[140] = 0xe40ecf0bL;  table[141] = 0x9309ff9dL;  table[142] = 0x0a00ae27L;  table[143] = 0x7d079eb1L;
        table[144] = 0xf00f9344L;  table[145] = 0x8708a3d2L;  table[146] = 0x1e01f268L;  table[147] = 0x6906c2feL;
        table[148] = 0xf762575dL;  table[149] = 0x806567cbL;  table[150] = 0x196c3671L;  table[151] = 0x6e6b06e7L;
        table[152] = 0xfed41b76L;  table[153] = 0x89d32be0L;  table[154] = 0x10da7a5aL;  table[155] = 0x67dd4accL;
        table[156] = 0xf9b9df6fL;  table[157] = 0x8ebeeff9L;  table[158] = 0x17b7be43L;  table[159] = 0x60b08ed5L;
        table[160] = 0xd6d6a3e8L;  table[161] = 0xa1d1937eL;  table[162] = 0x38d8c2c4L;  table[163] = 0x4fdff252L;
        table[164] = 0xd1bb67f1L;  table[165] = 0xa6bc5767L;  table[166] = 0x3fb506ddL;  table[167] = 0x48b2364bL;
        table[168] = 0xd80d2bdaL;  table[169] = 0xaf0a1b4cL;  table[170] = 0x36034af6L;  table[171] = 0x41047a60L;
        table[172] = 0xdf60efc3L;  table[173] = 0xa867df55L;  table[174] = 0x316e8eefL;  table[175] = 0x4669be79L;
        table[176] = 0xcb61b38cL;  table[177] = 0xbc66831aL;  table[178] = 0x256fd2a0L;  table[179] = 0x5268e236L;
        table[180] = 0xcc0c7795L;  table[181] = 0xbb0b4703L;  table[182] = 0x220216b9L;  table[183] = 0x5505262fL;
        table[184] = 0xc5ba3bbeL;  table[185] = 0xb2bd0b28L;  table[186] = 0x2bb45a92L;  table[187] = 0x5cb36a04L;
        table[188] = 0xc2d7ffa7L;  table[189] = 0xb5d0cf31L;  table[190] = 0x2cd99e8bL;  table[191] = 0x5bdeae1dL;
        table[192] = 0x9b64c2b0L;  table[193] = 0xec63f226L;  table[194] = 0x756aa39cL;  table[195] = 0x026d930aL;
        table[196] = 0x9c0906a9L;  table[197] = 0xeb0e363fL;  table[198] = 0x72076785L;  table[199] = 0x05005713L;
        table[200] = 0x95bf4a82L;  table[201] = 0xe2b87a14L;  table[202] = 0x7bb12baeL;  table[203] = 0x0cb61b38L;
        table[204] = 0x92d28e9bL;  table[205] = 0xe5d5be0dL;  table[206] = 0x7cdcefb7L;  table[207] = 0x0bdbdf21L;
        table[208] = 0x86d3d2d4L;  table[209] = 0xf1d4e242L;  table[210] = 0x68ddb3f8L;  table[211] = 0x1fda836eL;
        table[212] = 0x81be16cdL;  table[213] = 0xf6b9265bL;  table[214] = 0x6fb077e1L;  table[215] = 0x18b74777L;
        table[216] = 0x88085ae6L;  table[217] = 0xff0f6a70L;  table[218] = 0x66063bcaL;  table[219] = 0x11010b5cL;
        table[220] = 0x8f659effL;  table[221] = 0xf862ae69L;  table[222] = 0x616bffd3L;  table[223] = 0x166ccf45L;
        table[224] = 0xa00ae278L;  table[225] = 0xd70dd2eeL;  table[226] = 0x4e048354L;  table[227] = 0x3903b3c2L;
        table[228] = 0xa7672661L;  table[229] = 0xd06016f7L;  table[230] = 0x4969474dL;  table[231] = 0x3e6e77dbL;
        table[232] = 0xaed16a4aL;  table[233] = 0xd9d65adcL;  table[234] = 0x40df0b66L;  table[235] = 0x37d83bf0L;
        table[236] = 0xa9bcae53L;  table[237] = 0xdebb9ec5L;  table[238] = 0x47b2cf7fL;  table[239] = 0x30b5ffe9L;
        table[240] = 0xbdbdf21cL;  table[241] = 0xcabac28aL;  table[242] = 0x53b39330L;  table[243] = 0x24b4a3a6L;
        table[244] = 0xbad03605L;  table[245] = 0xcdd70693L;  table[246] = 0x54de5729L;  table[247] = 0x23d967bfL;
        table[248] = 0xb3667a2eL;  table[249] = 0xc4614ab8L;  table[250] = 0x5d681b02L;  table[251] = 0x2a6f2b94L;
        table[252] = 0xb40bbe37L;  table[253] = 0xc30c8ea1L;  table[254] = 0x5a05df1bL;  table[255] = 0x2d02ef8dL;
    }

    virtual CrcTable *clone() { return this; }

    virtual bool operator()(bool enabled, const Args &args) {
        // Trigger only if we're reading a table entry
        if (enabled && args.how==MemoryMap::MM_PROT_READ && 4==args.nbytes &&
            args.va>=table_va && args.va<table_va+sizeof(table) && 0==(args.va-table_va)%4) {
            size_t idx = (args.va-table_va)/4;
            trace->mesg("CrcTable: read entry %zu = 0x%08"PRIx32, idx, table[idx]);
            seen[idx] += 1;
            memcpy(args.buffer, table+idx, 4);
            args.nbytes_xfer = 4;
            enabled = false;
        }
        return enabled;
    }

    void show_results() {
        trace->mesg("CrcTable results: the following table entries were read by the specimen:");
        for (size_t i=0; i<seen.size(); i++) {
            if (seen[i]>0)
                trace->mesg("CrcTable results:   entry #%zu read %zu time%s", i, seen[i], 1==seen[i]?"":"s");
        }
    }
};

// Monitors the CPU instruction pointer.  When it reaches the specified value (beginning of main()), perform some analysis as
// described at the top of this file.
class Analysis: public RSIM_Callbacks::InsnCallback {
public:
    rose_addr_t trigger_va, analysis_va;
    bool triggered;
    uint8_t buf[0x1000];                // backing store for specimen memory used during this analysis
    rose_addr_t buf_va;                 // specimen address for "buf"
    pt_regs_32 regs;                    // registers we save during the analysis, and restore at the end
    CrcTable *crctable;                 // callback for dynamic generation of the CRC table

    static const uint32_t END_OF_ANALYSIS = 0x99887766;

    Analysis(rose_addr_t trigger_va, rose_addr_t analysis_va)
        : trigger_va(trigger_va), analysis_va(analysis_va), triggered(false), buf_va(0), crctable(NULL) {}

    // No-op since we're single threaded for this demo.
    virtual Analysis *clone() { return this; }

    virtual bool operator()(bool enabled, const Args &args) {
        RSIM_Process *process = args.thread->get_process();
        if (enabled && !triggered && args.insn->get_address()==trigger_va) {
            triggered = true;

            // Output is sent to the TRACE_MISC facility.  All messages will have a prefix that includes such things as the
            // process ID, the current EIP register value, time since start of simulation, etc.
            RTS_Message *trace = args.thread->tracing(TRACE_MISC);
            trace->mesg("Analysis: triggered for virtual address 0x%08"PRIx64, analysis_va);

            // Save registers to restore at the end of the analysis.
            regs = args.thread->get_regs();

            // Turn off delivery of async signals to the specimen since we're not actually simulating the real specimen at this
            // point.  We'll enable the delivery again when the analysis is finished.  Any signals that arrive during the
            // analysis will be delivered to the simulator rather than the specimen. Synchronous signals (SIGSEGV, etc) are
            // still delivered to the specimen.
            process->get_simulator()->deactivate();

            // Allocate some memory in the specimen for the buffer (first argument to updcrc()).  This buffer will be
            // removed from the specimen address space at the end of the analysis.  We could use RSIM_Process::mem_map(),
            // which is what the mmap and mmap2 system calls use, but doing so would modify future values returned by those
            // system calls.  By using a lower-level interface, we're able to map memory for the specimen without affecting
            // its future behavior.  GDB is unable to do this kind of manipulation.  We need a write lock around memory map
            // manipulation if this example were multi-threaded.  The buffer should be at or above 0x40000000, at least one
            // page in length, and aligned on a page boundary.
            RTS_WRITE(process->rwlock()) {
                buf_va = process->get_memory()->find_free(0x40000000, sizeof buf, 0x1000);
                if (buf_va) {
                    MemoryMap::MapElement me(buf_va, sizeof buf, buf, 0, MemoryMap::MM_PROT_RWX);
                    me.set_name("Debugging page");
                    process->get_memory()->insert(me);
                }
            } RTS_WRITE_END;
            if (!buf_va) {
                trace->mesg("Analysis: couldn't find free memory for buffer.");
                return true;  // proceed as if we were never called
            }
            trace->mesg("Analysis: our debug buffer is mapped at 0x%08"PRIx64" in the specimen", buf_va);
            //process->mem_showmap(trace, "Current memory map is:", "    ");

            // Initialize the buffer with some data.  Writing data into our own buffer causes the data to also be written
            // into the specimen.  We could have alternatively used RSIM_Process::mem_write(), which would have used the
            // specimen's buffer address instead.
            strcpy((char*)buf, "hello world!");

            // We also use the top of the buffer as the stack so that our analysis doesn't interfere with the specimen's
            // normal stack.  If the specimen uses the usual stack semantics (that anything below the current stack pointer
            // is dead) and our analysis restores the original stack pointer, we don't really need to do this.   Doing it
            // this way helps the analysis avoid interfering with the specimen.
            buf[0x1000-1] = 0x90;                                       // x86 NOP instruction at return address
            *(uint32_t*)(buf+0x1000-5)  = strlen((char*)buf);           // updcrc's second argument
            *(uint32_t*)(buf+0x1000-9)  = (uint32_t)buf_va;             // updcrc's first argument
            *(uint32_t*)(buf+0x1000-13) = buf_va+0x1000-1;              // special return address, the NOP above
            args.thread->policy.writeGPR(x86_gpr_sp, args.thread->policy.number<32>(buf_va+0x1000-13));

            // We know that the function being analyzed has a local variable (crc_32_tab) which is an array of 256 double
            // words.  We also know (by looking at the assembly) that the array ends 25 bytes from the top of our "buf" and
            // extends downward another 1024 bytes.  In otherwords, at offsets 0xbe7 to 0xfe7.
            crctable = new CrcTable(trace, buf_va+0xbe7);
            process->install_callback(crctable);

            // Modify EIP to point to the entry address of the function being analyzed, and set "enabled" to false to
            // indicate that the current instruction (the first instruction of main), should not be simulated at this
            // time.
            args.thread->policy.writeIP(args.thread->policy.number<32>(analysis_va));
            enabled = false;
            trace->mesg("Analysis: simulator will continue to run to analyse the function...");

        } else if (enabled && triggered && args.insn->get_address()==buf_va+0x1000-1) {
            // We've just returned from the analyzed function and we're about to execute the NOP instruction we inserted at the
            // top of our debugging stack.  Obtain the analyzed function's return value from the EAX register, restore all
            // registers to their original values, and unmap our debugging buffer from the specimen.
            RTS_Message *trace = args.thread->tracing(TRACE_MISC);
            uint32_t result = args.thread->policy.readGPR(x86_gpr_ax).known_value();
            trace->mesg("Analysis: function returned 0x%08"PRIx32, result);

            // Unmap our debugging page of memory
            RTS_WRITE(process->rwlock()) {
                process->get_memory()->erase(MemoryMap::MapElement(buf_va, sizeof buf));
            } RTS_WRITE_END;

            // Restore registers
            args.thread->init_regs(regs);

            // Deregister the dynamic CRC table
            process->get_callbacks().remove_memory_callback(RSIM_Callbacks::BEFORE, crctable);
            crctable->show_results();

            // Reactivate delivery of async signals to the specimen.
            process->get_simulator()->activate();

            // No need to execute our NOP instruction.
            enabled = false;
            trace->mesg("Analysis: simulator will continue running specimen now...");
        }
        return enabled;
    }
};


int main(int argc, char *argv[], char *envp[])
{
    RSIM_Linux32 sim;
    int n = sim.configure(argc, argv, envp);

    // Parse the ELF container so we can get to the symbol table.  This is standard ROSE static analysis.
    char *rose_argv[4];
    rose_argv[0] = argv[0];
    rose_argv[1] = strdup("-rose:read_executable_file_format_only");
    rose_argv[2] = argv[n];
    rose_argv[3] = NULL;
    SgProject *project = frontend(3, rose_argv);

    // Find the address of "main" and "updcrc" functions from the symbol table of the primary executable (the one we just
    // parsed).
    rose_addr_t main_va = RSIM_Tools::FunctionFinder().address(project, "main");
    assert(main_va!=0);
    rose_addr_t updcrc_va = RSIM_Tools::FunctionFinder().address(project, "updcrc");
    assert(updcrc_va!=0);

    // Register the analysis callback.
    sim.install_callback(new Analysis(main_va, updcrc_va));

    // The rest is the usual stuff needed to run the simulator.
    sim.install_callback(new RSIM_Tools::UnhandledInstruction);
    sim.exec(argc-n, argv+n);
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
