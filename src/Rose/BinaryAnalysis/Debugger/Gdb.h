#ifndef ROSE_BinaryAnalysis_Debugger_Gdb_H
#define ROSE_BinaryAnalysis_Debugger_Gdb_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_DEBUGGER_GDB

#include <Rose/BinaryAnalysis/Debugger/Base.h>
#include <Rose/BinaryAnalysis/Debugger/GdbResponse.h>
#include <Rose/Yaml.h>

#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <boost/process.hpp>

#include <condition_variable>
#include <string>
#include <thread>

namespace Rose {
namespace BinaryAnalysis {
namespace Debugger {

/** Debugging using GNU GDB. */
class Gdb: public Base {

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Types
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Shared ownership pointer. */
    using Ptr = GdbPtr;

    /** Describe a specimen. */
    struct Specimen {
        /** Name of GDB executable. */
        boost::filesystem::path gdbName = "gdb-multiarch";

        /** Path to executable file.
         *
         *  This is optional. */
        boost::filesystem::path executable;

        /** Remote target.
         *
         *  This is a host name and port. */
        struct Remote {
            std::string host;                           /**< Host name for remote connection. */
            uint16_t port = 0;                          /**< TCP/IP port number for remote connection. */
        } remote;
    };

    // Thread-safe FIFO
    template<class T>
    class Fifo {
    public:
        using Value = T;

    private:
        std::condition_variable cond_;                  // notified by append()
        std::mutex mutex_;                              // protects the following data members
        std::list<Value> items_;
        bool isClosed_ = false;

    public:
        // Append an item to the list
        void append(const Value &item) {
            const std::lock_guard<std::mutex> lock(mutex_);
            items_.push_back(item);
            cond_.notify_one();
        }

        // Mark the list as closed. No more items can be appended.
        void close() {
            {
                const std::lock_guard<std::mutex> lock(mutex_);
                isClosed_ = true;
            }
            cond_.notify_all();
        }

        // Returns the item from the queue, if any.
        Sawyer::Optional<Value> nonblockingNext() {
            const std::lock_guard<std::mutex> lock(mutex_);
            if (items_.empty()) {
                return Sawyer::Nothing();
            } else {
                Value item = items_.front();
                items_.pop_front();
                return item;
            }
        }

        // Blocks until an item can be returned or the queue is closed
        Sawyer::Optional<Value> blockingNext() {
            std::unique_lock<std::mutex> lock(mutex_);
            while (items_.empty() && !isClosed_)
                cond_.wait(lock);
            if (items_.empty()) {
                return Sawyer::Nothing();
            } else {
                Value item = items_.front();
                items_.pop_front();
                return item;
            }
        }

        // Remvoe all items
        void reset() {
            std::lock_guard<std::mutex> lock(mutex_);
            items_.clear();
            isClosed_ = false;
        }
    };

    // GDB command response record
    struct Response {
        bool hasEndMarker = false;

        struct ResultRecord {
            enum ResultClass { EMPTY, DONE, RUNNING, CONNECTED, ERROR, EXIT };
            ResultClass rclass = EMPTY;
            std::string token;
            std::list<Yaml::Node> results;
        };

        std::list<ResultRecord> resultRecord;
    };


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Data members
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    std::thread gdbThread_;
    Fifo<std::string> gdbOutput_;
    boost::asio::io_service ios_;
    boost::process::async_pipe gdbOutputPipe_;
    boost::process::opstream gdbInput_;
    boost::asio::streambuf gdbOutputBuffer_;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors and destructors
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
protected:
    Gdb();
public:
    ~Gdb();

public:
    /** Allocating constructor.
     *
     *  The returned debugger is not attached to any specimen. */
    static Ptr instance();

    /** Allocating constructor.
     *
     *  Create a new debugger and attach it to the specified specimen. */
    static Ptr instance(const Specimen&);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Attaching and detaching
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Attach to a GDB server.
     *
     *  For remote debugging, attach to the GDB server running at the specified @p host and @p port. */
    virtual void attach(const Specimen&);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Overrides for methods declared and documented in the super class.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    virtual bool isAttached() override;
    virtual void detach() override;
    virtual void terminate() override;
    virtual std::vector<ThreadId> threadIds() override;
    virtual void executionAddress(ThreadId, rose_addr_t) override;
    virtual rose_addr_t executionAddress(ThreadId) override;
    virtual void setBreakPoint(const AddressInterval&) override;
    virtual void clearBreakPoint(const AddressInterval&) override;
    virtual void clearBreakPoints() override;
    virtual void singleStep(ThreadId) override;
    virtual void runToBreakPoint(ThreadId) override;
    virtual Sawyer::Container::BitVector readRegister(ThreadId, RegisterDescriptor) override;
    virtual void writeRegister(ThreadId, RegisterDescriptor, const Sawyer::Container::BitVector&) override;
    virtual void writeRegister(ThreadId, RegisterDescriptor, uint64_t value) override;
    virtual size_t readMemory(rose_addr_t va, size_t nBytes, uint8_t *buffer) override;
    virtual std::vector<uint8_t> readMemory(rose_addr_t va, size_t nBytes) override;
    virtual Sawyer::Container::BitVector readMemory(rose_addr_t va, size_t nBytes, ByteOrder::Endianness order) override;
    virtual size_t writeMemory(rose_addr_t va, size_t nBytes, const uint8_t *bytes) override;
    virtual std::string readCString(rose_addr_t va, size_t maxBytes = UNLIMITED) override;
    virtual bool isTerminated() override;
    virtual std::string howTerminated() override;
    virtual RegisterDictionaryPtr registerDictionary() const override;
    virtual Disassembler::BasePtr disassembler() const override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Supporting functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    // Send a command to GDB and read its response
    void sendCommand(const std::string&);

    // Read GDB's multi-line response to a command
    std::list<GdbResponse> readRequiredResponses();
    std::list<GdbResponse> readOptionalResponses();
    std::list<GdbResponse> readResponseSet(bool required);

};

} // namespace
} // namespace
} // namespace

#endif
#endif
