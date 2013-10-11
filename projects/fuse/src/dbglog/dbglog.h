#pragma once

#include <list>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

namespace dbglog {

class printable
{
  public:
  virtual ~printable() {}
  virtual std::string str(std::string indent="")=0;
};

// Initializes the debug sub-system
void initializeDebug(std::string title, std::string workDir);

// Returns a string that contains n tabs
std::string tabs(int n);

class dbgStream;
typedef std::list<std::pair<int, std::list<int> > > location;

// Uniquely identifies a location with the debug information, including the file and region hierarchy
// Anchors can be created in two ways:
// - When their host output locations are reached. In this case the anchor's full file and region location is available
// - Before their host locations are reached (when forward links are created). In this case the anchor's location info
//   is not available at anchor creation time and must be initialized subsequently. We'll need to generate output for
//   links to anchors either before (forward links) or after (backward links) the anchor's output location has been 
//   reached. For backward links we can generate code easily since the anchor's target is known at output generation time.
//   To support forward links we do the following:
//   - When the output location is reached, we emit a piece of code to the script file that register's the location
//     in a data sctructure. There is one anchor script file for the entire output (not one per sub-file) to make it
//     easier to deal with anchors that cross files.
//   - When forward links are created we generate code that looks the link's destination in the data structure. If
//     the destination has already been loaded in the data structure, we go directly there. If the destination has not
//     been loaded, this means that the application generating the debug log has crashed and thus, we shouldn't follow
//     the link.
class anchor
{
  protected:
  static int maxAnchorID;
  int anchorID;
  
  // Associates each anchor with its location (if known). Useful for connecting anchor objects with started out
  // unlocated (e.g. forward links) and then were located when we reached their target. Since there may be multiple
  // copies of the original unlocated anchor running around, these copies won't be automatically updated. However,
  // this map will always keep the latest information.
  // Note: an alternative design would use smart pointers, since it would remove the need for keeping copies of anchors
  //       around since no anchor would ever go out of scope. However, a dependence on boost or C++11 seems too much to add.
  static std::map<int, location> anchorLocs;
  
  // Associates each anchor with a unique anchor ID. Useful for connecting multiple anchors that were created
  // independently but then ended up referring to the same location. We'll record the ID of the first one to reach
  // this location on locAnchorIDs and the others will be able to adjust themselves by adopting this ID.
  static std::map<location, int> locAnchorIDs;
  
  // The debug stream this anchor is associated with
  //dbgStream& myDbg;
  
  // Itentifies this anchor's location in the file and region hierarchies
  location loc;
  
  // Flag that indicates whether we've already reached this anchor's location in the output
  bool located;
  
  public:
  anchor(/*dbgStream& myDbg, */bool located=false);
  anchor(const anchor& that);
  anchor(/*dbgStream& myDbg, */bool located, int anchorID);
  
  void init(bool located);
  
  int getID() const { return anchorID; }
  
  bool isLocated() const { return located; }
  
  protected:
  // If this anchor is unlocated, checks anchorLocs to see if a location has been found and updates this
  // object accordingly.
  void update();
  
  public:
  static anchor noAnchor;
  
  void operator=(const anchor& that);
  
  bool operator==(const anchor& that) const;
  bool operator!=(const anchor& that) const;
  
  bool operator<(const anchor& that) const;
  
  const location& getLocation() const;
  
  // Called when the file location of this anchor has been reached
  void reachedAnchor();
  
  // Returns an <a href> tag that denotes a link to this anchor. Embeds the given text in the link.
  std::string link(std::string text) const;
  
  // Returns an <a href> tag that denotes a link to this anchor, using the default link image, which is followed by the given text.
  std::string linkImg(std::string text="") const;
  
  // Returns the JavaScript code that links to this anchor, which can be embedded on other javascript code.
  std::string getLinkJS() const;
    
  std::string str(std::string indent="") const;
};

// A block out debug output, which may be filled by various visual elements
class block
{
  std::string label;
  location    loc;
  std::string fileID;
  std::string blockID;
    
  // The anchor that denotes the starting point of this scope
  anchor startA;
  
  // Set of anchors that point to this block. Once we know this block's location we
  // locate them there.
  std::set<anchor> pointsToAnchors;
  
  protected:
  // Counts the number of times the block constructor has been called
  static int blockCount;
  
  public:
  // Initializes this block with the given label
  block(std::string label);
    
  // Initializes this block with the given label.
  // Includes one or more incoming anchors thas should now be connected to this block.
  block(std::string label, const anchor& pointsTo);
  block(std::string label, const std::set<anchor>& pointsTo);
  
  // Increments block count. This function serves as the one location that we can use to target conditional
  // breakpoints that aim to stop when the block count is a specific number
  int advanceBlockCount();
  
  // Attaches a given un-located anchor at this block
  void attachAnchor(anchor& a);
  
