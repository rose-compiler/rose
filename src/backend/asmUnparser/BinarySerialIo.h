#ifndef Rose_BinaryAnalysis_SerialIo_H
#define Rose_BinaryAnalysis_SerialIo_H

#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

#include <Progress.h>
#include <RoseException.h>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>
#include <Sawyer/Message.h>
#include <Sawyer/ProgressBar.h>
#include <Sawyer/Synchronization.h>

// Define this if you need to debug SerialIo -- it causes everything to run in the calling thread and avoid catching exceptions.
//#define ROSE_DEBUG_SERIAL_IO

#if defined(BOOST_WINDOWS)
    // Lacks POSIX file system, so we can't monitor the I/O progress
    #undef ROSE_SUPPORTS_SERIAL_IO
#elif !defined(ROSE_HAVE_BOOST_SERIALIZATION_LIB)
    // Lacks Boost's serialization library, which is how we convert objects to bytes and vice versa
    #undef ROSE_SUPPORTS_SERIAL_IO
#elif defined(__clang__)
    #define ROSE_SUPPORTS_SERIAL_IO /*supported*/
#elif defined(__GNUC__)
    #if __GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNU_C_PATCHLEVEL__ <= 40204
        // GCC <= 4.2.4 gets segfaults compiling this file
        #undef ROSE_SUPPORTS_SERIAL_IO
    #else
        #define ROSE_SUPPORTS_SERIAL_IO /*supported*/
    #endif
#else
    #define ROSE_SUPPORTS_SERIAL_IO /*supported*/
#endif

#ifdef ROSE_SUPPORTS_SERIAL_IO
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>
#endif

namespace Rose {
namespace BinaryAnalysis {

namespace Partitioner2 {
class Partitioner;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SerialIo
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Base class for binary state input and output.
 *
 *  A @ref SerialIo object writes analysis results to a file, or initializes analysis results from a file. It handles
 *  such things as not storing the same object twice when referenced by different pointers, storing derived objects through
 *  base-class pointers, and providing progress reports.
 *
 *  The file in which the state is stored is accessed sequentially, making it suitable to send output to a stream or read
 *  from a stream.  This also means that most of the interface for these objects has no need to be thread-safe, although
 *  the progress-reporting part of the API is thread-safe.
 *
 *  As objects are written to the output stream, they are each preceded by a object type identifier. These integer type
 *  identifiers are available when reading from the stream in order to decide which type of object to read next.
 *
 *  I/O errors are reported by throwing an @ref Exception. Errors thrown by underlying layers, such as Boost, are caught
 *  and rethrown as @ref Exception in order to simplify this interface.
 *
 *  Here's an example of how to write a partitioner followed by a vector of doubles to a state file using XML format:
 *
 * @code
 *  using namespace Rose::BinaryAnalysis;
 *  namespace P2 = Rose::BinaryAnalysis::Partitioner2;
 *  std::vector<double> myVector = ...;
 *  const SerialIo::Savable myVectorTypeId = SerialIo::USER_DEFINED; // maybe add some constant
 *  P2::Partitioner partitioner = ....;
 *  boost::filesystem::path fileName = "....";
 *  SerialOutput::Ptr saver = SerialOutput::instance();
 *
 *  try {
 *      saver->mlog[INFO] <<"saving state to " <<fileName;
 *      saver->format(SerialIo::XML);
 *      saver->open(fileName);
 *      saver->savePartitioner(partitioner);
 *      saver->saveObject(myVectorTypeId, myVector);
 *      saver->close();
 *      saver->mlog[INFO] <<"; saved\n";
 *  } catch (const SerialIo::Exception &e) {
 *      saver->mlog[ERROR] <<e.what() <<"\n";
 *  }
 * @endcode
 *
 *  Reading is similar:
 *
 * @code
 *  SerialInput::Ptr loader = SerialInput::instance();
 *  try {
 *      loader->mlog[INFO] <<"loading state from " <<fileName;
 *      loader->format(SerialIo::XML);
 *      loader->open(fileName);
 *      partitioner = loader->loadPartitioner();
 *      myVector = loader->loadObject<std::vector<double> >(myVectorTypeId);
 *      loader->close();
 *      loader->mlog[INFO] <<"; loaded\n";
 *  } catch (const SerialIo::Exception &e) {
 *      loader->mlog[ERROR] <<e.what() <<"\n";
 *  }
 * @endcode */
class SerialIo: public Sawyer::SharedObject {
public:
    /** Reference-counting pointer. */
    typedef Sawyer::SharedPointer<SerialIo> Ptr;

