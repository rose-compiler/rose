#include <sawyer/Message.h>

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/find.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/config.hpp>
#include <boost/foreach.hpp>
#include <cerrno>
#include <cmath>
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <sawyer/Sawyer.h>
#include <stdexcept>
#include <sstream>
#include <sys/stat.h>
#include <vector>

#ifdef BOOST_WINDOWS
//#   include <stdafx.h>
#   include <windows.h>
#   include <tchar.h>
#   include <psapi.h>
#else
#   include <syslog.h>
#endif

#if defined(SAWYER_HAVE_BOOST_CHRONO)
#   include <boost/chrono.hpp>
#elif defined(BOOST_WINDOWS)
#   include <time.h>
#   include <windows.h>
#   undef ERROR                                         // not sure where this pollution comes from
#   undef max                                           // more pollution
#else // POSIX
#   include <sys/time.h>                                // gettimeofday() and struct timeval
#endif

namespace Sawyer {
namespace Message {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SAWYER_EXPORT std::string
stringifyImportance(Importance importance) {
    switch (importance) {
        case DEBUG: return "DEBUG";
        case TRACE: return "TRACE";
        case WHERE: return "WHERE";
        case MARCH: return "MARCH";
        case INFO:  return "INFO";
        case WARN:  return "WARN";
        case ERROR: return "ERROR";
        case FATAL: return "FATAL";
        case N_IMPORTANCE:
            throw std::runtime_error("invalid message importance");
        //default: DO NOT USE or else we won't get compiler warnings when new importances are defined
    }
    throw std::runtime_error("invalid message importance");
}

SAWYER_EXPORT std::string
stringifyColor(AnsiColor color) {
    switch (color) {
        case COLOR_BLACK:   return "black";
        case COLOR_RED:     return "red";
        case COLOR_GREEN:   return "green";
        case COLOR_YELLOW:  return "yellow";
        case COLOR_BLUE:    return "blue";
        case COLOR_MAGENTA: return "magenta";
        case COLOR_CYAN:    return "cyan";
        case COLOR_WHITE:   return "white";
        case COLOR_DEFAULT: return "default";
    }
    throw std::runtime_error("invalid color");
}

SAWYER_EXPORT std::string
escape(const std::string &s) {
    std::string retval;
    for (size_t i=0; i<s.size(); ++i) {
        switch (s[i]) {
            case '\a': retval += "\\a"; break;
            case '\b': retval += "\\b"; break;
            case '\t': retval += "\\t"; break;
            case '\n': retval += "\\n"; break;
            case '\v': retval += "\\v"; break;
            case '\f': retval += "\\f"; break;
            case '\r': retval += "\\r"; break;
            case '\\': retval += "\\\\"; break;
            default:
                if (isprint(s[i])) {
                    retval += s[i];
                } else {
                    char buf[8];
#include <sawyer/WarningsOff.h>
                    sprintf(buf, "\\%03o", (unsigned)s[i]);
#include <sawyer/WarningsRestore.h>
                    retval += buf;
                }
                break;
        }
    }
    return retval;
}

SAWYER_EXPORT double
now() {
#if defined(SAWYER_HAVE_BOOST_CHRONO)
    boost::chrono::system_clock::time_point curtime = boost::chrono::system_clock::now();
    boost::chrono::system_clock::time_point epoch;
    boost::chrono::duration<double> diff = curtime - epoch;
    return diff.count();
#elif defined(BOOST_WINDOWS)
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    unsigned __int64 t = ft.dwHighDateTime;
    t <<= 32;
    t |= ft.dwLowDateTime;
    t /= 10;                                            // convert into microseconds
    //t -= 11644473600000000Ui64;                       // convert file time to microseconds since Unix epoch
    return t / 1e6;
#else // POSIX
    struct timeval t;
    if (-1==gettimeofday(&t, NULL))
        return 0.0;
    return t.tv_sec + 1e-6 * t.tv_usec;
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SAWYER_EXPORT ColorSet
ColorSet::blackAndWhite() {
    ColorSet cs;
    cs[WARN] = cs[ERROR] = cs[FATAL] = ColorSpec(COLOR_DEFAULT, COLOR_DEFAULT, true);
    return cs;
}

SAWYER_EXPORT ColorSet
ColorSet::fullColor() {
    ColorSet cs;
    cs[DEBUG] = ColorSpec(COLOR_DEFAULT, COLOR_DEFAULT, false);
    cs[TRACE] = ColorSpec(COLOR_CYAN,    COLOR_DEFAULT, false);
    cs[WHERE] = ColorSpec(COLOR_CYAN,    COLOR_DEFAULT, false);
    cs[MARCH] = ColorSpec(COLOR_GREEN,   COLOR_DEFAULT, false);
    cs[INFO]  = ColorSpec(COLOR_GREEN,   COLOR_DEFAULT, false);
    cs[WARN]  = ColorSpec(COLOR_YELLOW,  COLOR_DEFAULT, false);
    cs[ERROR] = ColorSpec(COLOR_RED,     COLOR_DEFAULT, false);
    cs[FATAL] = ColorSpec(COLOR_RED,     COLOR_DEFAULT, true);
    return cs;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SAWYER_EXPORT MesgProps
MesgProps::merge(const MesgProps &other) const {
    MesgProps retval = *this;
    if (!facilityName)
        retval.facilityName = other.facilityName;
    if (!importance)
        retval.importance = other.importance;
    if (indeterminate(isBuffered))
        retval.isBuffered = other.isBuffered;
    if (!completionStr)
        retval.completionStr = other.completionStr;
    if (!interruptionStr)
        retval.interruptionStr = other.interruptionStr;
    if (!cancelationStr)
        retval.cancelationStr = other.cancelationStr;
    if (!lineTermination)
        retval.lineTermination = other.lineTermination;
    if (indeterminate(useColor))
        retval.useColor = other.useColor;
    return retval;
}

SAWYER_EXPORT void
MesgProps::print(std::ostream &o) const {
    o <<"{facilityName=";
    if (facilityName) {
        o <<"\"" <<*facilityName <<"\"";
    } else {
        o <<"undef";
    }

    o <<", importance=";
    if (importance) {
        o <<stringifyImportance(*importance);
    } else {
        o <<"undef";
    }

    o <<", isBuffered=";
    if (!indeterminate(isBuffered)) {
        o <<(isBuffered ? "yes" : "no");
    } else {
        o <<"undef";
    }

    o <<", completionStr=";
    if (completionStr) {
        o <<"\"" <<escape(*completionStr) <<"\"";
    } else {
        o <<"undef";
    }

    o <<", interruptionStr=";
    if (interruptionStr) {
        o <<"\"" <<escape(*interruptionStr) <<"\"";
    } else {
        o <<"undef";
    }

    o <<", cancelationStr=";
    if (cancelationStr) {
        o <<"\"" <<escape(*cancelationStr) <<"\"";
    } else {
        o <<"undef";
    }

    o <<", lineTermination=";
    if (lineTermination) {
        o <<"\"" <<escape(*lineTermination) <<"\"";
    } else {
        o <<"undef";
    }
    
    o <<", useColor=";
    if (!indeterminate(useColor)) {
        o <<(useColor ? "yes" : "no");
    } else {
        o <<"undef";
    }

    o <<"}";
}

SAWYER_EXPORT std::ostream&
operator<<(std::ostream &o, const MesgProps &props) {
    props.print(o);
    return o;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned Mesg::nextId_;

SAWYER_EXPORT void
Mesg::insert(const std::string &s) {
    if (isComplete())
        throw std::runtime_error("cannot add text to a completed message");
    text_ += s;
}

SAWYER_EXPORT void
Mesg::insert(char ch) {
    if (isComplete())
        throw std::runtime_error("cannot add text to a completed message");
    text_ += ch;
}

SAWYER_EXPORT void
Mesg::post(const BakedDestinations &baked) const {
    for (BakedDestinations::const_iterator bi=baked.begin(); bi!=baked.end(); ++bi)
        bi->first->post(*this, bi->second);
}

SAWYER_EXPORT bool
Mesg::hasText() const {
    return boost::find_token(text_, boost::is_graph());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SAWYER_EXPORT void
Destination::bakeDestinations(const MesgProps &props, BakedDestinations &baked) {
    baked.push_back(std::make_pair(sharedFromThis(), mergeProperties(props)));
}

SAWYER_EXPORT MesgProps
Destination::mergeProperties(const MesgProps &props) {
    return overrides_.merge(props.merge(dflts_));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SAWYER_EXPORT void
Multiplexer::bakeDestinations(const MesgProps &props, BakedDestinations &baked) {
    MesgProps downwardProps = mergeProperties(props);
    for (Destinations::const_iterator di=destinations_.begin(); di!=destinations_.end(); ++di)
        (*di)->bakeDestinations(downwardProps, baked);
}

// Multiplexors are never included in baked results (they control baking instead), so messages should never be posted directly
// to multiplexers.
SAWYER_EXPORT void
Multiplexer::post(const Mesg &mesg, const MesgProps &props) {
    assert(!"messages should not be posted to multiplexers");
}

SAWYER_EXPORT MultiplexerPtr
Multiplexer::addDestination(const DestinationPtr &destination) {
    assert(destination!=NULL);

    // Make sure this doesn't introduce a cycle
    std::vector<DestinationPtr> work(1, destination);
    while (!work.empty()) {
        DestinationPtr d = work.back();
        if (getRawPointer(d)==this)
            throw std::runtime_error("cycle introduced in Sawyer::Multiplexer tree");
        work.pop_back();
        if (Multiplexer *seq = dynamic_cast<Multiplexer*>(getRawPointer(d)))
            work.insert(work.end(), seq->destinations_.begin(), seq->destinations_.end());
    }
    
    // Add it as the last child
    destinations_.push_back(destination);
    return sharedFromThis().dynamicCast<Multiplexer>();
}

SAWYER_EXPORT MultiplexerPtr
Multiplexer::removeDestination(const DestinationPtr &destination) {
    destinations_.erase(std::remove(destinations_.begin(), destinations_.end(), destination), destinations_.end());
    return sharedFromThis().dynamicCast<Multiplexer>();
}

SAWYER_EXPORT MultiplexerPtr
Multiplexer::to(const DestinationPtr &destination) {
    addDestination(destination);
    return sharedFromThis().dynamicCast<Multiplexer>();
}

SAWYER_EXPORT MultiplexerPtr
Multiplexer::to(const DestinationPtr &d1, const DestinationPtr &d2) {
    to(d1);
    return to(d2);
}

SAWYER_EXPORT MultiplexerPtr
Multiplexer::to(const DestinationPtr &d1, const DestinationPtr &d2,
                               const DestinationPtr &d3) {
    to(d1, d2);
    return to(d3);
}

SAWYER_EXPORT MultiplexerPtr
Multiplexer::to(const DestinationPtr &d1, const DestinationPtr &d2,
                               const DestinationPtr &d3, const DestinationPtr &d4) {
    to(d1, d2);
    return to(d3, d4);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SAWYER_EXPORT void
Filter::bakeDestinations(const MesgProps &props, BakedDestinations &baked) {
    if (shouldForward(props)) {
        Multiplexer::bakeDestinations(props, baked);
        forwarded(props);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SAWYER_EXPORT bool
SequenceFilter::shouldForward(const MesgProps&) {
    bool retval = nPosted_ >= nSkip_ &&
                  0 == (nPosted_ - nSkip_) % std::max(rate_, (size_t)1) &&
                  (0==limit_ || (nPosted_ - nSkip_) / std::max(rate_, (size_t)1) < limit_);
    ++nPosted_;
    return retval;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SAWYER_EXPORT TimeFilterPtr
TimeFilter::initialDelay(double delta) {
    if (delta > 0.0) {
        initialDelay_ = delta;
        if (0==nPosted_)
            lastBakeTime_ = now();
    }
    return sharedFromThis().dynamicCast<TimeFilter>();
}

SAWYER_EXPORT bool
TimeFilter::shouldForward(const MesgProps&) {
    ++nPosted_;
    lastBakeTime_ = now();
    return lastBakeTime_ - prevMessageTime_ >= minInterval_;
}

SAWYER_EXPORT void
TimeFilter::forwarded(const MesgProps&) {
    prevMessageTime_ = lastBakeTime_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SAWYER_EXPORT bool
ImportanceFilter::shouldForward(const MesgProps &props) {
    assert(!props.importance || (*props.importance >=0 && *props.importance < N_IMPORTANCE));
    return props.importance && enabled_[*props.importance];
}

SAWYER_EXPORT ImportanceFilterPtr
ImportanceFilter::enable(Importance imp) {
    enabled(imp, true);
    return sharedFromThis().dynamicCast<ImportanceFilter>();
}

SAWYER_EXPORT ImportanceFilterPtr
ImportanceFilter::disable(Importance imp) {
    enabled(imp, false);
    return sharedFromThis().dynamicCast<ImportanceFilter>();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool HighWater::isValid() const {
    return bool(id_);
}

void HighWater::emitted(const Mesg &mesg, const MesgProps &props) {
    if (mesg.isComplete()) {
        *this = HighWater();
    } else {
        id_ = mesg.id();
        props_ = props;
        ntext_ = mesg.text().size();
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// This is a pointer so that it doesn't depend on order of global variable initialization. We'll allocate it the first time we
// need it.
Gang::GangMap *Gang::gangs_ = NULL;

GangPtr Gang::instanceForId(int id) {
    if (!gangs_)
        gangs_ = new GangMap;
    return gangs_->insertMaybe(id, Gang::instance());
}

void Gang::removeInstance(int id) {
    if (!gangs_)
        gangs_ = new GangMap;
    gangs_->erase(id);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SAWYER_EXPORT void
Prefix::setProgramName() {
#ifdef BOOST_WINDOWS
# if 0 // [Robb Matzke 2014-06-13] temporarily disable for ROSE linking error (needs psapi.lib in Windows)
    if (HANDLE handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, GetCurrentProcessId())) {
        TCHAR buffer[MAX_PATH];
        if (GetModuleFileNameEx(handle, 0, buffer, MAX_PATH)) { // requires linking with MinGW's psapi.a
            std::string name = buffer;
            size_t slash_idx = name.rfind('\\');
            if (slash_idx != std::string::npos)
                name = name.substr(slash_idx+1);
            if (name.size()>4 && 0==name.substr(name.size()-4, 4).compare(".exe"))
                name = name.substr(0, name.size()-4);
            programName_ = name;
        }
        CloseHandle(handle);
    }
# else
    programName_ = "FIXME(Sawyer::Message::Prefix::setProgramName)";
# endif
#else
    if (FILE *f = fopen("/proc/self/cmdline", "r")) {
        std::string name;
        int c;
        while ((c = fgetc(f)) > 0)
            name += (char)c;
        fclose(f);
        size_t slash_idx = name.rfind('/');
        if (slash_idx != std::string::npos)
            name = name.substr(slash_idx+1);
        if (name.size()>3 && 0==name.substr(0, 3).compare("lt-"))
            name = name.substr(3);
        programName_ = name;
    }
#endif
    if (programName_.orElse("").empty())
        throw std::runtime_error("cannot obtain program name for message prefixes");
}

SAWYER_EXPORT void
Prefix::setStartTime() {
#if 0 /* FIXME[Robb Matzke 2014-01-19]: st_ctime_usec is not defined. */
    struct stat sb;
    if (-1 == stat("/proc/self", &sb))
        throw std::runtime_error("cannot stat /proc/self");
    startTime_ = sb.st_ctime + 1e-9*sb.st_ctime_usec;
#else /* this is the work-around */
    startTime_ = now();
#endif
}

SAWYER_EXPORT void
Prefix::initFromSystem() {
    setProgramName();
    setStartTime();
}

SAWYER_EXPORT std::string
Prefix::toString(const Mesg &mesg, const MesgProps &props) const {
    std::ostringstream retval;
    std::string separator = "";

    std::string endColor;
    if (props.useColor && props.importance) {
        const ColorSpec &cs = colorSet_[*props.importance];
        if (!cs.isDefault()) {
            const char *semi = "";
            retval <<"\033[";
            if (COLOR_DEFAULT!=cs.foreground) {
                retval <<(30+cs.foreground);
                semi = ";";
            }
            if (COLOR_DEFAULT!=cs.background) {
                retval <<semi <<(40+cs.background);
                semi = ";";
            }
            if (cs.bold)
                retval <<semi <<"1";
            retval <<"m";
            endColor = "\033[m";
        }
    }

    std::string programNameShown;
    if (showProgramName_ && programName_) {
        programNameShown = *programName_;
        retval <<*programName_;
        if (showThreadId_) {
#ifdef BOOST_WINDOWS
            retval <<"[" <<GetCurrentProcessId() <<"]";
#else
            retval <<"[" <<getpid() <<"]";
#endif
        }
        separator = " ";
    }

    if (showElapsedTime_ && startTime_) {
        double delta = now() - *startTime_;
        retval.precision(5);
        retval <<separator <<std::fixed <<delta <<"s";
        separator = " ";
    }

    std::string facilityNameShown;
    if (showFacilityName_ && props.facilityName) {
        if (SOMETIMES!=showFacilityName_ || 0!=programNameShown.compare(*props.facilityName)) {
            facilityNameShown = *props.facilityName;
            retval <<separator <<*props.facilityName;
        }
        separator = " ";
    }

    if (showImportance_ && props.importance) {
        if (facilityNameShown.empty())
            retval <<separator;
        retval <<"[" <<std::setw(5) <<std::left <<stringifyImportance(*props.importance) <<"]";
        separator = " ";
    }

    if (separator.empty()) {
        retval <<endColor;
    } else {
        retval <<":" <<endColor <<" ";
    }

    return retval.str();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SAWYER_EXPORT void
UnformattedSink::init() {
    defaultProperties().importance = INFO;
    defaultProperties().isBuffered = false;
    defaultProperties().completionStr = "";
    defaultProperties().interruptionStr = "...";
    defaultProperties().cancelationStr = "... [CANCELD]";
    defaultProperties().lineTermination = "\n";
    defaultProperties().useColor = true;
}

SAWYER_EXPORT std::string
UnformattedSink::maybeTerminatePrior(const Mesg &mesg, const MesgProps &props) {
    std::string retval;
    if (!mesg.isEmpty()) {
        if (gang()->isValid() && gang()->id() != mesg.id()) {
            retval = gang()->properties().interruptionStr.orDefault() +
                     gang()->properties().lineTermination.orDefault();
            gang()->clear();
        }
    }
    return retval;
}

SAWYER_EXPORT std::string
UnformattedSink::maybePrefix(const Mesg &mesg, const MesgProps &props) {
    std::string retval;
    if (!mesg.isEmpty() && !gang()->isValid())
        retval = prefix()->toString(mesg, props);
    return retval;
}

SAWYER_EXPORT std::string
UnformattedSink::maybeBody(const Mesg &mesg, const MesgProps &props) {
    std::string retval;
    if (!mesg.isEmpty())
        retval = mesg.text().substr(gang()->ntext());
    return retval;
}

SAWYER_EXPORT std::string
UnformattedSink::maybeFinal(const Mesg &mesg, const MesgProps &props) {
    std::string retval;
    if (!mesg.isEmpty()) {
        if (mesg.isCanceled()) {
            retval = props.cancelationStr.orDefault() +
                     props.lineTermination.orDefault();
            gang()->clear();
        } else if (mesg.isComplete()) {
            retval = props.completionStr.orDefault() +
                     props.lineTermination.orDefault();
            gang()->clear();
        } else {
            gang()->emitted(mesg, props);
        }
    }
    return retval;
}

SAWYER_EXPORT std::string
UnformattedSink::render(const Mesg &mesg, const MesgProps &props) {
    std::string retval;
    retval += maybeTerminatePrior(mesg, props);         // force side effects in a particular order
    retval += maybePrefix(mesg, props);
    retval += maybeBody(mesg, props);
    retval += maybeFinal(mesg, props);
    return retval;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SAWYER_EXPORT void
FdSink::init() {
#ifdef BOOST_WINDOWS
    gangInternal(Gang::instanceForId(fd_));
    overrideProperties().useColor = true;
#else
    if (isatty(fd_)) {
        gangInternal(Gang::instanceForTty());
        defaultProperties().useColor = true;            // use color if the user doesn't care
    } else {
        gangInternal(Gang::instanceForId(fd_));
        overrideProperties().useColor = false;          // force false; user can still set this if they really want color
    }
#endif
    defaultProperties().isBuffered = 2!=fd_;            // assume stderr is unbuffered and the rest are buffered
}

SAWYER_EXPORT void
FdSink::post(const Mesg &mesg, const MesgProps &props) {
#ifdef BOOST_WINDOWS
    // FIXME[Robb Matzke 2014-06-10]: what is the most basic file level on Windows; one which doesn't need construction?
    std::cout <<render(mesg, props);
#else
    std::string s = render(mesg, props);
    const char *buf = s.c_str();
    size_t nbytes = s.size();

    while (nbytes > 0) {
        ssize_t nwritten = write(fd_, buf, nbytes);
        if (-1==nwritten && EINTR==errno) {
            // try again
        } else if (-1==nwritten) {
            break;
        } else {
            assert((size_t)nwritten <= nbytes);
            nbytes -= nwritten;
        }
    }
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SAWYER_EXPORT void
FileSink::init() {
#ifdef BOOST_WINDOWS
    gangInternal(Gang::instanceForTty());
    overrideProperties().useColor = true;
    defaultProperties().isBuffered = false;
#else
    if (isatty(fileno(file_))) {
        gangInternal(Gang::instanceForTty());
        overrideProperties().useColor = true;           // use color if the user doesn't care
    } else {
        gangInternal(Gang::instanceForId(fileno(file_)));
        overrideProperties().useColor = false;          // force false; user can still set this if they really want color
    }
    defaultProperties().isBuffered = 2!=fileno(file_);  // assume stderr is unbuffered and the rest are buffered
#endif
}

SAWYER_EXPORT void
FileSink::post(const Mesg &mesg, const MesgProps &props) {
    fputs(render(mesg, props).c_str(), file_);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SAWYER_EXPORT void
StreamSink::post(const Mesg &mesg, const MesgProps &props) {
    stream_ <<render(mesg, props);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_WINDOWS
SyslogSink::SyslogSink(const char *ident, int option, int facility) {
    init();
    openlog(ident, option, facility);
}

SAWYER_EXPORT void
SyslogSink::init() {
    overrideProperties().isBuffered = true;
}

SAWYER_EXPORT void
SyslogSink::post(const Mesg &mesg, const MesgProps &props) {
    if (mesg.isComplete()) {
        int priority = LOG_ERR;
        switch (props.importance.orElse(ERROR)) {
            case DEBUG: priority = LOG_DEBUG;   break;
            case TRACE: priority = LOG_DEBUG;   break;
            case WHERE: priority = LOG_DEBUG;   break;
            case MARCH: priority = LOG_DEBUG;   break;
            case INFO:  priority = LOG_INFO;    break;
            case WARN:  priority = LOG_WARNING; break;
            case ERROR: priority = LOG_ERR;     break;
            case FATAL: priority = LOG_CRIT;    break;
            case N_IMPORTANCE: break;
        }
        syslog(priority, "%s", mesg.text().c_str());
    }
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SAWYER_EXPORT void
StreamBuf::post() {
    if (enabled_ && message_.hasText() && (message_.isComplete() || anyUnbuffered_)) {
        assert(isBaked_);
        message_.post(baked_);
    }
}

SAWYER_EXPORT void
StreamBuf::completeMessage() {
    if (!message_.isEmpty()) {
        message_.complete();
        post();
    }
    message_ = Mesg(dflt_props_);
    baked_.clear();
    isBaked_ = false;
    anyUnbuffered_ = false;
}

SAWYER_EXPORT void
StreamBuf::cancelMessage() {
    if (!message_.isEmpty()) {
        message_.cancel();
        post();
    }
    message_ = Mesg(dflt_props_);
    baked_.clear();
    isBaked_ = false;
    anyUnbuffered_ = false;
}

SAWYER_EXPORT void
StreamBuf::bake() {
    if (!isBaked_) {
        destination_->bakeDestinations(message_.properties(), baked_/*out*/);
        anyUnbuffered_ = false;
        for (BakedDestinations::const_iterator bi=baked_.begin(); bi!=baked_.end() && !anyUnbuffered_; ++bi)
            anyUnbuffered_ = !bi->second.isBuffered;
        isBaked_ = true;
    }
}

SAWYER_EXPORT std::streamsize
StreamBuf::xsputn(const char *s, std::streamsize &n) {
    static const char termination_symbol = '\n';

    for (std::streamsize i=0; i<n; ++i) {
        if (termination_symbol==s[i]) {
            completeMessage();
        } else if ('\r'!=s[i]) {
            message_.insert(s[i]);
            for (std::streamsize i=0; i<n; ++i) {
                if (isgraph(s[i])) {
                    bake();
                    break;
                }
            }
        }
    }
    post();
    return n;
}

SAWYER_EXPORT StreamBuf::int_type
StreamBuf::overflow(int_type c) {
    if (c==traits_type::eof())
        return traits_type::eof();
    char_type ch = traits_type::to_char_type(c);
    std::streamsize nchars = 1;
    return xsputn(&ch, nchars) == 1 ? c : traits_type::eof();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SAWYER_EXPORT void
Stream::enable(bool b) {
    if (!b) {
        streambuf_->enabled_ = false;
    } else if (!streambuf_->enabled_) {
        streambuf_->enabled_ = true;
        streambuf_->post();
    }
}

SAWYER_EXPORT void
Stream::completionString(const std::string &s, bool asDefault) {
    streambuf_->message_.properties().completionStr = s;
    if (asDefault) {
        streambuf_->dflt_props_.completionStr = s;
    } else if (streambuf_->isBaked_) {
        throw std::runtime_error("message properties are already baked");
    }
}

SAWYER_EXPORT void
Stream::interruptionString(const std::string &s, bool asDefault) {
    streambuf_->message_.properties().interruptionStr = s;
    if (asDefault) {
        streambuf_->dflt_props_.interruptionStr = s;
    } else if (streambuf_->isBaked_) {
        throw std::runtime_error("message properties are already baked");
    }
}

SAWYER_EXPORT void
Stream::cancelationString(const std::string &s, bool asDefault) {
    streambuf_->message_.properties().cancelationStr = s;
    if (asDefault) {
        streambuf_->dflt_props_.cancelationStr = s;
    } else if (streambuf_->isBaked_) {
        throw std::runtime_error("message properties are already baked");
    }
}

SAWYER_EXPORT void
Stream::facilityName(const std::string &s, bool asDefault) {
    streambuf_->message_.properties().facilityName = s;
    if (asDefault) {
        streambuf_->dflt_props_.facilityName = s;
    } else if (streambuf_->isBaked_) {
        throw std::runtime_error("message properties are already baked");
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SAWYER_EXPORT
SProxy::SProxy(std::ostream *o): stream_(NULL) {
    Stream *s = dynamic_cast<Stream*>(o);
    assert(s!=NULL);
    ++s->nrefs_;
    reset();
    stream_ = s;
}

SAWYER_EXPORT
SProxy::SProxy(std::ostream &o): stream_(NULL) {
    Stream *s = dynamic_cast<Stream*>(&o);
    assert(s!=NULL);
    ++s->nrefs_;
    reset();
    stream_ = s;
}

SAWYER_EXPORT
SProxy::SProxy(const SProxy &other): stream_(other.stream_) {
    if (stream_)
        ++stream_->nrefs_;
}

SAWYER_EXPORT SProxy&
SProxy::operator=(const SProxy &other) {
    if (this != &other) {
        if (other.stream_) {
            ++other.stream_->nrefs_;
            reset();
            stream_ = other.stream_;
        } else {
            reset();
        }
    }
    return *this;
}

SAWYER_EXPORT void
SProxy::reset() {
    if (stream_!=NULL) {
        assert(stream_->nrefs_ > 0);
        if (0 == --stream_->nrefs_)
            delete stream_;
        stream_ = NULL;
    }
}

SAWYER_EXPORT
SProxy::operator bool() const {
    return stream_!=NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SAWYER_EXPORT Facility&
Facility::initStreams(const DestinationPtr &destination) {
    if (streams_.empty()) {
        for (int i=0; i<N_IMPORTANCE; ++i)
            streams_.push_back(new Stream(name_, (Importance)i, destination));
    } else {
        for (size_t i=0; i<streams_.size(); ++i)
            streams_[i]->destination(destination);
    }
    return *this;
}

SAWYER_EXPORT Facility&
Facility::renameStreams(const std::string &name) {
    for (size_t i=0; i<streams_.size(); ++i)
        streams_[i]->facilityName(name.empty() ? name_ : name);
    return *this;
}

SAWYER_EXPORT Stream&
Facility::get(Importance imp) {
    if (imp<0 || imp>=N_IMPORTANCE)
        throw std::runtime_error("invalid importance level");
    if ((size_t)imp>=streams_.size() || NULL==streams_[imp].get()) {
        // If you're looking at this line in a debugger it's probably because you're trying to use a Stream from a
        // default-constructed Facility.  Facilities that are allocated statically and/or at global scope should probably
        // either be constructed with Facility(const std::string&) or initialized by assigning some other facility to them.
        // Another possibility is that you provided Sawyer::Message::merr as the destination before libsawyer had a chance to
        // initialize that global variable. You can work around that problem by calling Sawyer::initializeLibrary() first.
        //
        // ROSE users: librose does not currently (2014-09-09) initialize libsawyer until the ROSE frontend() is called. If
        // you're calling into librose before calling "frontend" then you probably want to explicitly initialize ROSE by
        // invoking rose::Diagnostics::initialize() early in "main".
        throw std::runtime_error("stream " + stringifyImportance(imp) +
                                 (name_.empty() ? std::string() : " in facility \"" + name_ + "\"") +
                                 " is not initialized yet");
    }
    return *streams_[imp];
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SAWYER_EXPORT Facilities&
Facilities::impset(Importance imp, bool enabled) {
    if (!impsetInitialized_) {
#if 0 // these are typically too verbose for end users
        impset_.insert(DEBUG);
        impset_.insert(TRACE);
        impset_.insert(WHERE);
#endif
        impset_.insert(MARCH);
        impset_.insert(INFO);
        impset_.insert(WARN);
        impset_.insert(ERROR);
        impset_.insert(FATAL);
    }
    if (enabled) {
        impset_.insert(imp);
    } else {
        impset_.erase(imp);
    }
    impsetInitialized_ = true;
    return *this;
}

SAWYER_EXPORT Facilities&
Facilities::insert(Facility &facility, std::string name) {
    if (name.empty())
        name = facility.name();
    if (name.empty())
        throw std::logic_error("facility name is empty and no name was supplied");
    const char *s = name.c_str();
    if (0!=name.compare(parseFacilityName(s)))
        throw std::logic_error("name '"+name+"' is not valid for the Facilities::control language");
    FacilityMap::NodeIterator found = facilities_.find(name);
    if (found!=facilities_.nodes().end()) {
        if (found->value()!= &facility)
            throw std::logic_error("message facility '"+name+"' is used more than once");
    } else {
        facilities_.insert(name, &facility);
    }

    if (!impsetInitialized_) {
        impset_.clear();
        for (int i=0; i<N_IMPORTANCE; ++i) {
            Importance mi = (Importance)i;
            if (facility[mi])
                impset_.insert(mi);
        }
        impsetInitialized_ = true;
    }
    return *this;
}

SAWYER_EXPORT Facilities&
Facilities::insertAndAdjust(Facility &facility, std::string name) {
    insert(facility, name); // throws

    // Now that the facility has been successfully inserted...
    for (int i=0; i<N_IMPORTANCE; ++i) {
        Importance mi = (Importance)i;
        facility[mi].enable(impset_.find(mi)!=impset_.end());
    }
    return *this;
}

SAWYER_EXPORT Facilities&
Facilities::erase(Facility &facility) {
    FacilityMap map = facilities_;;
    BOOST_FOREACH (const FacilityMap::Node &node, map.nodes()) {
        if (node.value() == &facility)
            facilities_.erase(node.key());
    }
    return *this;
}

SAWYER_EXPORT Facilities&
Facilities::reenable() {
    BOOST_FOREACH (const FacilityMap::Node &node, facilities_.nodes()) {
        for (int i=0; i<N_IMPORTANCE; ++i) {
            Importance imp = (Importance)i;
            node.value()->get(imp).enable(impset_.find(imp)!=impset_.end());
        }
    }
    return *this;
}

SAWYER_EXPORT Facilities&
Facilities::reenableFrom(const Facilities &other) {
    BOOST_FOREACH (const FacilityMap::Node &src, other.facilities_.nodes()) {
        FacilityMap::NodeIterator fi_dst = facilities_.find(src.key());
        if (fi_dst!=facilities_.nodes().end()) {
            for (int i=0; i<N_IMPORTANCE; ++i) {
                Importance imp = (Importance)i;
                fi_dst->value()->get(imp).enable(src.value()->get(imp).enabled());
            }
        }
    }
    return *this;
}

SAWYER_EXPORT Facilities&
Facilities::enable(const std::string &switch_name, bool b) {
    FacilityMap::NodeIterator found = facilities_.find(switch_name);
    if (found != facilities_.nodes().end()) {
        if (b) {
            for (int i=0; i<N_IMPORTANCE; ++i) {
                Importance imp = (Importance)i;
                found->value()->get(imp).enable(impset_.find(imp)!=impset_.end());
            }
        } else {
            for (int i=0; i<N_IMPORTANCE; ++i)
                found->value()->get((Importance)i).disable();
        }
    }
    return *this;
}
    
SAWYER_EXPORT Facilities&
Facilities::enable(Importance imp, bool b) {
    if (b) {
        impset_.insert(imp);
    } else {
        impset_.erase(imp);
    }
    BOOST_FOREACH (const FacilityMap::Node &node, facilities_.nodes())
        node.value()->get(imp).enable(b);
    return *this;
}

SAWYER_EXPORT Facilities&
Facilities::enable(bool b) {
    BOOST_FOREACH (Facility *facility, facilities_.values()) {
        if (b) {
            for (int i=0; i<N_IMPORTANCE; ++i) {
                Importance imp = (Importance)i;
                facility->get(imp).enable(impset_.find(imp)!=impset_.end());
            }
        } else {
            for (int i=0; i<N_IMPORTANCE; ++i)
                facility->get((Importance)i).disable();
        }
    }
    return *this;
}

std::string
Facilities::ControlTerm::toString() const {
    std::string s = enable ? "enable" : "disable";
    if (lo==hi) {
        s += " level " + stringifyImportance(lo);
    } else {
        s += " levels " + stringifyImportance(lo) + " through " + stringifyImportance(hi);
    }
    s += " for " + (facilityName.empty() ? "all registered facilities" : facilityName);
    return s;
}

// Matches the Perl regular expression /^\s*([a-zA-Z]\w*((\.|::)[a-zA-Z]\w*)*/
// On match, returns $1 and str points to the next character after the regular expression
// When not matched, returns "" and str is unchanged
SAWYER_EXPORT std::string
Facilities::parseFacilityName(const char *&str) {
    std::string name;
    const char *s = str;
    while (isspace(*s)) ++s;
    while (isalpha(*s)) {
        while (isalnum(*s) || '_'==*s) name += *s++;
        if ('.'==s[0] && (isalpha(s[1]) || '_'==s[1])) {
            name += ".";
            ++s;
        } else if (':'==s[0] && ':'==s[1] && (isalpha(s[2]) || '_'==s[2])) {
            name += "::";
            s += 2;
        }
    }
    if (!name.empty())
        str = s;
    return name;
}

// Matches the Perl regular expression /^\s*([+!]?)/ and returns $1 on success with str pointing to the character after the
// match.  Returns the empty string on failure with str not adjusted.
SAWYER_EXPORT std::string
Facilities::parseEnablement(const char *&str) {
    const char *s = str;
    while (isspace(*s)) ++s;
    if ('!'==*s || '+'==*s) {
        str = s+1;
        return std::string(s, 1);
    }
    return "";
}

// Matches the Perl regular expression /^\s*(<=?|>=?)/ and returns $1 on success with str pointing to the character after
// the match. Returns the empty string on failure with str not adjusted.
SAWYER_EXPORT std::string
Facilities::parseRelation(const char *&str) {
    const char *s = str;
    while (isspace(*s)) ++s;
    if (!strncmp(s, "<=", 2) || !strncmp(s, ">=", 2)) {
        str = s + 2;
        return std::string(s, 2);
    } else if ('<'==*s || '>'==*s) {
        str = s + 1;
        return std::string(s, 1);
    }
    return "";
}

// Matches the Perl regular expression /^\s*(all|none|debug|trace|where|info|warn|error|fatal)\b/
// On match, returns $1 and str points to the next character after the match
// On failure, returns "" and str is unchanged
SAWYER_EXPORT std::string
Facilities::parseImportanceName(const char *&str) {
    static const char *words[] = {"all", "none", "debug", "trace", "where", "march", "info", "warn", "error", "fatal"};
    static const size_t nwords = sizeof(words)/sizeof(words[0]);

    const char *s = str;
    while (isspace(*s)) ++s;
    for (size_t i=0; i<nwords; ++i) {
        size_t n = strlen(words[i]);
        if (boost::iequals(std::string(s).substr(0, n), std::string(words[i]).substr(0, n)) && '_'!=s[n]) {
            str += (s-str) + n;
            return words[i];
        }
    }
    return "";
}

SAWYER_EXPORT Importance
Facilities::importanceFromString(const std::string &str) {
    if (boost::iequals(str, "debug"))
        return DEBUG;
    if (boost::iequals(str, "trace"))
        return TRACE;
    if (boost::iequals(str, "where"))
        return WHERE;
    if (boost::iequals(str, "march"))
        return MARCH;
    if (boost::iequals(str, "info"))
        return INFO;
    if (boost::iequals(str, "warn"))
        return WARN;
    if (boost::iequals(str, "error"))
        return ERROR;
    if (boost::iequals(str, "fatal"))
        return FATAL;
    return N_IMPORTANCE;                                // error
}

// parses a StreamControlList. On success, returns a non-empty vector and adjust 'str' to point to the next character after the
// list.  On failure, throw a ControlError.
SAWYER_EXPORT std::list<Facilities::ControlTerm>
Facilities::parseImportanceList(const std::string &facilityName, const char *&str) {
    const char *s = str;
    std::list<ControlTerm> retval;

    while (1) {
        const char *elmtStart = s;

        // List elements are separated by a comma.
        if (!retval.empty()) {
            while (isspace(*s)) ++s;
            if (','!=*s) {
                s = elmtStart;
                break;
            }
            ++s;
        }

        while (isspace(*s)) ++s;
        const char *enablementStart = s;
        std::string enablement = parseEnablement(s);

        while (isspace(*s)) ++s;
        const char *relationStart = s;
        std::string relation = parseRelation(s);

        while (isspace(*s)) ++s;
        const char *importanceStart = s;
        std::string importance = parseImportanceName(s);
        if (importance.empty()) {
            if (!enablement.empty() || !relation.empty() || (isalpha(s[0]) && !retval.empty()))
                throw ControlError("message importance level expected", importanceStart);
            s = elmtStart;
            break;
        }

        ControlTerm term(facilityName, enablement.compare("!")!=0);
        if (boost::iequals(importance, "all") || boost::iequals(importance, "none")) {
            if (!enablement.empty())
                throw ControlError("'"+importance+"' cannot be preceded by '"+enablement+"'", enablementStart);
            if (!relation.empty())
                throw ControlError("'"+importance+"' cannot be preceded by '"+relation+"'", relationStart);
            term.lo = DEBUG;
            term.hi = FATAL;
            term.enable = !boost::iequals(importance, "none");
        } else {
            Importance imp = importanceFromString(importance);
            if (N_IMPORTANCE==imp)
                throw ControlError("'"+importance+"' is not a valid importance", relationStart);
            if (relation.empty()) {
                term.lo = term.hi = imp;
            } else if (relation[0]=='<') {
                term.lo = DEBUG;
                term.hi = imp;
                if (1==relation.size()) {
                    if (DEBUG==imp)
                        continue; // empty set
                    term.hi = (Importance)(term.hi - 1);
                }
            } else {
                term.lo = imp;
                term.hi = FATAL;
                if (1==relation.size()) {
                    if (FATAL==imp)
                        continue; // empty set
                    term.lo = (Importance)(term.lo + 1);
                }
            }
        }
        retval.push_back(term);
    }

    if (!retval.empty())
        str = s;
    return retval;
}

SAWYER_EXPORT std::string
Facilities::control(const std::string &ss) {
    const char *start = ss.c_str();
    const char *s = start;
    std::list<ControlTerm> terms;

    try {
        while (1) {
            std::list<ControlTerm> t2 = parseImportanceList("", s);
            if (t2.empty()) {
                // facility name
                while (isspace(*s)) ++s;
                const char *facilityNameStart = s;
                std::string facilityName = parseFacilityName(s);
                if (facilityName.empty())
                    break;
                if (!facilities_.exists(facilityName))
                    throw ControlError("no such message facility '"+facilityName+"'", facilityNameStart);

                // stream control list in parentheses
                while (isspace(*s)) ++s;
                if ('('!=*s)
                    throw ControlError("expected '(' after message facility name '"+facilityName+"'", s);
                ++s;
                t2 = parseImportanceList(facilityName, s);
                if (t2.empty())
                    throw ControlError("expected stream control list after '('", s);
                while (isspace(*s)) ++s;
                if (')'!=*s)
                    throw ControlError("expected ')' at end of stream control list for '"+facilityName+"'", s);
                ++s;
            }

            terms.insert(terms.end(), t2.begin(), t2.end());
            while (isspace(*s)) ++s;
            if (','!=*s)
                break;
            ++s;
        }

        while (isspace(*s)) ++s;
        if (*s) {
            if (terms.empty())
                throw ControlError("syntax error", s);
            if (terms.back().facilityName.empty())
                throw ControlError("syntax error in global list", s);
            throw ControlError("syntax error after '"+terms.back().facilityName+"' list", s);
        }
    } catch (const ControlError &error) {
        std::string s = error.mesg + "\n";
        size_t offset = error.inputPosition - start;
        if (offset <= ss.size()) {
            s += "    error occurred in \"" + ss + "\"\n";
            s += "    at this position   " + std::string(offset, '-') + "^\n";
        }
        return s;
    }

    for (std::list<ControlTerm>::iterator ti=terms.begin(); ti!=terms.end(); ++ti) {
        const ControlTerm &term = *ti;
        if (term.facilityName.empty()) {
            for (Importance imp=term.lo; imp<=term.hi; imp=(Importance)(imp+1))
                enable(imp, term.enable);
        } else {
            FacilityMap::NodeIterator found = facilities_.find(term.facilityName);
            assert(found!=facilities_.nodes().end() && found->value()!=NULL);
            for (Importance imp=term.lo; imp<=term.hi; imp=(Importance)(imp+1))
                found->value()->get(imp).enable(term.enable);
        }
    }

    return ""; // no errors
}

SAWYER_EXPORT std::vector<std::string>
Facilities::facilityNames() const {
    std::vector<std::string> allNames;
    BOOST_FOREACH (const std::string &name, facilities_.keys())
        allNames.push_back(name);
    return allNames;
}

SAWYER_EXPORT void
Facilities::print(std::ostream &log) const {
    if (impsetInitialized_) {
        for (int i=0; i<N_IMPORTANCE; ++i) {
            Importance mi = (Importance)i;
            log <<(impset_.find(mi)==impset_.end() ? '-' : (mi==WHERE?'H':stringifyImportance(mi)[0]));
        }
        log <<" default enabled levels\n";
    }

    if (facilities_.isEmpty()) {
        log <<"no message facilities registered\n";
    } else {
        BOOST_FOREACH (const FacilityMap::Node &fnode, facilities_.nodes()) {
            Facility *facility = fnode.value();

            // A short easy to read format. Letters indicate the importances that are enabled; dashes keep them aligned.
            // Sort of like the format 'ls -l' uses to show permissions.
            for (int i=0; i<N_IMPORTANCE; ++i) {
                Importance mi = (Importance)i;
                log <<(facility->get(mi) ? (mi==WHERE?'H':stringifyImportance(mi)[0]) : '-');
            }
            log <<" " <<fnode.key() <<"\n";
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SAWYER_EXPORT DestinationPtr merr SAWYER_STATIC_INIT;
SAWYER_EXPORT Facility mlog SAWYER_STATIC_INIT ("sawyer");
SAWYER_EXPORT Facilities mfacilities SAWYER_STATIC_INIT;
SAWYER_EXPORT bool isInitialized;
SAWYER_EXPORT SProxy assertionStream SAWYER_STATIC_INIT;

SAWYER_EXPORT bool
initializeLibrary() {
    if (!isInitialized) {
        isInitialized = true;
        merr = FdSink::instance(2);
        mlog = Facility("", merr);
        mlog[DEBUG].disable();
        mlog[TRACE].disable();
        mlog[WHERE].disable();
        mlog[MARCH].disable();
        mlog[INFO ].disable();
        mfacilities.insert(mlog, "sawyer");
    }
    return true;
}

} // namespace
} // namespace