  std::string getLabel() const { return label; }
  const location& getLocation() const { return loc; }
  void setLocation(const location& loc);
  std::string getFileID() const { return fileID; }
  std::string getBlockID() const { return blockID; }
  
  protected:
  anchor& getAnchorRef();
  
  public:
  anchor getAnchor() const;
    
  // Called to notify this block that a sub-block was started/completed inside of it. 
  // Returns true of this notification should be propagated to the blocks 
  // that contain this block and false otherwise.
  virtual bool subBlockEnterNotify(block* subBlock) { return true; }
  virtual bool subBlockExitNotify (block* subBlock) { return true; }
  
  // Called to enable the block to print its entry and exit text
  virtual void printEntry(std::string loadCmd) {}
  virtual void printExit() {}
};

// Adopted from http://wordaligned.org/articles/cpp-streambufs
// A extension of stream that corresponds to a single file produced by dbglog
class dbgBuf: public std::streambuf
{
  friend class dbgStream;
  // True immediately after a new line
  bool synched;
  // True if the owner dbgStream is writing text and false if the user is
  bool ownerAccess;
  std::streambuf* baseBuf;
  std::list<block*> blocks;

  // The number of observed '<' characters that have not yet been balanced out by '>' characters.
  //      numOpenAngles = 1 means that we're inside an HTML tag
  //      numOpenAngles > 1 implies an error or text inside a comment
  int numOpenAngles;
  
  public:
  int getNumOpenAngles() const { return numOpenAngles; }
  
  protected:

  // The number of divs that have been inserted into the output
  std::list<int> parentDivs;
  // For each div a list of the strings that need to be concatenated to form the indent for each line
  std::list<std::list<std::string> > indents;

  // Flag that indicates that a new line has begun and thus before the next printed character
  // we need to print the indent
  bool needIndent;
public:
        
  virtual ~dbgBuf() {};
  // Construct a streambuf which tees output to both input
  // streambufs.
  dbgBuf();
  dbgBuf(std::streambuf* baseBuf);
  void init(std::streambuf* baseBuf);
  
private:
  // This dbgBuf has no buffer. So every character "overflows"
  // and can be put directly into the teed buffers.
  virtual int overflow(int c);

  // Prints the indent to the stream buffer, returns 0 on success non-0 on failure
  //int printIndent();

  // Prints the given string to the stream buffer
  int printString(std::string s);

  //virtual int sputc(char c);

  // Get the current indentation depth within the current div
  int getIndentDepth();
  // Get the current indentation within the current div
  std::string getIndent();
  // Add more indentation within the current div
  void addIndent(std::string indent);
  // Remove the most recently added indent within the current div
  void remIndent();

  virtual std::streamsize xsputn(const char * s, std::streamsize n);

  // Sync buffer.
  virtual int sync();
        
  // Switch between the owner class and user code writing text
protected:
  void userAccessing();
  void ownerAccessing();

  // Called when a block is entered.
  void enterBlock(block* b);
  // Called when a block is exited. Returns the block that was exited.
  block* exitBlock();
  
  // Returns the depth of enterBlock calls that have not yet been matched by exitBlock calls
  int blockDepth();
};


// Stream that uses dbgBuf
class dbgStream : public std::ostream
{
  std::list<std::ofstream*> indexFiles;
  std::list<std::ofstream*> dbgFiles;
  std::list<std::string>    detailFileRelFNames; // Relative names of all the dbg files on the stack
  std::list<std::ofstream*> summaryFiles;
  std::list<std::ofstream*> scriptFiles;
  // The directories in which different widgets store their output. Created upon request by different widgets.
  std::set<std::string>     widgetDirs;
  // Global script file that includes any additional scripts required by widgets 
  std::ofstream             scriptIncludesFile;
  // Records the paths of the scripts that have already been included. Maps script paths to their types.
  std::map<std::string, std::string> includedScripts;
  // Records the paths of the files/directories that have been included/copied into the generated output
  std::set<std::string>     includedFiles;
  // Number of anchor IDs to store in a single anchor script file
  int                       anchorsPerScriptFile;
  public:
  int getAnchorsPerScriptFile() const { return anchorsPerScriptFile; }
  private:
  std::list<int>            fileLevel;
  location                  loc;
  // All the blocks within the current location. The top-level list denotes files.
  // Each pair within it is the block in the higher-level file that contains the 
  // lower-level file as well as the list of blocks in the lower-level file. 
  std::list<std::pair<block*, std::list<block*> > > blocks;
  std::list<dbgBuf*>        fileBufs;
  // The scopes inside which sub-files are loaded
  std::list<block*>         fileBlocks;
  
  dbgBuf defaultFileBuf;
  
  // The title of the file
  std::string title;
  // The root working directory
  std::string workDir;
  // The directory where all images will be stored
  std::string imgDir;
  // The directory that widgets can use as temporary scratch space
  std::string tmpDir;
  // The total number of images in the output file
  int numImages;
  