    /** Format of the state file. */
    enum Format {
        BINARY,         /**< Binary state files are smaller and faster than the other formats, but are not portable across
                         *   architectures. */
        TEXT,           /**< Textual binary state files use a custom format (Boost serialization format) that stores the
                         *   data as ASCII text. They are larger and slower than binary files but not as large and slow
                         *   as XML or JSON files. They are portable across architectures. */
        XML             /**< The states are stored as XML, which is a very verbose and slow format. Avoid using this if
                         *   possible. */
    };

    /** Types of objects that can be saved. */
    enum Savable {
        NO_OBJECT           = 0x00000000, /**< Object type for newly-initialized serializers. */
        PARTITIONER         = 0x00000001, /**< Rose::BinaryAnalysis::Partitioner2::Partitioner. */
        AST                 = 0x00000002, /**< Abstract syntax tree. */
        END_OF_DATA         = 0x0000fffe, /**< Marks the end of the data stream. */
        ERROR               = 0x0000ffff, /**< Marks that the stream has encountered an error condition. */
        USER_DEFINED        = 0x00010000, /**< First user-defined object number. */
        USER_DEFINED_LAST   = 0xffffffff  /**< Last user-defined object number. */
    };

    /** Errors thrown by this API. */
    class Exception: public Rose::Exception {
    public:
        /** Construct an exception with an error message. */
        explicit Exception(const std::string &s): Rose::Exception(s) {}
        ~Exception() throw() {}
    };

private:
    mutable SAWYER_THREAD_TRAITS::Mutex mutex_; // protects the following data members
    Format format_;
    Progress::Ptr progress_;
    bool isOpen_;
    Savable objectType_;

protected:
    Sawyer::ProgressBar<size_t> progressBar_;

    // We use low-level file descriptors under an std::stream I/O interface in order to provide progress reports. This
    // allows one thread to be reading from or writing to the file and another thread to monitor the file position to
    // report progress. The C++ standard library doesn't have an API for wrapping file descriptors in a stream interface,
    // so we use Boost.
    int fd_;

protected:
    SerialIo()
        : format_(BINARY), progress_(Progress::instance()), isOpen_(false), objectType_(NO_OBJECT),
          progressBar_(mlog[Sawyer::Message::MARCH]), fd_(-1) {
        init();
        progressBar_.suffix(" bytes");
    }

public:
    /** Message facility. */
    static Sawyer::Message::Facility mlog;

    /** Destructor.
     *
     *  Since I/O objects are reference counted, the user should not invoke the destructor explicitly; it will be
     *  called automatically when the last reference to this object goes away.
     *
     *  The destructor closes any attached file but does not throw exceptions. Therefore, it is better to always
     *  explicitly @ref close an I/O object before destroying it. */
    virtual ~SerialIo();

    /** Create a new Savable enum constant.
     *
     *  This is a convenience function to create a new Savable constant which is an @p offset from USER_DEFINED. */
    static Savable userSavable(unsigned offset);

    /** Property: File format.
     *
     *  This property specifies the file format of the data. It can only be set for output, and only before the output
     *  file is opened.
     *
     *  Thread safety: This method is thread-safe.
     *
     * @{ */
    Format format() const;
    void format(Format);
    /** @} */

    /** Property: Progress reporter.
     *
     *  A progress reporting object can be specified in which case the I/O operations will update this object. I/O objects are
     * created with an initial progress reporter, but this can be changed at any time.
     *
     * Thread safety: This method is thread-safe.
     *
     * @{ */
    Progress::Ptr progress() const;
    void progress(const Progress::Ptr&);
    /** @} */

    /** Attach a file.
     *
     *  When opening an output stream, the file is created or truncated; when opening an input stream the file must already
     *  exist. If a file is already attached, then the previous file is closed first before this new one is opened.
     *
     *  Throws an @ref Exception if the file cannot be attached or the previous if any, cannot be closed.
     *
     *  Thread safety: This method is not thread-safe. */
    virtual void open(const boost::filesystem::path&) = 0;

