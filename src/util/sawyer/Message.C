#include "Message.h"

#include <cerrno>
#include <cstdio>
#include <iomanip>
#include <stdexcept>
#include <sstream>
#include <sys/stat.h>
#include <syslog.h>
#include <vector>

namespace Sawyer {
namespace Message {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string stringifyImportance(Importance importance) {
    switch (importance) {
        case DEBUG: return "DEBUG";
        case TRACE: return "TRACE";
        case WHERE: return "WHERE";
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

std::string stringifyColor(AnsiColor color) {
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

double timeval_delta(const timeval &begin, const timeval &end) {
    return (1.0*end.tv_sec-begin.tv_sec) + 1e-6*end.tv_usec - 1e-6*begin.tv_usec;
}

std::string escape(const std::string &s) {
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
                    sprintf(buf, "\\%03o", (unsigned)s[i]);
                    retval += buf;
                }
                break;
        }
    }
    return retval;
}

double now() {
    timeval tv;
    if (-1 == gettimeofday(&tv, NULL))
        return 0;
    return 1.0 * tv.tv_sec + 1e-6 * tv.tv_usec;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ColorSet ColorSet::blackAndWhite() {
    ColorSet cs;
    cs[WARN] = cs[ERROR] = cs[FATAL] = ColorSpec(COLOR_DEFAULT, COLOR_DEFAULT, true);
    return cs;
}

ColorSet ColorSet::fullColor() {
    ColorSet cs;
    cs[DEBUG] = ColorSpec(COLOR_DEFAULT, COLOR_DEFAULT, false);
    cs[TRACE] = ColorSpec(COLOR_CYAN,    COLOR_DEFAULT, false);
    cs[WHERE] = ColorSpec(COLOR_CYAN,    COLOR_DEFAULT, false);
    cs[INFO]  = ColorSpec(COLOR_GREEN,   COLOR_DEFAULT, false);
    cs[WARN]  = ColorSpec(COLOR_YELLOW,  COLOR_DEFAULT, false);
    cs[ERROR] = ColorSpec(COLOR_RED,     COLOR_DEFAULT, false);
    cs[FATAL] = ColorSpec(COLOR_RED,     COLOR_DEFAULT, true);
    return cs;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MesgProps MesgProps::merge(const MesgProps &other) const {
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

void MesgProps::print(std::ostream &o) const {
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

std::ostream& operator<<(std::ostream &o, const MesgProps &props) {
    props.print(o);
    return o;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned Mesg::nextId_;

void Mesg::insert(const std::string &s) {
    if (isComplete())
        throw std::runtime_error("cannot add text to a completed message");
    text_ += s;
}

void Mesg::insert(char ch) {
    if (isComplete())
        throw std::runtime_error("cannot add text to a completed message");
    text_ += ch;
}

void Mesg::post(const BakedDestinations &baked) const {
    for (BakedDestinations::const_iterator bi=baked.begin(); bi!=baked.end(); ++bi)
        bi->first->post(*this, bi->second);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Destination::bakeDestinations(const MesgProps &props, BakedDestinations &baked) {
    baked.push_back(std::make_pair(shared_from_this(), mergeProperties(props)));
}

MesgProps Destination::mergeProperties(const MesgProps &props) {
    return overrides_.merge(props.merge(dflts_));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Multiplexer::bakeDestinations(const MesgProps &props, BakedDestinations &baked) {
    MesgProps downwardProps = mergeProperties(props);
    for (Destinations::const_iterator di=destinations_.begin(); di!=destinations_.end(); ++di)
        (*di)->bakeDestinations(downwardProps, baked);
}

// Multiplexors are never included in baked results (they control baking instead), so messages should never be posted directly
// to multiplexers.
void Multiplexer::post(const Mesg &mesg, const MesgProps &props) {
    assert(!"messages should not be posted to multiplexers");
}

void Multiplexer::addDestination(const DestinationPtr &destination) {
    assert(destination!=NULL);

    // Make sure this doesn't introduce a cycle
    std::vector<DestinationPtr> work(1, destination);
    while (!work.empty()) {
        DestinationPtr d = work.back();
        if (d.get()==this)
            throw std::runtime_error("cycle introduced in Sawyer::Multiplexer tree");
        work.pop_back();
        if (Multiplexer *seq = dynamic_cast<Multiplexer*>(d.get()))
            work.insert(work.end(), seq->destinations_.begin(), seq->destinations_.end());
    }
    
    // Add it as the last child
    destinations_.push_back(destination);
}

void Multiplexer::removeDestination(const DestinationPtr &destination) {
    destinations_.erase(std::remove(destinations_.begin(), destinations_.end(), destination), destinations_.end());
}

MultiplexerPtr Multiplexer::to(const DestinationPtr &destination) {
    addDestination(destination);
    return boost::dynamic_pointer_cast<Multiplexer>(shared_from_this());
}

MultiplexerPtr Multiplexer::to(const DestinationPtr &d1, const DestinationPtr &d2) {
    to(d1);
    return to(d2);
}

MultiplexerPtr Multiplexer::to(const DestinationPtr &d1, const DestinationPtr &d2,
                               const DestinationPtr &d3) {
    to(d1, d2);
    return to(d3);
}

MultiplexerPtr Multiplexer::to(const DestinationPtr &d1, const DestinationPtr &d2,
                               const DestinationPtr &d3, const DestinationPtr &d4) {
    to(d1, d2);
    return to(d3, d4);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Filter::bakeDestinations(const MesgProps &props, BakedDestinations &baked) {
    if (shouldForward(props)) {
        Multiplexer::bakeDestinations(props, baked);
        forwarded(props);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool SequenceFilter::shouldForward(const MesgProps&) {
    bool retval = nPosted_ >= nSkip_ &&
                  0 == (nPosted_ - nSkip_) % std::max(rate_, (size_t)1) &&
                  (0==limit_ || (nPosted_ - nSkip_) / std::max(rate_, (size_t)1) < limit_);
    ++nPosted_;
    return retval;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool TimeFilter::shouldForward(const MesgProps&) {
    gettimeofday(&lastBakeTime_, NULL);
    return timeval_delta(prevMessageTime_, lastBakeTime_) >= minInterval_;
}

void TimeFilter::forwarded(const MesgProps&) {
    prevMessageTime_ = lastBakeTime_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ImportanceFilter::shouldForward(const MesgProps &props) {
    assert(!props.importance || (*props.importance >=0 && *props.importance < N_IMPORTANCE));
    return props.importance && enabled_[*props.importance];
}

ImportanceFilterPtr ImportanceFilter::enable(Importance imp) {
    enabled(imp, true);
    return boost::dynamic_pointer_cast<ImportanceFilter>(shared_from_this());
}

ImportanceFilterPtr ImportanceFilter::disable(Importance imp) {
    enabled(imp, false);
    return boost::dynamic_pointer_cast<ImportanceFilter>(shared_from_this());
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

Gang::GangMap Gang::gangs_;

GangPtr Gang::instanceForId(int id) {
    GangMap::iterator found = gangs_.find(id);
    if (found==gangs_.end())
        found = gangs_.insert(std::make_pair(id, Gang::instance())).first;
    return found->second;
}

void Gang::removeInstance(int id) {
    gangs_.erase(id);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Prefix::setProgramName() {
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
    if (programName_.get_value_or("").empty())
        throw std::runtime_error("cannot obtain program name for message prefixes");
}

void Prefix::setStartTime() {
#if 0 /* FIXME[Robb Matzke 2014-01-19]: st_ctime_usec is not defined. */
    struct stat sb;
    if (-1 == stat("/proc/self", &sb))
        throw std::runtime_error("cannot stat /proc/self");
    startTime_->tv_sec = sb.st_ctime;
    startTime_->tv_usec = sb.st_ctime_usec;
#else /* this is the work-around */
    struct timeval tv;
    gettimeofday(&tv, NULL);
    startTime_ = tv;
#endif
}

void Prefix::initFromSystem() {
    setProgramName();
    setStartTime();
}

std::string Prefix::toString(const Mesg &mesg, const MesgProps &props) const {
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
    
    if (showProgramName_ && programName_) {
        retval <<*programName_;
        if (showThreadId_)
            retval <<"[" <<getpid() <<"]";
        separator = " ";
    }

    if (showElapsedTime_ && startTime_) {
        timeval tv;
        if (-1 != gettimeofday(&tv, NULL)) {
            double delta = timeval_delta(*startTime_, tv);
            retval.precision(5);
            retval <<separator <<std::fixed <<delta <<"s";
            separator = " ";
        }
    }

    if (showFacilityName_ && props.facilityName) {
        retval <<separator <<*props.facilityName;
        if (showImportance_ && props.importance)
            retval <<"[" <<std::setw(5) <<std::left <<stringifyImportance(*props.importance) <<"]";
        separator = " ";
    } else if (showImportance_ && props.importance) {
        retval <<separator <<"[" <<std::setw(5) <<std::left <<stringifyImportance(*props.importance) <<"]";
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

void UnformattedSink::init() {
    defaultProperties().importance = INFO;
    defaultProperties().isBuffered = false;
    defaultProperties().completionStr = "";
    defaultProperties().interruptionStr = "...";
    defaultProperties().cancelationStr = "... [CANCELD]";
    defaultProperties().lineTermination = "\n";
    defaultProperties().useColor = true;
}

std::string UnformattedSink::maybeTerminatePrior(const Mesg &mesg, const MesgProps &props) {
    std::string retval;
    if (!mesg.isEmpty()) {
        if (gang()->isValid() && gang()->id() != mesg.id()) {
            retval = gang()->properties().interruptionStr.get_value_or(std::string("")) +
                     gang()->properties().lineTermination.get_value_or(std::string(""));
            gang()->clear();
        }
    }
    return retval;
}

std::string UnformattedSink::maybePrefix(const Mesg &mesg, const MesgProps &props) {
    std::string retval;
    if (!mesg.isEmpty() && !gang()->isValid())
        retval = prefix()->toString(mesg, props);
    return retval;
}

std::string UnformattedSink::maybeBody(const Mesg &mesg, const MesgProps &props) {
    std::string retval;
    if (!mesg.isEmpty())
        retval = mesg.text().substr(gang()->ntext());
    return retval;
}

std::string UnformattedSink::maybeFinal(const Mesg &mesg, const MesgProps &props) {
    std::string retval;
    if (!mesg.isEmpty()) {
        if (mesg.isCanceled()) {
            retval = props.cancelationStr.get_value_or(std::string("")) +
                     props.lineTermination.get_value_or(std::string(""));
            gang()->clear();
        } else if (mesg.isComplete()) {
            retval = props.completionStr.get_value_or(std::string("")) +
                     props.lineTermination.get_value_or(std::string(""));
            gang()->clear();
        } else {
            gang()->emitted(mesg, props);
        }
    }
    return retval;
}

std::string UnformattedSink::render(const Mesg &mesg, const MesgProps &props) {
    std::string retval;
    retval += maybeTerminatePrior(mesg, props);         // force side effects in a particular order
    retval += maybePrefix(mesg, props);
    retval += maybeBody(mesg, props);
    retval += maybeFinal(mesg, props);
    return retval;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void FdSink::init() {
    if (isatty(fd_)) {
        gang(Gang::instanceForTty());
        defaultProperties().useColor = true;            // use color if the user doesn't care
    } else {
        gang(Gang::instanceForId(fd_));
        overrideProperties().useColor = false;          // force false; user can still set this if they really want color
    }
    defaultProperties().isBuffered = 2!=fd_;            // assume stderr is unbuffered and the rest are buffered
}

void FdSink::post(const Mesg &mesg, const MesgProps &props) {
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
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void FileSink::init() {
    if (isatty(fileno(file_))) {
        gang(Gang::instanceForTty());
        overrideProperties().useColor = true;           // use color if the user doesn't care
    } else {
        gang(Gang::instanceForId(fileno(file_)));
        overrideProperties().useColor = false;          // force false; user can still set this if they really want color
    }
    defaultProperties().isBuffered = 2!=fileno(file_);  // assume stderr is unbuffered and the rest are buffered
}

void FileSink::post(const Mesg &mesg, const MesgProps &props) {
    fputs(render(mesg, props).c_str(), file_);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void StreamSink::post(const Mesg &mesg, const MesgProps &props) {
    stream_ <<render(mesg, props);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SyslogSink::SyslogSink(const char *ident, int option, int facility) {
    init();
    openlog(ident, option, facility);
}

void SyslogSink::init() {
    overrideProperties().isBuffered = true;
}

void SyslogSink::post(const Mesg &mesg, const MesgProps &props) {
    if (mesg.isComplete()) {
        int priority = LOG_ERR;
        switch (props.importance.get_value_or(ERROR)) {
            case DEBUG: priority = LOG_DEBUG;   break;
            case TRACE: priority = LOG_DEBUG;   break;
            case WHERE: priority = LOG_DEBUG;   break;
            case INFO:  priority = LOG_INFO;    break;
            case WARN:  priority = LOG_WARNING; break;
            case ERROR: priority = LOG_ERR;     break;
            case FATAL: priority = LOG_CRIT;    break;
            case N_IMPORTANCE: break;
        }
        syslog(priority, "%s", mesg.text().c_str());
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void StreamBuf::post() {
    if (enabled_ && !message_.isEmpty() && (message_.isComplete() || anyUnbuffered_)) {
        assert(isBaked_);
        message_.post(baked_);
    }
}

void StreamBuf::completeMessage() {
    if (!message_.isEmpty()) {
        message_.complete();
        post();
    }
    message_ = Mesg(dflt_props_);
    baked_.clear();
    isBaked_ = false;
    anyUnbuffered_ = false;
}

void StreamBuf::cancelMessage() {
    if (!message_.isEmpty()) {
        message_.cancel();
        post();
    }
    message_ = Mesg(dflt_props_);
    baked_.clear();
    isBaked_ = false;
    anyUnbuffered_ = false;
}

void StreamBuf::bake() {
    if (!isBaked_) {
        destination_->bakeDestinations(message_.properties(), baked_/*out*/);
        anyUnbuffered_ = false;
        for (BakedDestinations::const_iterator bi=baked_.begin(); bi!=baked_.end() && !anyUnbuffered_; ++bi)
            anyUnbuffered_ = !bi->second.isBuffered;
        isBaked_ = true;
    }
}

std::streamsize StreamBuf::xsputn(const char *s, std::streamsize &n) {
    static const char termination_symbol = '\n';

    for (std::streamsize i=0; i<n; ++i) {
        if (termination_symbol==s[i]) {
            completeMessage();
        } else if ('\r'!=s[i]) {
            message_.insert(s[i]);
            bake();
        }
    }
    post();
    return n;
}

StreamBuf::int_type StreamBuf::overflow(int_type c) {
    if (c==traits_type::eof())
        return traits_type::eof();
    char_type ch = traits_type::to_char_type(c);
    std::streamsize nchars = 1;
    return xsputn(&ch, nchars) == 1 ? c : traits_type::eof();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Stream::enable(bool b) {
    if (!b) {
        streambuf_->enabled_ = false;
    } else if (!streambuf_->enabled_) {
        streambuf_->enabled_ = true;
        streambuf_->post();
    }
}

void Stream::completionString(const std::string &s, bool asDefault) {
    streambuf_->message_.properties().completionStr = s;
    if (asDefault) {
        streambuf_->dflt_props_.completionStr = s;
    } else if (streambuf_->isBaked_) {
        throw std::runtime_error("message properties are already baked");
    }
}

void Stream::interruptionString(const std::string &s, bool asDefault) {
    streambuf_->message_.properties().interruptionStr = s;
    if (asDefault) {
        streambuf_->dflt_props_.interruptionStr = s;
    } else if (streambuf_->isBaked_) {
        throw std::runtime_error("message properties are already baked");
    }
}

void Stream::cancelationString(const std::string &s, bool asDefault) {
    streambuf_->message_.properties().cancelationStr = s;
    if (asDefault) {
        streambuf_->dflt_props_.cancelationStr = s;
    } else if (streambuf_->isBaked_) {
        throw std::runtime_error("message properties are already baked");
    }
}

void Stream::facilityName(const std::string &s, bool asDefault) {
    streambuf_->message_.properties().facilityName = s;
    if (asDefault) {
        streambuf_->dflt_props_.facilityName = s;
    } else if (streambuf_->isBaked_) {
        throw std::runtime_error("message properties are already baked");
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SProxy::SProxy(std::ostream *o): stream_(NULL) {
    Stream *s = dynamic_cast<Stream*>(o);
    assert(s!=NULL);
    ++s->nrefs_;
    reset();
    stream_ = s;
}

SProxy::SProxy(std::ostream &o): stream_(NULL) {
    Stream *s = dynamic_cast<Stream*>(&o);
    assert(s!=NULL);
    ++s->nrefs_;
    reset();
    stream_ = s;
}

SProxy::SProxy(const SProxy &other): stream_(other.stream_) {
    if (stream_)
        ++stream_->nrefs_;
}

SProxy& SProxy::operator=(const SProxy &other) {
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

void SProxy::reset() {
    if (stream_!=NULL) {
        assert(stream_->nrefs_ > 0);
        if (0 == --stream_->nrefs_)
            delete stream_;
        stream_ = NULL;
    }
}

SProxy::operator bool() const {
    return stream_!=NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Facility::initStreams(const DestinationPtr &destination) {
    for (int i=0; i<N_IMPORTANCE; ++i)
        streams_.push_back(new Stream(name_, (Importance)i, destination));
}

Stream& Facility::get(Importance imp) {
    if (imp<0 || imp>=N_IMPORTANCE)
        throw std::runtime_error("invalid importance level");
    if ((size_t)imp>=streams_.size() || NULL==streams_[imp]) {
        // If you're looking at this line in a debugger, it's probably because you haven't called
        // Sawyer::Message::initializeLibrary().  ROSE USERS: Sawyer initialization happens in
        // rose::Diagnostics::initialize().
        throw std::runtime_error("stream " + stringifyImportance(imp) + " is not initialized yet");
    }
    return *streams_[imp];
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Facilities::impset(Importance imp, bool enabled) {
    if (enabled) {
        impset_.insert(imp);
    } else {
        impset_.erase(imp);
    }
    impsetInitialized_ = true;
}

void Facilities::insert(Facility &facility, std::string name) {
    if (name.empty())
        name = facility.name();
    if (name.empty())
        throw std::logic_error("facility name is empty and no name was supplied");
    const char *s = name.c_str();
    if (0!=name.compare(parseFacilityName(s)))
        throw std::logic_error("name '"+name+"' is not valid for the Facilities::control language");
    FacilityMap::iterator found = facilities_.find(name);
    if (found!=facilities_.end()) {
        if (found->second != &facility)
            throw std::logic_error("message facility '"+name+"' is used more than once");
    } else {
        facilities_.insert(std::make_pair(name, &facility));
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
}

void Facilities::insertAndAdjust(Facility &facility, std::string name) {
    ImportanceSet imps = impset_;
    insert(facility, name); // throws

    // Now that the facility has been successfully inserted...
    impset_ = imps;
    for (int i=0; i<N_IMPORTANCE; ++i) {
        Importance mi = (Importance)i;
        facility[mi].enable(imps.find(mi)!=imps.end());
    }
}

void Facilities::erase(Facility &facility) {
    FacilityMap map = facilities_;;
    for (FacilityMap::iterator fi=map.begin(); fi!=map.end(); ++fi) {
        if (fi->second == &facility)
            facilities_.erase(fi->first);
    }
}

void Facilities::reenable() {
    for (FacilityMap::iterator fi=facilities_.begin(); fi!=facilities_.end(); ++fi) {
        for (int i=0; i<N_IMPORTANCE; ++i) {
            Importance imp = (Importance)i;
            fi->second->get(imp).enable(impset_.find(imp)!=impset_.end());
        }
    }
}

void Facilities::reenableFrom(const Facilities &other) {
    for (FacilityMap::const_iterator fi_src=other.facilities_.begin(); fi_src!=other.facilities_.end(); ++fi_src) {
        FacilityMap::iterator fi_dst = facilities_.find(fi_src->first);
        if (fi_dst!=facilities_.end()) {
            for (int i=0; i<N_IMPORTANCE; ++i) {
                Importance imp = (Importance)i;
                fi_dst->second->get(imp).enable(fi_src->second->get(imp).enabled());
            }
        }
    }
}

void Facilities::enable(const std::string &switch_name, bool b) {
    FacilityMap::iterator found = facilities_.find(switch_name);
    if (found != facilities_.end()) {
        if (b) {
            for (int i=0; i<N_IMPORTANCE; ++i) {
                Importance imp = (Importance)i;
                found->second->get(imp).enable(impset_.find(imp)!=impset_.end());
            }
        } else {
            for (int i=0; i<N_IMPORTANCE; ++i)
                found->second->get((Importance)i).disable();
        }
    }
}
    
void Facilities::enable(Importance imp, bool b) {
    if (b) {
        impset_.insert(imp);
    } else {
        impset_.erase(imp);
    }
    for (FacilityMap::iterator fi=facilities_.begin(); fi!=facilities_.end(); ++fi)
        fi->second->get(imp).enable(b);
}

void Facilities::enable(bool b) {
    for (FacilityMap::iterator fi=facilities_.begin(); fi!=facilities_.end(); ++fi) {
        Facility *facility = fi->second;
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
}

std::string Facilities::ControlTerm::toString() const {
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
std::string Facilities::parseFacilityName(const char *&str) {
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
std::string Facilities::parseEnablement(const char *&str) {
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
std::string Facilities::parseRelation(const char *&str) {
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
std::string Facilities::parseImportanceName(const char *&str) {
    static const char *words[] = {"all", "none", "debug", "trace", "where", "info", "warn", "error", "fatal",
                                  "ALL", "NONE", "DEBUG", "TRACE", "WHERE", "INFO", "WARN", "ERROR", "FATAL"};
    static const size_t nwords = sizeof(words)/sizeof(words[0]);

    const char *s = str;
    while (isspace(*s)) ++s;
    for (size_t i=0; i<nwords; ++i) {
        size_t n = strlen(words[i]);
        if (0==strncmp(s, words[i], n) && !isalnum(s[n]) && '_'!=s[n]) {
            str += (s-str) + n;
            return words[i];
        }
    }
    return "";
}

Importance Facilities::importanceFromString(const std::string &str) {
    if (0==str.compare("debug") || 0==str.compare("DEBUG"))
        return DEBUG;
    if (0==str.compare("trace") || 0==str.compare("TRACE"))
        return TRACE;
    if (0==str.compare("where") || 0==str.compare("WHERE"))
        return WHERE;
    if (0==str.compare("info")  || 0==str.compare("INFO"))
        return INFO;
    if (0==str.compare("warn")  || 0==str.compare("WARN"))
        return WARN;
    if (0==str.compare("error") || 0==str.compare("ERROR"))
        return ERROR;
    if (0==str.compare("fatal") || 0==str.compare("FATAL"))
        return FATAL;
    abort();
}

// parses a StreamControlList. On success, returns a non-empty vector and adjust 'str' to point to the next character after the
// list.  On failure, throw a ControlError.
std::list<Facilities::ControlTerm> Facilities::parseImportanceList(const std::string &facilityName, const char *&str) {
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
            if (!enablement.empty() || !relation.empty())
                throw ControlError("message importance level expected", importanceStart);
            s = elmtStart;
            break;
        }

        ControlTerm term(facilityName, enablement.compare("!")!=0);
        if (0==importance.compare("all") || 0==importance.compare("none")) {
            if (!enablement.empty())
                throw ControlError("'"+importance+"' cannot be preceded by '"+enablement+"'", enablementStart);
            if (!relation.empty())
                throw ControlError("'"+importance+"' cannot be preceded by '"+relation+"'", relationStart);
            term.lo = DEBUG;
            term.hi = FATAL;
            term.enable = 0!=importance.compare("none");
        } else {
            Importance imp = importanceFromString(importance);
            if (relation.empty()) {
                term.lo = term.hi = importanceFromString(importance);
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

std::string Facilities::control(const std::string &ss) {
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
                if (facilities_.find(facilityName)==facilities_.end())
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
            FacilityMap::iterator found = facilities_.find(term.facilityName);
            assert(found!=facilities_.end() && found->second!=NULL);
            for (Importance imp=term.lo; imp<=term.hi; imp=(Importance)(imp+1))
                found->second->get(imp).enable(term.enable);
        }
    }

    return ""; // no errors
}

void Facilities::print(std::ostream &log) const {
    for (int i=0; i<N_IMPORTANCE; ++i) {
        Importance mi = (Importance)i;
        log <<(impset_.find(mi)==impset_.end() ? '-' : stringifyImportance(mi)[0]);
    }
    log <<" default enabled levels\n";

    if (facilities_.empty()) {
        log <<"no message facilities registered\n";
    } else {
        for (FacilityMap::const_iterator fi=facilities_.begin(); fi!=facilities_.end(); ++fi) {
            Facility *facility = fi->second;

            // A short easy to read format. Letters indicate the importances that are enabled; dashes keep them aligned.
            // Sort of like the format 'ls -l' uses to show permissions.
            for (int i=0; i<N_IMPORTANCE; ++i) {
                Importance mi = (Importance)i;
                log <<(facility->get(mi) ? stringifyImportance(mi)[0] : '-');
            }
            log <<" " <<fi->first <<"\n";
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DestinationPtr merr;
Facility log;
Facilities facilities;
bool isInitialized;

bool initializeLibrary() {
    if (!isInitialized) {
        isInitialized = true;
        merr = FdSink::instance(2);
        log = Facility("", merr);
        facilities.insert(log, "sawyer");
    }
    return true;
}

} // namespace
} // namespace