  bool initialized;
  
public:
  // Construct an ostream which tees output to the supplied
  // ostreams.
  dbgStream();
  dbgStream(std::string title, std::string workDir, std::string imgDir, std::string tmpDir);
  void init(std::string title, std::string workDir, std::string imgDir, std::string tmpDir);
  ~dbgStream();

  // Switch between the owner class and user code writing text into this stream
  void userAccessing();
  void ownerAccessing();
  
  // Return the root working directory
  const std::string& getWorkDir() const { return workDir; }
  // Return the directory where all images will be stored
  const std::string& getImgDir() const { return imgDir; }
  // Return the directory that widgets can use as temporary scratch space
  const std::string& getTmpDir() const { return tmpDir; }
    
  // Creates an output directory for the given widget and returns its path as a pair:
  // <path relative to the current working directory that can be used to create paths for writing files,
  //  path relative to the output directory that can be used inside generated HTML>
  std::pair<std::string, std::string> createWidgetDir(std::string widgetName);
  
  // Add an include of the given script in the generated HTML output. There are generic scripts that will be
  // included in every output document that is loaded. The path of the script must be relative
  // to the output HTML directory
  void includeWidgetScript(std::string scriptPath, std::string scriptType="text/javascript");
  
  // Adds the given JavaScript command text to the script that will be loaded with the current file.
  // This command is guaranteed to run after the body of the file is loaded but before the anchors
  // referentsare loaded.
  void widgetScriptCommand(std::string command);
  
  // Includes the given file or directory into the generated HTML output by copying it from its absolute
  // path to the given relative path within the output directory
  void includeFile(std::string path);

  // Returns the string representation of the current file level
  std::string fileLevelStr(const location& myLoc) const;
  
  // Returns a string that encodes a Javascript array of integers that together form the unique ID string of 
  // the current file within the hierarchy of files
  std::string fileLevelJSIntArray(const location& myLoc) const;
  
  ///const std::list<int>& getFileLevel() const;
  const location& getLocation() const;
    
  // Returns the unique ID string of the current region, including all the files that it may be contained in
  static std::string blockGlobalStr(const location& myLoc);

  // Given two location objects, returns the longest prefix location that is common to both of them
  static location commonSubLocation(const location& a, const location& b);
  
  // Called to inform the blocks that contain the given block that it has been entered
  void subBlockEnterNotify(block* subBlock);
  
  // Called to inform the blocks that contain the given block that it has been exited
  void subBlockExitNotify(block* subBlock);
  
  // Returns the depth of enterBlock calls that have not yet been matched by exitBlock calls within the current file
  int blockDepth() const;
  // Index of the current block in the current fil
  int blockIndex() const;

  // Enter a new file level. Return a string that contains the JavaScript command to open this file in the current view.
  std::string enterFileLevel(block* b, bool topLevel=false);
  
  // Exit a current file level
  block* exitFileLevel(bool topLevel=false);
    
  // Record the mapping from the given anchor ID to the given string in the global script file
  void writeToAnchorScript(int anchorID, const location& myLoc);

  void printSummaryFileContainerHTML(std::string absoluteFileName, std::string relativeFileName, std::string title);
  void printDetailFileContainerHTML(std::string absoluteFileName, std::string title);
  
  // Called when a block is entered.
  std::string enterBlock(block* b, bool newFileEntered);//, std::string fileIDJSArray="");//std::string detailContentURL="", std::string summaryContentURL="");
    // Called when a block is exited. Returns the block that was exited.
  block* exitBlock();

  // Adds an image to the output with the given extension and returns the path of this image
  // so that the caller can write to it.
  std::string addImage(std::string ext=".gif");
  
  // Add a given amount of indent space to all subsequent text within the current function
  void addIndent(std::string indent);
  // Remove the most recently added indent
  void remIndent();
}; // dbgStream

extern bool initializedDebug;
extern dbgStream dbg;

class indent {
public:
  bool active;
  indent(std::string space, int curDebugLevel=0, int targetDebugLevel=0);
  indent(int curDebugLevel=0, int targetDebugLevel=0);
  ~indent();
};

// Class that makes it possible to generate string labels by using the << syntax.
// Examples: Label() << "a=" << (5+2) << "!"
//           Label("a=") << (5+2) << "!"
struct txt : std::string {
  txt() {}
  txt(const std::string& initTxt) {
    _stream << initTxt;
    assign(_stream.str());
  }
  
  template <typename T>
  txt& operator<<(T const& t) {
    _stream << t;
    assign(_stream.str());
    return *this;
  }

  std::string str() const { return _stream.str(); }
  std::ostringstream _stream;
};

// Given a string, returns a version of the string with all the control characters that may appear in the 
// string escaped to that the string can be written out to Dbg::dbg with no formatting issues.
// This function can be called on text that has already been escaped with no harm.
std::string escape(std::string s);
} // namespace dbglog

