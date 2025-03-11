#ifndef ROSE_BinaryAnalysis_Debugger_Gdb_H
#define ROSE_BinaryAnalysis_Debugger_Gdb_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_DEBUGGER_GDB
#include <Rose/BinaryAnalysis/Debugger/Base.h>

#include <Rose/BinaryAnalysis/AddressIntervalSet.h>
#include <Rose/BinaryAnalysis/Debugger/GdbResponse.h>
#include <Rose/Yaml.h>

#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <boost/process.hpp>

#include <condition_variable>
#include <future>
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
    class Specimen {
    public:
        /** Remote target.
         *
         *  This is a host name and port. */
        struct Remote {
            std::string host = "localhost";             /**< Host name for remote connection. */
            uint16_t port = 1234;                       /**< TCP/IP port number for remote connection. */
        };

    private:
        boost::filesystem::path gdbName_ = "gdb-multiarch"; // name of the GDB executable
        boost::filesystem::path executable_;                // optional name of executable containing symbols
        Remote remote_;                                     // how to connect to the GDB server

    public:
        /** Default constructor. */
        Specimen();

        /** Construct a specimen for a specific executable.
         *
         *  The executable name is only used to specify where the debugging symbols are stored. The actual executable being
         *  executed is run under the control of the GDB server which is configured seperately from the ROSE library which
         *  talks to the GDB client. */
        Specimen(const boost::filesystem::path &exeName);

        /** Construct a specimen for a specific executable and GDB server.
         *
         *  The executable name is only used to specify where the debugging symbols are stored. The actual executable being
         *  executed is run under the control of the GDB server which is configured seperately from the ROSE library which
         *  talks to the GDB client.
         *
         *  The @p host and @p port describe how to connect to the GDB server. */
        Specimen(const boost::filesystem::path &exeName, const std::string &host, uint16_t port);

    public:
        /** Property: Name of GDB executable.
         *
         * @{ */
        const boost::filesystem::path &gdbName() const;
        void gdbName(const boost::filesystem::path&);
        /** @} */

        /** Property: Optional path to executable file.
         *
         *  This is the name of the executable containing the debugging symbols. It is not necessarily the executable being
         *  run since that's the reponsibility of the separate GDB server.
         *
         * @{ */
        const boost::filesystem::path& executable() const;
        void executable(const boost::filesystem::path&);
        /** @} */

        /** Property: How to connect to the GDB server.
         *
         * @{ */
        const Remote& remote() const;
        void remote(const Remote&);
        void remote(const std::string &host, uint16_t port);
        /** @} */
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

        // Remove all items from this FIFO
        void clear() {
            const std::lock_guard<std::mutex> lock(mutex_);
            items_.clear();
            cond_.notify_all();
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
    std::vector<std::pair<std::string, RegisterDescriptor>> registers_;
    std::list<GdbResponse> responses_;                          // accumulated responses
    AddressIntervalSet breakPoints_;                            // all break points
    std::map<Address, unsigned /*bp_id*/> gdbBreakPoints_;      // subset of breakpoints known to GDB
    std::future<int> exitCodeFuture_;                           // exit code returned from GDB thread
    Sawyer::Optional<int> exitCode_;                            // exit code from the GDB process

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
    // Low-level stuff not often used publically but available nonetheless.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Send command to debugger.
     *
     *  Clears accumulated responses, then sends the specified command to GDB and returns the subsequent responses. The
     *  responses are all those that have arrived asynchronously since any prior command was executed, the required response
     *  for this command, and all asynchronous reponses that arrive before this function returns. The responses are also
     *  available with the @ref responses property. */
    const std::list<GdbResponse>& sendCommand(const std::string&);

    /** Read up to one pending asynchronous reponse.
     *
     *  If an asynchronous reponse is available, then this function consumes it from the queue and returns it. A response
     *  normally consists of multiple lines of output returned as a list of individual @ref GdbResponse objects. */
    std::list<GdbResponse> readRequiredResponses();

    /** Read one reponse.
     *
     *  Blocks until a response is available, then this function consumes it from the queue and returns it. A response
     *  normally consists of multiple lines of output returned as a list of individual @ref GdbResponse objects. */
    std::list<GdbResponse> readOptionalResponses();

    /** Return all accumulated responses.
     *
     *  This is the ordered sequence of GDB responses since the last time it was cleared. */
    const std::list<GdbResponse>& responses() const;

    /** Clear the accumulated responses. */
    void resetResponses();

    /** Get the list of register names and descriptors for this architecture.
     *
     *  These registers are reported in the same order as used by GDB. */
    const std::vector<std::pair<std::string, RegisterDescriptor>>& registerNames() const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Overrides for methods declared and documented in the super class.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    virtual bool isAttached() override;
    virtual void detach() override;
    virtual void terminate() override;
    virtual std::vector<ThreadId> threadIds() override;
    virtual void setBreakPoint(const AddressInterval&) override;
    virtual void setBreakPoints(const AddressIntervalSet&) override;
    virtual AddressIntervalSet breakPoints() override;
    virtual void clearBreakPoint(const AddressInterval&) override;
    virtual void clearBreakPoints() override;
    virtual void singleStep(ThreadId) override;
    virtual void runToBreakPoint(ThreadId) override;
    virtual Sawyer::Container::BitVector readRegister(ThreadId, RegisterDescriptor) override;
    virtual std::vector<RegisterDescriptor> availableRegisters() override;
    virtual void writeRegister(ThreadId, RegisterDescriptor, const Sawyer::Container::BitVector&) override;
    virtual void writeRegister(ThreadId, RegisterDescriptor, uint64_t value) override;
    virtual size_t readMemory(Address va, size_t nBytes, uint8_t *buffer) override;
    virtual std::vector<uint8_t> readMemory(Address va, size_t nBytes) override;
    virtual Sawyer::Container::BitVector readMemory(Address va, size_t nBytes, ByteOrder::Endianness order) override;
    virtual size_t writeMemory(Address va, size_t nBytes, const uint8_t *bytes) override;
    virtual bool isTerminated() override;
    virtual std::string howTerminated() override;
    virtual Sawyer::Container::BitVector readAllRegisters(ThreadId) override;
    virtual void writeAllRegisters(ThreadId, const Sawyer::Container::BitVector&) override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Supporting functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    // Read GDB's multi-line response to a command
    std::list<GdbResponse> readResponseSet(bool required);

    // Find the index for the GDB register whose major and minor numbers match the specified register.
    Sawyer::Optional<size_t> findRegisterIndex(RegisterDescriptor) const;

    // Find the register descriptor for the GDB register whose major and minor numbers match the specified register. Returns
    // an empty descriptor if not found.
    RegisterDescriptor findRegister(RegisterDescriptor) const;

    // True if GDB should handle the break points; false if ROSE should handle the break points.
    bool gdbHandlesBreakPoints() const;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
