#ifndef ROSE_BinaryAnalysis_SerialIo_H
#define ROSE_BinaryAnalysis_SerialIo_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <ROSE_UNUSED.h>
#include <Rose/BinaryAnalysis/BasicTypes.h>
#include <Rose/BinaryAnalysis/Serialization/SerialFrame.h>
#include <Rose/Exception.h>
#include <Rose/Progress.h>

#include <Sawyer/Message.h>
#include <Sawyer/ProgressBar.h>
#include <Sawyer/Synchronization.h>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <functional>
#include <memory>
#include <vector>
#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#endif

namespace Rose {
namespace BinaryAnalysis {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SerialIo
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Extension of the base SerialIo class defined in BasicTypes.h
 *
 *  A @ref SerialIo object writes analysis results to a file, or initializes analysis results from a file. It handles
 *  such things as not storing the same object twice when referenced by different pointers, storing derived objects
 * through base-class pointers, and providing progress reports.
 *
 *  The file in which the state is stored is accessed sequentially, making it suitable to send output to a stream or
 * read from a stream.  This also means that most of the interface for these objects has no need to be thread-safe,
 * although the progress-reporting part of the API is thread-safe.
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
 *  P2::Partitioner::Ptr partitioner = ....;
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
class SerialIo {
  public:
    /** Reference counting pointer. */
    using Ptr = SerialIoPtr;

    class Deserializer;
    class Serializer;

    struct SerializationRegistration {
        Serialization::Format                          format = Serialization::BINARY;
        std::function<std::shared_ptr<Deserializer>()> deserializer;
        std::function<std::shared_ptr<Serializer>()>   serializer;
    };

    static void                                              registerSerialization(SerializationRegistration);
    static const Sawyer::Optional<SerializationRegistration> findSerialization(Serialization::Format);

  private:
    mutable SAWYER_THREAD_TRAITS::Mutex mutex_; // protects the following data members
    Serialization::Format               format_;
    Progress::Ptr                       progress_;
    bool                                isOpen_;
    Serialization::Savable              objectType_;

  protected:
    Sawyer::ProgressBar<size_t> progressBar_;
    int                         fd_; // Used by SerialFrame

  protected:
    SerialIo() :
      format_(Serialization::BINARY), progress_(Progress::instance()), isOpen_(false),
      objectType_(Serialization::NO_OBJECT), progressBar_(mlog[Sawyer::Message::MARCH]), fd_(-1) {
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
    static Serialization::Savable userSavable(unsigned offset);

    /** Property: File format.
     *
     *  This property specifies the file format of the data. It can only be set for output, and only before the output
     *  file is opened.
     *
     *  Thread safety: This method is thread-safe.
     *
     * @{ */
    Serialization::Format format() const;
    void                  format(Serialization::Format);
    /** @} */

    /** Property: Progress reporter.
     *
     *  A progress reporting object can be specified in which case the I/O operations will update this object. I/O
     * objects are created with an initial progress reporter, but this can be changed at any time.
     *
     * Thread safety: This method is thread-safe.
     *
     * @{ */
    Progress::Ptr progress() const;
    void          progress(const Progress::Ptr&);
    /** @} */

    /** Attach a file.
     *
     *  When opening an output stream, the file is created or truncated; when opening an input stream the file must
     * already exist. If a file is already attached, then the previous file is closed first before this new one is
     * opened.
     *
     *  Throws an @ref Exception if the file cannot be attached or the previous if any, cannot be closed.
     *
     *  Thread safety: This method is not thread-safe. */
    virtual void open(const boost::filesystem::path&) = 0;

    /** Detach a file.
     *
     *  If a file is attached to this I/O object, that file is closed and this object is set to its detached state. This
     * is a no-op if no file is attached.
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
    Serialization::Savable objectType() const;

  protected:
    // Set or clear the isOpen flag.
    void setIsOpen(bool b);

    // Set object type to ERROR to indicate that a read was unsuccessful
    void objectType(Serialization::Savable);

  private:
    void init();

  protected:
    /** Create a progress callback that wraps the IO progress reporting.
     *
     * @param phase The name of the phase for progress reporting
     * @return A callback function that reports progress to this IO object
     */
    Serialization::ProgressCallback makeProgressCallback(const char* phase);

    /* Legacy declarations for compatibility */
  public:
    using Format                   = Serialization::Format;
    static constexpr Format BINARY = Serialization::BINARY;
    static constexpr Format TEXT   = Serialization::TEXT;
    static constexpr Format XML    = Serialization::XML;
};

/** Interface for serializer implementations.
 *
 * This interface is payload-based rather than fd-based. It converts objects to
 * binary payloads (std::vector<char>) that can be stored in the container format.
 */
class SerialIo::Serializer {
  public:
    virtual ~Serializer() = default;

