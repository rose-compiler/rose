#include "rose.h"
#include "Debugger.h"

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>

size_t
Debugger::memory(rose_addr_t addr, size_t request_size, unsigned char *buffer) {
    size_t return_size;
    const unsigned char *bytes = (const unsigned char*)execute(CMD_MEM, addr, request_size, &return_size);
    assert(request_size>=return_size);
    memcpy(buffer, bytes, return_size);
    memset(buffer+return_size, 0, request_size-return_size);
    return return_size;
}

uint64_t
Debugger::memory(rose_addr_t addr) {
    size_t size;
    const unsigned char *bytes = (const unsigned char*)execute(CMD_MEM, addr, 4, &size);
    if (size!=4)
        return 0;
    uint64_t retval = 0;
    for (size_t j=0; j<size; j++) {
        retval |= (uint64_t)(bytes[j]) << (8*j);
    }
    return retval;
}

const RegisterSet&
Debugger::registers() {
    if (!regs_current) {
        size_t size;
        const void *x = execute(CMD_REGS, &size);
        assert(size==sizeof(regs));
        memcpy(&regs, x, size);
        regs_current = true;
    }
    return regs;
}

void
Debugger::ctor(const std::string &hostname, short port) {
    /* Initialize data members. */
    server = -1;
    status = 0;
    result = NULL;
    result_nalloc = 0;
    memset(&regs, 0, sizeof regs);
    regs_current = false;

    struct hostent *he = gethostbyname2(hostname.c_str(), AF_INET);
    assert(he);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = my_htons(port);
    assert(he->h_length==sizeof(addr.sin_addr.s_addr));
    memcpy(&addr.sin_addr.s_addr, he->h_addr, he->h_length);

    server = socket(AF_INET, SOCK_STREAM, 0);
    assert(server>=0);
    if (connect(server, (struct sockaddr*)&addr, sizeof addr)<0) {
        fprintf(stderr, "cannot connect to server at %s:%hd: %s\n", hostname.c_str(), port, strerror(errno));
        exit(1);
    }
}

short
Debugger::my_htons(short n) {
    static unsigned u = 1;
    if (*((char*)&u)) {
        /* Little endian */
        return (((unsigned short)n & 0x00ff)<<8) | (((unsigned short)n & 0xff00)>>8);
    } else {
        return n;
    }
}

void
Debugger::send(DebuggerCommand cmd) {
    regs_current = false;
    unsigned char byte = cmd;
    ssize_t n = write(server, &byte, 1);
    if (n<0) {
        fprintf(stderr, "write command: %s\n", strerror(errno));
        exit(1);
    } else if (n!=1) {
        fprintf(stderr, "write command: short write\n");
        exit(1);
    }
}

void
Debugger::send(DebuggerCommand cmd, uint64_t arg1) {
    send(cmd);
    ssize_t n = write(server, &arg1, sizeof arg1);
    if (n<0) {
        fprintf(stderr, "write arg1: %s\n", strerror(errno));
        exit(1);
    } else if (n!=sizeof arg1) {
        fprintf(stderr, "write arg1: short write\n");
        exit(1);
    }
}

void
Debugger::send(DebuggerCommand cmd, uint64_t arg1, uint64_t arg2) {
    send(cmd, arg1);
    ssize_t n = write(server, &arg2, sizeof arg2);
    if (n<0) {
        fprintf(stderr, "write arg2: %s\n", strerror(errno));
        exit(1);
    } else if (n!=sizeof arg2) {
        fprintf(stderr, "write arg2: short write\n");
        exit(1);
    }
}

uint64_t
Debugger::recv() {
    ssize_t n = read(server, &status, sizeof status);
    if (n<0) {
        fprintf(stderr, "read status: %s\n", strerror(errno));
        exit(1);
    } else if (n!=sizeof status) {
        fprintf(stderr, "read status: short read\n");
        exit(1);
    }
    return status;
}

size_t
Debugger::recv(void **bufp, size_t *sizep) {
    size_t need = recv();
    if (need > *sizep) {
        *sizep = need;
        *bufp = realloc(*bufp, *sizep);
        assert(*bufp);
    }
    ssize_t n = read(server, *bufp, need);
    if (n<0) {
        fprintf(stderr, "read data: %s\n", strerror(errno));
        exit(1);
    } else if ((size_t)n!=need) {
        fprintf(stderr, "read data: short read\n");
        exit(1);
    }
    return need;
}
