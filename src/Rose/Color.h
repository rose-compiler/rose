#ifndef ROSE_Color_H
#define ROSE_Color_H
#include <RoseFirst.h>

#include <algorithm>
#include <Sawyer/CommandLine.h>
#include <Sawyer/Map.h>
#include <string>
#include <rosedll.h>

// Name space pollution cleanup
#ifdef _MSC_VER
  #undef RGB
#endif

namespace Rose {

/** Colors. */
namespace Color {

/** Whether colored output is enabled. */
enum class Enabled {
    OFF,                                                /**< Disable colored output. */
    ON,                                                 /**< Force colored output. */
    AUTO                                                /**< Use colored output if standard output is a terminal. */
};

/** Color theme.
 *
 *  Controls whether to use dark text on a light background, or light text on a dark background. */
enum class Theme {
    DARK_ON_LIGHT,                                      /**< Dark text on light background. */
    LIGHT_ON_DARK                                       /**< Light text on dark background. */
};

/** Control colored command output. */
struct Colorization {
    Sawyer::Optional<Enabled> enabled;                  /**< Whether colored output is enabled. */
    Sawyer::Optional<Theme> theme;                      /**< The color theme. */

    /** True if color is enabled in this situation. */
    bool isEnabled() const;

    /** Merge this color and another to produce a result. */
    Colorization merge(const Colorization&) const;
};

/** Layer to which color applies. */
enum class Layer {
    NONE,                                               /**< No specific layer. */
    FOREGROUND,                                         /**< Foreground colors. */
    BACKGROUND                                          /**< Background colors. */
};

/** Parses an output color specification.
 *
 *  This parser is used by @ref Sawyer::CommandLine when parsing command-line switches. For instance, many ROSE tools have a
 *  "--color" switch. The canonical documentation for this parser comes from its @ref docString function. In short, it parses
 *  one or two words separated by a comma, the words being when to use colorized output ("off", "on", "auto") and the basic
 *  theme ("dark", or "light"). */
class ColorizationParser: public Sawyer::CommandLine::ValueParser {
protected:
    ColorizationParser() {}
    ColorizationParser(const Sawyer::CommandLine::ValueSaver::Ptr &valueSaver)
        : Sawyer::CommandLine::ValueParser(valueSaver) {}

public:
    /** Shared ownership pointer to a @ref ColorizationParser. See @ref heap_object_shared_ownership. */
    typedef Sawyer::SharedPointer<ColorizationParser> Ptr;

    /** Allocating constructor. */
    static Ptr instance() {
        return Ptr(new ColorizationParser);
    }

    /** Allocating constructor. */
    static Ptr instance(const Sawyer::CommandLine::ValueSaver::Ptr &valueSaver) {
        return Ptr(new ColorizationParser(valueSaver));
    }

    /** Documentation for parser. */
    static std::string docString();

    /** Parse a colorized output specification from a C string.
     *
     *  Tries to parse a colorized output specification from the @p input string, and if successful adjusts @p rest to point to
     *  the first character beyond what was parsed. If a syntax error occurs, then an @c std::runtime_error is thrown. */
    static Colorization parse(const char *input, const char **rest);

    /** Parse a colorized output specification from a C++ string.
     *
     *  Tries to parse a colorized output specification from the @p input string. The string may contain leading and trailing
     *  white space, but any extra characters will cause a syntax error. Syntax errors are reported by throwing @c
     *  std::runtime_error.  Since the underlying parsing is done on C strings, this function is ill-defined when the @p input
     *  contains NUL bytes. */
    static Colorization parse(const std::string &input);

private:
    virtual Sawyer::CommandLine::ParsedValue
    operator()(const char *input, const char **rest, const Sawyer::CommandLine::Location &loc) /*override*/;
};

ColorizationParser::Ptr colorizationParser(Colorization &storage);
ColorizationParser::Ptr colorizationParser();

// Used internally to merge colorization command-line switch arguments.
class ColorizationMerge: public Sawyer::CommandLine::ValueAugmenter {
public:
    typedef Sawyer::SharedPointer<ColorizationMerge> Ptr;

    static Ptr instance() {
        return Ptr(new ColorizationMerge);
    }