    /** Serialize a partitioner to a binary payload.
     *
     * @param partitioner The partitioner to serialize
     * @param progress A callback function to report progress during serialization
     * @return A vector containing the serialized partitioner data
     */
    virtual std::vector<char>
    savePartitioner(const Partitioner2::PartitionerConstPtr& partitioner, Serialization::ProgressCallback progress) = 0;
};

/** Interface for deserializer implementations.
 *
 * This interface is payload-based rather than fd-based. It converts binary
 * payloads (std::vector<char>) back into objects.
 */
class SerialIo::Deserializer {
  public:
    virtual ~Deserializer() = default;

    /** Deserialize a partitioner from a binary payload.
     *
     * @param data Pointer to the serialized data
     * @param size Size of the serialized data in bytes
     * @param progress A callback function to report progress during deserialization
     * @return The deserialized partitioner object
     */
    virtual Partitioner2::PartitionerPtr
    loadPartitioner(const std::vector<char>& data, Serialization::ProgressCallback progress) = 0;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SerialOutput
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Output binary analysis state.
 *
 *  Writes binary analysis state to a file that can be read later to re-initialize ROSE to the same state. */
class SerialOutput: public SerialIo {
  public:
    /** Reference counting pointer. */
    using Ptr = SerialOutputPtr;

  private:
    std::shared_ptr<Serialization::SerialFrame> frame_;
    std::shared_ptr<SerialIo::Serializer>       serializer_;

  protected:
    SerialOutput() {}

  public:
    ~SerialOutput();
    void open(const boost::filesystem::path& fileName) override;
    void close() override;

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
    void savePartitioner(const Partitioner2::PartitionerConstPtr&);

    /** Save a binary AST.
     *
     *  Saves the entire AST rooted at the specified node. Due to elimination of duplicate objects, saving the same tree
     *  (or any subtree) twice will have negligible effect on the output file size.
     *
     *  Throws an @ref Exception if the AST cannot be saved. It is permissible to pass a null pointer, in which case the
     *  null pointer is saved and can be read back later.
     *
     *  Thread safety: This method is not thread-safe. Furthermore, it temporarily sets the parent pointer of the
     * specified AST node to null and thus may interfere with threads that reading this part of the AST.  It is unsafe
     * to invoke this method while other threads are modifying this part of the AST since AST modification is not thread
     * safe. */
    void saveAst(SgAsmNode*);
    void saveAst(SgBinaryComposite*);

  private:
    // The saveAstHelper is what actually gets called for the functions above. It doesn't descriminate between nodes
    // that support serialization and those that don't, which is why it's private. Use only the public functions because
    // they'll give you a nice compiler error if you try to save an Ast node type that isn't supported.
    void saveAstHelper(SgNode*);

  public:
    /** Save an object to the output stream.
     *
     *  The @p objectTypeId and corresponding @p object are written to the output stream. The object must either
     * implement the Boost serialization interface or be understood by the Boost serialization interface.  The @p
     * objectTypeId is used to identify the object type when reading the archive later since the object type must be
     * known before it can be read.
     *
     *  If the @p object is a pointer, then the object is not actually written a second time if the same object has been
     *  written previously. Most objects recursively write all contained objects, and the elision of previously written
     *  objects becomes important in this case.  When reading object later, the input stream will appear to contain both
     *  objects even if they're the same pointer, and will return the same pointer for both objects. I.e., object
     * sharing during output will result in object sharing during input.
     *
     *  Throws an @ref Exception if any errors occur. */
    template <class T> void saveObject(Serialization::Savable objectTypeId, const T& object) {
        if (!isOpen())
            throw Exception("cannot save object when no file is open");
        if (Serialization::ERROR == objectType())
            throw Exception("cannot save object because stream is in error state");
        if (!frame_)
            throw Exception("frame container not initialized");
        if (!serializer_)
            throw Exception("serializer not initialized");

        // Save the object in a different thread and update the progress in this thread
        std::string                 errorMessage;
        boost::thread               worker(startWorker<T>, this, objectTypeId, object, errorMessage);
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
    }