    /** Detach a file.
     *
     *  If a file is attached to this I/O object, that file is closed and this object is set to its detached state. This is
     *  a no-op if no file is attached.
     *
     *  Throws an @ref Exception if the file cannot be detached.
     *
     *  The @ref close method is automatically called during object destruction, although its exceptions are suppressed
     *  in that situation.
     *
     *  Thread safety: This method is not thread-safe. */
    virtual void close() = 0;

    /** Whether a file is attached.
     *
     * Returns true if this I/O object is attached to a file.
     *
     * Thread safety: This method is thread-safe. */
    bool isOpen() const;

    /** Type ID for next object.
     *
     *  When using an input stream, this returns the type ID for the next object to be read from the stream. When using
     *  an output stream, it's the type of the object that was last written. */
    Savable objectType() const;

protected:
    // Set or clear the isOpen flag.
    void setIsOpen(bool b);

    // Set object type to ERROR to indicate that a read was unsuccessful
    void objectType(Savable);

private:
    void init();
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SerialOutput
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Output binary analysis state.
 *
 *  Writes binary analysis state to a file that can be read later to re-initialize ROSE to the same state. */
class SerialOutput: public SerialIo {
public:
    typedef Sawyer::SharedPointer<SerialOutput> Ptr;

private:
#ifdef ROSE_SUPPORTS_SERIAL_IO
    boost::iostreams::file_descriptor_sink device_;
    boost::iostreams::stream<boost::iostreams::file_descriptor_sink> file_;
    boost::archive::binary_oarchive *binary_archive_;
    boost::archive::text_oarchive *text_archive_;
    boost::archive::xml_oarchive *xml_archive_;
#endif

protected:
#ifdef ROSE_SUPPORTS_SERIAL_IO
    SerialOutput(): binary_archive_(NULL), text_archive_(NULL), xml_archive_(NULL) {}
#else
    SerialOutput() {}
#endif

public:
    ~SerialOutput();
    void open(const boost::filesystem::path &fileName) ROSE_OVERRIDE;
    void close() ROSE_OVERRIDE;

    /** Factory method to create a new instance.
     *
     *  The returned instance is in a detached state, therefore the @ref open method needs to be called before any I/O
     *  operations can be invoked. */
    static Ptr instance() { return Ptr(new SerialOutput); }

    /** Save a binary analysis partitioner.
     *
     *  The specified partitioner, including all data reachable from the partitioner such as specimen data, instruction
     *  ASTs, and analysis results, is written to the attached file.
     *
     *  Throws an @ref Exception if the partitioner cannot be saved.
     *
     *  Thread safety: This method is not thread-safe. No other thread should be using this object, and no other thread
     *  should be modifying the partitioner. */
    void savePartitioner(const Partitioner2::Partitioner&);

    /** Save a binary AST.
     *
     *  Saves the entire AST rooted at the specified node. Due to elimination of duplicate objects, saving the same tree
     *  (or any subtree) twice will have negligible effect on the output file size.
     *
     *  Throws an @ref Exception if the AST cannot be saved. It is permissible to pass a null pointer, in which case the
     *  null pointer is saved and can be read back later.
     *
     *  Thread safety: This method is not thread-safe. Furthermore, it temporarily sets the parent pointer of the specified
     *  AST node to null and thus may interfere with threads that reading this part of the AST.  It is unsafe to invoke this
     *  method while other threads are modifying this part of the AST since AST modification is not thread safe. */
    void saveAst(SgAsmNode*);
    void saveAst(SgBinaryComposite*);
private:
    // The saveAstHelper is what actually gets called for the functions above. It doesn't descriminate between nodes that
    // support serialization and those that don't, which is why it's private. Use only the public functions because they'll
    // give you a nice compiler error if you try to save an Ast node type that isn't supported.
    void saveAstHelper(SgNode*);
public:

    /** Save an object to the output stream.
     *
     *  The @p objectTypeId and corresponding @p object are written to the output stream. The object must either implement
     *  the Boost serialization interface or be understood by the Boost serialization interface.  The @p objectTypeId is
     *  used to identify the object type when reading the archive later since the object type must be known before it can
     *  be read.
     *
     *  If the @p object is a pointer, then the object is not actually written a second time if the same object has been
     *  written previously. Most objects recursively write all contained objects, and the elision of previously written
     *  objects becomes important in this case.  When reading object later, the input stream will appear to contain both
     *  objects even if they're the same pointer, and will return the same pointer for both objects. I.e., object sharing
     *  during output will result in object sharing during input.
     *
     *  Throws an @ref Exception if any errors occur. */
    template<class T>
    void saveObject(Savable objectTypeId, const T &object) {
        if (!isOpen())
            throw Exception("cannot save object when no file is open");
        if (ERROR == objectType())
            throw Exception("cannot save object because stream is in error state");

#ifndef ROSE_SUPPORTS_SERIAL_IO
        throw Exception("binary state files are not supported in this configuration");
#elif defined(ROSE_DEBUG_SERIAL_IO)
        std::string errorMessage;
        asyncSave(objectTypeId, object, &errorMessage);
#else
        // A different thread saves the object while this thread updates the progress
        std::string errorMessage;
        boost::thread worker(startWorker<T>, this, objectTypeId, &object, &errorMessage);
        boost::chrono::milliseconds timeout((unsigned)(1000 * Sawyer::ProgressBarSettings::minimumUpdateInterval()));
        progressBar_.prefix("writing");
        while (!worker.try_join_for(timeout)) {
            off_t cur = ::lseek(fd_, 0, SEEK_CUR);
            if (-1 == cur) {
                ++progressBar_; // so a spinner moves
            } else {
                progressBar_.value(cur);
                if (Progress::Ptr p = progress())
                    p->update(Progress::Report(cur, NAN));
            }
        }
        if (!errorMessage.empty())
            throw Exception(errorMessage);
#endif
    }

private:
    template<class T>
    static void startWorker(SerialOutput *saver, Savable objectTypeId, const T *object, std::string *errorMessage) {
        ASSERT_not_null(object);
        saver->asyncSave(objectTypeId, *object, errorMessage);
    }

    // This might run in its own thread.
    template<class T>
    void asyncSave(Savable objectTypeId, const T &object, std::string *errorMessage) {
        ASSERT_not_null(errorMessage);
#ifndef ROSE_SUPPORTS_SERIAL_IO
        ASSERT_not_reachable("not supported in this configuration");
#else
#if !defined(ROSE_DEBUG_SERIAL_IO)
        try {
#endif
            objectType(ERROR);
            switch (format()) {
                case BINARY:
                    ASSERT_not_null(binary_archive_);
                    *binary_archive_ <<BOOST_SERIALIZATION_NVP(objectTypeId);
                    *binary_archive_ <<BOOST_SERIALIZATION_NVP(object);
                    break;
                case TEXT:
                    ASSERT_not_null(text_archive_);
                    *text_archive_ <<BOOST_SERIALIZATION_NVP(objectTypeId);
                    *text_archive_ <<BOOST_SERIALIZATION_NVP(object);
                    break;
                case XML:
                    ASSERT_not_null(xml_archive_);
                    *xml_archive_ <<BOOST_SERIALIZATION_NVP(objectTypeId);
                    *xml_archive_ <<BOOST_SERIALIZATION_NVP(object);
                    break;
            }
            objectType(objectTypeId);
#if !defined(ROSE_DEBUG_SERIAL_IO)
        } catch (const Exception &e) {
            *errorMessage = e.what();
        } catch (...) {
            *errorMessage = "failed to write object to output stream";
        }
#endif
#endif
    }
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SerialInput
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Input binary analysis state.
 *
 *  Reads a previously saved binary analysis state file to re-initialize ROSE to a previous state. */
class SerialInput: public SerialIo {
public:
    typedef Sawyer::SharedPointer<SerialInput> Ptr;

private:
#ifdef ROSE_SUPPORTS_SERIAL_IO
    size_t fileSize_;
    boost::iostreams::file_descriptor_source device_;
    boost::iostreams::stream<boost::iostreams::file_descriptor_source> file_;
    boost::archive::binary_iarchive *binary_archive_;
    boost::archive::text_iarchive *text_archive_;
    boost::archive::xml_iarchive *xml_archive_;
#endif

protected:
#ifdef ROSE_SUPPORTS_SERIAL_IO
    SerialInput(): fileSize_(0), binary_archive_(NULL), text_archive_(NULL), xml_archive_(NULL) {}
#else
    SerialInput() {}
#endif

public:
    ~SerialInput();
    void open(const boost::filesystem::path &fileName) ROSE_OVERRIDE;
    void close() ROSE_OVERRIDE;

    /** Factory method to create a new instance.
     *
     * The returned instance is in a detached state, therefore the @ref open method needs to be called before any I/O
     * operation can be invoked. */
    static Ptr instance() { return Ptr(new SerialInput); }