    virtual Sawyer::CommandLine::ParsedValues
    operator()(const Sawyer::CommandLine::ParsedValues &prev, const Sawyer::CommandLine::ParsedValues &cur) /*OVERRIDE*/;
};

/** Type for color components.
 *
 *  ROSE doesn't do any heavy lifting of colors, so there's no need to distinguish between 8-bit colors, 16-bit colors, etc.
 *  Therefore we just use floating point for everything. */
typedef double Component;

/** Clip a floating point value between 0 and 1. */
template<typename T>
T clip(T c) {
    return std::max(T(0), std::min(c, T(1)));
}

class HSV;

/** Colors in RGB space. */
class ROSE_UTIL_API RGB {
    Component r_, g_, b_, a_;

public:
    /** Default constructed color.
     *
     *  Default constructed colors are useful when a color is stored in a container, but should not generally be used
     *  otherwise.  Therefore, a default constructed color will be bright red as a warning. */
    RGB(): r_(1.0), g_(0.0), b_(0.0), a_(1.0) {}        // bright red

    /** Construct a color specified by components.
     *
     *  The components should be each be between 0 and 1. Values outside the domain are clipped. */
    RGB(Component r, Component g, Component b, Component a=1.0): r_(clip(r)), g_(clip(g)), b_(clip(b)), a_(clip(a)) {}

    /** Convert an HSV color to RGB space. */
    RGB(const HSV&);                                    // implicit

    /** Component of color.
     *
     *  RGB components are named @c red, @c green, @c blue, and @c alpha (or just the first letter of each).
     *
     *  Each component is a floating-point value between zero and one.
     *
     * @{ */
    Component r() const { return r_; }
    Component g() const { return g_; }
    Component b() const { return b_; }
    Component a() const { return a_; }
    Component red() const { return r_; }
    Component green() const { return g_; }
    Component blue() const { return b_; }
    Component alpha() const { return a_; }
    /** @} */

    /** Convert to HTML string. */
    std::string toHtml() const;

    /** Convert to ANSI color escape. */
    std::string toAnsi(Layer) const;
};

/** Colors in HSV space.
 *
 *  Use HSV when possible since most manipulations of color are defined in this domain and therefore must otherwise be
 *  converted to/from the @ref RGB space. */
class ROSE_UTIL_API HSV {
    Component h_, s_, v_, a_;
public:

    /** Default constructed color.
     *
     *  Default constructed colors are useful when a color is stored in a container, but should not generally be used
     *  otherwise.  Therefore, a default constructed color will be bright red as a warning. */
    HSV(): h_(0.0), s_(1.0), v_(0.5), a_(1.0) {}        // bright red

    /** Construct a color from components.
     *
     *  The components are each values between zero and one. Values outside this domain are clipped. */
    HSV(Component h, Component s, Component v, Component a=1.0): h_(clip(h)), s_(clip(s)), v_(clip(v)), a_(clip(a)) {}

    /** Convert an RGB color to HSV space. */
    HSV(const RGB&);                                    // implicit

    /** Component of color.
     *
     *  HSV components are named @c hue, @c saturation, @c value, and @c alpha (or just the first letter of each).
     *
     *  Components are floating-point values between zero and one.
     *
     * @{ */
    Component h() const { return h_; }
    Component s() const { return s_; }
    Component v() const { return v_; }
    Component a() const { return a_; }
    Component hue() const { return h_; }
    Component saturation() const { return s_; }
    Component value() const { return v_; }
    Component alpha() const { return a_; }
    /** @} */

    /** Convert to HTML string. */
    std::string toHtml() const;

    /** Convert to ANSI color escape. */
    std::string toAnsi(Layer) const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Gradients

/** Mapping from floating point to color.
 *
 *  Maps a floating point value to a color by linear interpolation between specified colors. */
class ROSE_UTIL_API Gradient {
public:
    typedef Sawyer::Container::Map<double, HSV> ColorMap;

private:
    ColorMap colors_;
    HSV nanColor_;

public:
    /** Default constructor.
     *
     *  A default-constructed gradient maps all points in the domain to the default-constructed color. */
    Gradient() {}

    /** Construct a one-color gradient.
     *
     *  Implicit conversion from a color to a "constant gradient".  That's a bit of a misnomer, but it turns out to be useful
     *  for allowing a single color to be specified where a gradient is supported.
     *
     * @{ */
    Gradient(const RGB &color) { colors_.insert(0.0, color); } // implicit
    Gradient(const HSV &color) { colors_.insert(0.0, color); } // implicit
    /** @} */

    /** Construct a mapping with two colors.
     *
     *  Constructs a gradient that maps x less than or equal to zero to @p color1; x greater than or equal to one to @p
     *  color2, and interpolates between zero and one. */
    Gradient(const HSV &color1, const HSV &color2) {
        colors_.insert(0.0, color1);
        colors_.insert(1.0, color2);
    }
    
    /** Remove all points from the gradient. */
    void clear() { colors_.clear(); }