  private:
    template <class T>
    static void
    startWorker(SerialOutput* saver, Serialization::Savable objectTypeId, const T& object, std::string& errorMessage) {
        saver->saveWorker(objectTypeId, object, errorMessage);
    }

    template <class T>
    void saveWorker(Serialization::Savable objectTypeId, const T& object, std::string& errorMessage) {
        try {
            // Create a FrameRecord with appropriate metadata
            Serialization::FrameRecord frameRecord(objectTypeId, format());

            // Serialize the object into the frameRecord's payload
            frameRecord.serializeObject(object);

            // Write the frame record
            frame_->writeFrameRecord(frameRecord);
            this->objectType(objectTypeId);
        } catch (const std::exception& e) {
            errorMessage = std::string("Failed to save object using container: ") + e.what();
        } catch (...) {
            errorMessage = "failed to write object to output stream";
        }
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
    /** Reference counting pointer. */
    using Ptr = SerialInputPtr;

  private:
    std::shared_ptr<Serialization::SerialFrame> frame_;
    std::shared_ptr<SerialIo::Deserializer>     deserializer_;

  protected:
    SerialInput() {}

  public:
    ~SerialInput();
    void open(const boost::filesystem::path& fileName) override;
    void close() override;

    /** Factory method to create a new instance.
     *
     * The returned instance is in a detached state, therefore the @ref open method needs to be called before any I/O
     * operation can be invoked. */
    static Ptr instance() { return Ptr(new SerialInput); }

    /** Read a frame record from the container and validate its metadata.
     *
     * @param expectedType The expected object type for the record
     * @return The validated frame record
     * @throws Exception if the record's metadata doesn't match expectations
     */
    Serialization::FrameRecord readAndValidateRecord(Serialization::Savable expectedType);

    /** Type of next object in the input stream.
     *
     *  Returns an indication for the type of the next item in the input stream.
     *
     * Throws an @ref Exception if no file is attached.
     *
     * Thread safety: This method is not thread safe. */
    Serialization::Savable nextObjectType();

    /** Load a partitioner from the input stream.
     *
     *  Initializes the specified partitioner with data from the input stream.
     *
     *  Throws an @ref Exception if no file is attached to this I/O object or if the next object to be read from the
     *  input is not a partitioner, or if any other errors occur while reading the partitioner. */
    Partitioner2::PartitionerPtr loadPartitioner();

    /** Load an AST from the input stream.
     *
     *  Loads an AST from the input stream and returns a pointer to its root. If a null AST was stored, then a null
     *  pointer is returned.
     *
     * Throws an @ref Exception if no file is attached to this I/O object or if the next object to be read from the
     * input is not an AST, or if any other errors occur while reading the AST. */
    SgNode* loadAst();

    /** Load an object from the input stream.
     *
     *  An object with the specified tag must exist as the next item in the stream. Such an object is created,
     * initialized from the stream, and returned.  If an object is provided as the second argument, then it's
     * initialized from the stream.
     *
     * @{ */
    template <class T> T loadObject(Serialization::Savable objectTypeId) {
        if (!deserializer_)
            throw Exception("serialization backend not initialized");

        T object;
        loadObject<T>(objectTypeId, object);
        return object;
    }
    template <class T> void loadObject(Serialization::Savable objectTypeId, T& object) {
        if (!isOpen())
            throw Exception("cannot load object when no file is open");
        if (!frame_)
            throw Exception("frame container not initialized");
        if (!deserializer_)
            throw Exception("deserializer not initialized");

        try {
            // Read and validate the frame record
            auto frameRecord = readAndValidateRecord(objectTypeId);

            // Deserialize object from payload
            frameRecord.deserializeObject(object);

            objectType(Serialization::END_OF_DATA);
        } catch (const boost::archive::archive_exception& e) {
            throw Exception(std::string("Failed to load object from Boost archive: ") + e.what());
        } catch (const std::exception& e) {
            throw Exception(std::string("Failed to load object using container: ") + e.what());
        }
    }
    /** @} */

  protected:
    // Read the next object type from the input stream
    void advanceObjectType();

    // Check ROSE version compatibility
    void checkCompatibility(const std::string& fileVersion);
};

} // namespace BinaryAnalysis
} // namespace Rose

#endif
#endif