    /** Type of next object in the input stream.
     *
     *  Returns an indication for the type of the next item in the input stream.
     *
     * Throws an @ref Exception if no file is attached.
     *
     * Thread safety: This method is not thread safe. */
    Savable nextObjectType();

    /** Load a partitioner from the input stream.
     *
     *  Initializes the specified partitioner with data from the input stream.
     *
     *  Throws an @ref Exception if no file is attached to this I/O object or if the next object to be read from the
     *  input is not a partitioner, or if any other errors occur while reading the partitioner. */
    Partitioner2::Partitioner loadPartitioner();

    /** Load an AST from the input stream.
     *
     *  Loads an AST from the intput stream and returns a pointer to its root. If a null AST was stored, then a null
     *  pointer is returned.
     *
     * Throws an @ref Exception if no file is attached to this I/O object or if the next object to be read from the
     * input is not an AST, or if any other errors occur while reading the AST. */
    SgNode* loadAst();

    /** Load an object from the input stream.
     *
     *  An object with the specified tag must exist as the next item in the stream. Such an object is created, initialized from
     *  the stream, and returned.  If an object is provided as the second argument, then it's initialized from the stream.
     *
     * @{ */
    template<class T>
    T loadObject(Savable objectTypeId) {
        T object;
        loadObject<T>(objectTypeId, object);
        return object;
    }
    template<class T>
    void loadObject(Savable objectTypeId, T &object) {
        if (!isOpen())
            throw Exception("cannot load object when no file is open");

#ifndef ROSE_SUPPORTS_SERIAL_IO
        throw Exception("binary state files are not supported in this configuration");
#else
        if (ERROR == objectType())
            throw Exception("cannot read object because stream is in error state");
        if (objectType() != objectTypeId) {
            throw Exception("unexpected object type (expected " + boost::lexical_cast<std::string>(objectTypeId) +
                            " but read " + boost::lexical_cast<std::string>(objectType()) + ")");
        }
        objectType(ERROR); // in case of exception
        std::string errorMessage;
#ifdef ROSE_DEBUG_SERIAL_IO
        asyncLoad(object, &errorMessage);
#else
        boost::thread worker(startWorker<T>, this, &object, &errorMessage);
        boost::chrono::milliseconds timeout((unsigned)(1000 * Sawyer::ProgressBarSettings::minimumUpdateInterval()));
        progressBar_.prefix("reading");
        while (!worker.try_join_for(timeout)) {
            if (fileSize_ > 0) {
                off_t cur = ::lseek(fd_, 0, SEEK_CUR);
                if (cur != -1) {
                    progressBar_.value(cur);
                    if (Progress::Ptr p = progress())
                        p->update(Progress::Report(cur, fileSize_));
                }
            } else {
                ++progressBar_; // so the spinner moves
            }
        }
        if (!errorMessage.empty())
            throw Exception(errorMessage);
#endif
        advanceObjectType();
#endif
    }
    /** @} */
        
private:
    template<class T>
    static void startWorker(SerialInput *loader, T *object, std::string *errorMessage) {
        loader->asyncLoad(*object, errorMessage);
    }

    // Might run in its own thread
    template<class T>
    void asyncLoad(T &object, std::string *errorMessage) {
        ASSERT_not_null(errorMessage);
#ifndef ROSE_SUPPORTS_SERIAL_IO
        ASSERT_not_reachable("not supported in this configuration");
#else
#if !defined(ROSE_DEBUG_SERIAL_IO)
        try {
#endif
            switch (format()) {
                case BINARY:
                    ASSERT_not_null(binary_archive_);
                    *binary_archive_ >>object;
                    break;
                case TEXT:
                    ASSERT_not_null(text_archive_);
                    *text_archive_ >>object;
                    break;
                case XML:
                    ASSERT_not_null(xml_archive_);
                    *xml_archive_ >>BOOST_SERIALIZATION_NVP(object);
                    break;
            }
#if !defined(ROSE_DEBUG_SERIAL_IO)
        } catch (const Exception &e) {
            *errorMessage = e.what();
        } catch (...) {
            *errorMessage = "failed to read object from input stream";
        }
#endif
#endif
    }

protected:
    // Read the next object type from the input stream
    void advanceObjectType();
};

} // namespace
} // namespace

#endif
#endif