    /** Color for NaN lookups.
     *
     * @{ */
    const HSV& nanColor() const { return nanColor_; }
    void nanColor(const HSV &c) { nanColor_ = c; }
    /** @} */

    /** Insert a point into the gradient.
     *
     *  This method is just a wrapper around `this->colors().insert(where, color)` and is provided because it's a common
     *  operation. */
    void insert(double where, const HSV &color) { colors_.insert(where, color); }

    /** Return a color for the specified position.
     *
     *  If the color map is empty then a default-constructed color is returned.  If the color map has only one color then that
     *  color is returned.
     *
     * @{ */
    HSV interpolate(double) const;
    HSV operator()(double x) const { return interpolate(x); }
    /** @} */

    /** Reference to color map.
     *
     *  Returns a reference to the colors that are defined in this gradient.
     *
     * @{ */
    const ColorMap& colorMap() const { return colors_; }
    ColorMap& colorMap() { return colors_; }
    /** @} */
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Operations

/** Invert the brightness.
 *
 *  Light colors will become dark, dark colors will become light.  The hue, saturation and alpha are not affected. */
HSV invertBrightness(const HSV&);

/** Darken a color.
 *
 *  Makes a color darker according to @p amount, which should be in the range [0..1].  If @p amount is zero then the value is
 *  not changed, if @p amount is one then the color becomes black, others are interpolated linearly. This method does not
 *  affect the hue, saturation, or alpha. */
ROSE_UTIL_API HSV darken(const HSV&, double amount);

/** Lighten a color.
 *
 *  Makes a color lighter by adjusting its value upward by the specified ratio.  A value of zero does not affect the lightness
 *  while a value of one will make it white. Other values are linearly interpolated between those two points. */
ROSE_UTIL_API HSV lighten(const HSV&, double amount);

/** Make a color less saturated.
 *
 *  Makes a color less saturated according to @p amount. An amount of zero does not change the saturation, one removes all
 *  saturation, and other values are linearly interpolated between those two extremes. */
ROSE_UTIL_API HSV fade(const HSV&, double amount);

/** Adjust color for terminal.
 *
 *  Given a color suitable for a dark foreground on a light background (typical tty or printed output), convert the color as
 *  specified by the global color output command-line options. */
HSV terminal(const HSV &color, const Colorization&);

/** Create an HTML color string.
 *
 *  HTML color strings are of the form "#RRGGBB". */
ROSE_UTIL_API std::string toHtml(const RGB&);

/** Create an ANSI color escape.
 *
 * These characters can be emitted to an ANSI terminal after the appropriate escape sequence in order to change the foreground
 * or background color.  The escape sequence is usually "\033[38;2;" for the foreground and "\033[48;2;" for the
 * background. The foreground and background colors can be canceled by emitting "\033[0m". If a layer is specified, then the
 * escape sequence is part of the return value. ANSI doesn't support alphas, so if the alpha is less than 0.5 the return value
 * is an empty string. */
ROSE_UTIL_API std::string toAnsi(const RGB&, Layer);

// printing
std::ostream& operator<<(std::ostream&, const RGB&);
std::ostream& operator<<(std::ostream&, const HSV&);
std::ostream& operator<<(std::ostream&, const Gradient&);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Predefined colors

extern const HSV HSV_CLEAR;                             // my favorite color ;-)
extern const HSV HSV_BLACK;                             /**< Black. */
extern const HSV HSV_WHITE;                             /**< White. */
extern const HSV HSV_RED;                               /**< Red. */
extern const HSV HSV_GREEN;                             /**< Green. */
extern const HSV HSV_BLUE;                              /**< Blue. */
extern const HSV HSV_CYAN;                              /**< Cyan. */
extern const HSV HSV_MAGENTA;                           /**< Magenta. */
extern const HSV HSV_YELLOW;                            /**< Yellow. */
extern const HSV HSV_GRAY;                              /**< Gray. */

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** ANSI color names for terminal output. */
enum class AnsiColor {
    CLEAR,                                              /**< Clear. */
    RED,                                                /**< Red. */
    GREEN,                                              /**< Green. */
    YELLOW,                                             /**< Yellow. */
    BLUE,                                               /**< Blue. */
    MAGENTA,                                            /**< Magenta. */
    CYAN,                                               /**< Cyan. */
    GRAY                                                /**< Gray. */
};

/** Convert a color enum to a string. */
std::string colorName(AnsiColor);

/** ANSI color escape for terminal output. Indexed by ColorName. */
std::string ansiColorEscape(AnsiColor);

} // namespace
} // namespace

#endif
