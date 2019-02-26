#ifndef ROSE_Color_H
#define ROSE_Color_H

#include <algorithm>
#include <Sawyer/Map.h>
#include <string>
#include <rosedll.h>

namespace Rose {

/** Colors. */
namespace Color {

/** Type for color components.
 *
 *  FIXME[Robb P. Matzke 2014-12-19]: This should ideally be a template parameter so we can support 8-bit color, 16-bit color,
 *  floating point color using float or double, etc.  At the moment, we'll use double since I don't expect that ROSE will be
 *  doing any heavy lifting of colors; i.e., no large image arrays, just color configuration kinds of things. */
typedef double Component;


/** Clip a floating point value between 0 and 1. */
template<typename T>
T clip(T c) {
    return std::max(T(0), std::min(c, T(1)));
}

class HSV;
#ifdef _MSC_VER
  #undef RGB
#endif
/** Colors in RGB space. */
class ROSE_UTIL_API RGB {
    Component r_, g_, b_, a_;

public:
    /** Default constructed color.
     *
     *  Default constructed colors are useful when a color is stored in a container, but should not generally be used
     *  otherwise.  Therefore, a default constructed color will be bright red as a warning. */
    RGB(): r_(1.0), g_(0.0), b_(0.0), a_(1.0) {}        // bright red

    /** Construct a color specified by components. */
    RGB(Component r, Component g, Component b, Component a=1.0): r_(clip(r)), g_(clip(g)), b_(clip(b)), a_(clip(a)) {}

    /** Convert an HSV color to RGB space. */
    RGB(const HSV&);                                    // implicit

    /** Component of color.
     *
     *  RGB components are named @c red, @c green, @c blue, and @c alpha (or just the first letter of each).
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
};

/** Colors in HSV space.
 *
 *  Use this one when possible since most manipulations of color are defined in this domain and therefore must otherwise be
 *  converted to/from the RGB space. */
class ROSE_UTIL_API HSV {
    Component h_, s_, v_, a_;
public:

    /** Default constructed color.
     *
     *  Default constructed colors are useful when a color is stored in a container, but should not generally be used
     *  otherwise.  Therefore, a default constructed color will be bright red as a warning. */
    HSV(): h_(0.0), s_(1.0), v_(0.5), a_(1.0) {}        // bright red

    /** Construct a color from components. */
    HSV(Component h, Component s, Component v, Component a=1.0): h_(clip(h)), s_(clip(s)), v_(clip(v)), a_(clip(a)) {}

    /** Convert an RGB color to HSV space. */
    HSV(const RGB&);                                    // implicit

    /** Component of color.
     *
     *  HSV components are named @c hue, @c saturation, @c value, and @c alpha (or just the first letter of each).
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
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Gradients

/** Mapping from floating point to color.
 *
 *  Maps a floating point value in the range [0..1] to a color by linear interpolation between specified colors. */
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
     *  Constructs a gradient that maps x less than or equal to zero to @ref color1; x greater than or equal to one to @ref
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
     *  This method is just a wrapper around <code>this->colors().insert(where, color)</code> and is provided because it's a
     *  common operation. */
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

/** Create an HTML color string.
 *
 *  HTML color strings are of the form "#RRGGBB". */
ROSE_UTIL_API std::string toHtml(const RGB&);

// printing
std::ostream& operator<<(std::ostream&, const RGB&);
std::ostream& operator<<(std::ostream&, const HSV&);
std::ostream& operator<<(std::ostream&, const Gradient&);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Predefined colors

extern const HSV HSV_CLEAR;                             // my favorite color ;-)
extern const HSV HSV_BLACK;
extern const HSV HSV_WHITE;
extern const HSV HSV_RED;
extern const HSV HSV_GREEN;
extern const HSV HSV_BLUE;
extern const HSV HSV_CYAN;
extern const HSV HSV_MAGENTA;
extern const HSV HSV_YELLOW;
extern const HSV HSV_GRAY;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** ANSI color names for terminal output. */
enum AnsiColor { ANSI_CLEAR, ANSI_RED, ANSI_GREEN, ANSI_YELLOW, ANSI_BLUE, ANSI_MAGENTA, ANSI_CYAN, ANSI_GRAY };

/** Convert a color enum to a string. */
std::string colorName(AnsiColor);

/** ANSI color escape for terminal output. Indexed by ColorName. */
std::string ansiColorEscape(AnsiColor);

} // namespace
} // namespace

#endif
