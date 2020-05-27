#include <cstdio>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <Color.h>
#include <cmath>
#include <rose_isnan.h>

namespace Rose {
namespace Color {

// Predefined colors names
const HSV HSV_CLEAR  (0.0, 0.0, 0.0, 0.0);
const HSV HSV_BLACK  (0.0, 0.0, 0.0);
const HSV HSV_WHITE  (0.0, 0.0, 1.0);
const HSV HSV_GRAY   (0.0, 0.0, 0.5);

const HSV HSV_RED    (0.0,   1.0, 0.5);
const HSV HSV_YELLOW (1/6.0, 1.0, 0.5);
const HSV HSV_GREEN  (2/6.0, 1.0, 0.5);
const HSV HSV_CYAN   (3/6.0, 1.0, 0.5);
const HSV HSV_BLUE   (4/6.0, 1.0, 0.5);
const HSV HSV_MAGENTA(5/6.0, 1.0, 0.5);

// Internal function used when converting HSV to RGB.  Borrowed with permission from Robb's Beenav navigation software [Robb
// P. Matzke 2014-12-02]
static Component
rgb_from_hue(Component p, Component q, Component t)
{
    if (t<0)
        t += 1.0;
    if (t>1)
        t -= 1.0;
    if (t < 1/6.0)
        return p + (q-p) * 6 * t;
    if (t < 0.5)
        return q;
    if (t < 2/3.0)
        return p + (q-p) * (2/3.0 - t) * 6;
    return p;
}

// Borrowed with permission from Robb's Beenav navigation software [Robb P. Matzke 2014-12-02]
RGB::RGB(const HSV &hsv) {
    a_ = hsv.a();
    if (0.0==hsv.v()) {
        r_ = g_ = b_ = 0;
    } else {
        Component q = hsv.v() < 0.5 ? hsv.v() * (1+hsv.s()) : hsv.v() + hsv.s() - hsv.v()*hsv.s();
        Component p = 2.0 * hsv.v() - q;
        r_ = rgb_from_hue(p, q, hsv.h()+1/3.0);
        g_ = rgb_from_hue(p, q, hsv.h());
        b_ = rgb_from_hue(p, q, hsv.h()-1/3.0);
    }
}

// Borrowed with permission from Robb's Beenav navigation software [Robb P. Matzke 2014-12-02]
HSV::HSV(const RGB &rgb) {
    Component hi = std::max(std::max(rgb.r(), rgb.g()), rgb.b());
    Component lo = std::min(std::min(rgb.r(), rgb.g()), rgb.b());
    a_ = rgb.a();
    v_ = (hi+lo)/2.0;
    if (hi==lo) {
        h_ = s_ = 0.0;
    } else {
        Component delta = hi - lo;
        s_ = v_ > 0.5 ? delta / (2.0-hi-lo) : delta / (hi+lo);
        if (hi == rgb.r()) {
            h_ = (rgb.g()-rgb.b())/delta + (rgb.g()<rgb.b() ? 6 : 0);
        } else if (hi == rgb.g()) {
            h_ = (rgb.b()-rgb.r())/delta + 2;
        } else {
            h_ = (rgb.r()-rgb.g())/delta + 4;
        }
        h_ /= 6.0;
    }
}

std::string RGB::toHtml() const {
    return Color::toHtml(*this);
}

std::string HSV::toHtml() const {
    return Color::toHtml(*this);
}

std::string RGB::toAnsi() const {
    return Color::toAnsi(*this);
}

std::string HSV::toAnsi() const {
    return Color::toAnsi(*this);
}

HSV invertBrightness(const HSV &hsv) {
    return HSV(hsv.h(), hsv.s(), (1.0-hsv.v()), hsv.a());
}

HSV darken(const HSV &hsv, double amount) {
    double v = clip(hsv.v() - hsv.v()*amount);
    return HSV(hsv.h(), hsv.s(), v, hsv.a());
}

HSV lighten(const HSV &hsv, double amount) {
    double v = clip((1.0 - hsv.v())*amount + hsv.v());
    return HSV(hsv.h(), hsv.s(), v, hsv.a());
}

HSV fade(const HSV &hsv, double amount) {
    double s = clip(hsv.s() - hsv.s()*amount);
    return HSV(hsv.h(), s, hsv.v(), hsv.a());
}

std::string toHtml(const RGB &rgb) {
    // Microsoft doesn't define round(double) in <cmath>
    unsigned r = boost::numeric::converter<unsigned, double>::convert(clip(rgb.r())*255);
    unsigned g = boost::numeric::converter<unsigned, double>::convert(clip(rgb.g())*255);
    unsigned b = boost::numeric::converter<unsigned, double>::convert(clip(rgb.b())*255);

    return (boost::format("#%02x%02x%02x") % r % g % b).str();
}

std::string toAnsi(const RGB &rgb) {
    // Microsoft doesn't define round(double) in <cmath>
    unsigned r = boost::numeric::converter<unsigned, double>::convert(clip(rgb.r())*255);
    unsigned g = boost::numeric::converter<unsigned, double>::convert(clip(rgb.g())*255);
    unsigned b = boost::numeric::converter<unsigned, double>::convert(clip(rgb.b())*255);

    return (boost::format("%d;%d;%dm") % r % g % b).str();
}

HSV
Gradient::interpolate(double x) const {
    if (colors_.isEmpty()) {
      return HSV();
    } else if(rose_isnan(x)) {
      return nanColor_;
    } else if (x <= colors_.least()) {
      return *colors_.values().begin();
    } else if (x >= colors_.greatest()) {
        ColorMap::ConstValueIterator iter = colors_.values().end();
        --iter;
        return *iter;
    } else {
        ColorMap::ConstNodeIterator hi = colors_.lowerBound(x);
        ASSERT_forbid(hi == colors_.nodes().end());     // would have been found by x>=colors_.greatest()
        ASSERT_forbid(hi == colors_.nodes().begin());   // would have been found by x<=colors_.least()
        ColorMap::ConstNodeIterator lo = hi; --lo;
        double interpolant = (x - lo->key()) / (hi->key() - lo->key());
        const HSV &c1 = lo->value();
        const HSV &c2 = hi->value();
        return HSV(c1.h() + interpolant * (c2.h()-c1.h()),
                   c1.s() + interpolant * (c2.s()-c1.s()),
                   c1.v() + interpolant * (c2.v()-c1.v()),
                   c1.a() + interpolant * (c2.a()-c1.a()));
    }
}

std::ostream&
operator<<(std::ostream &out, const RGB &color) {
    out <<"RGB(" <<color.r() <<", " <<color.g() <<", " <<color.b() <<", " <<color.a() <<")";
    return out;
}

std::ostream&
operator<<(std::ostream &out, const HSV &color) {
    out <<"HSV(" <<color.h() <<", " <<color.s() <<", " <<color.v() <<", " <<color.a() <<")";
    return out;
}

std::ostream&
operator<<(std::ostream &out, const Gradient &gradient) {
    out <<"Gradient(";
    size_t n = 0;
    BOOST_FOREACH (const Gradient::ColorMap::Node &node, gradient.colorMap().nodes())
        out <<(1==++n?"":", ") <<node.key() <<"=>" <<node.value();
    out <<")";
    return out;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Ansi

std::string
colorName(AnsiColor c) {
    switch (c) {
        case ANSI_CLEAR:   return "clear";
        case ANSI_RED:     return "red";
        case ANSI_GREEN:   return "green";
        case ANSI_YELLOW:  return "yellow";
        case ANSI_BLUE:    return "blue";
        case ANSI_MAGENTA: return "magenta";
        case ANSI_CYAN:    return "cyan";
        case ANSI_GRAY:    return "gray";
    }
    ASSERT_not_reachable("invalid ANSI color");
}

std::string
ansiColorEscape(AnsiColor c) {
    switch (c) {
        case ANSI_CLEAR:   return "";
        case ANSI_RED:     return "\033[31m";
        case ANSI_GREEN:   return "\033[32m";
        case ANSI_YELLOW:  return "\033[33m";
        case ANSI_BLUE:    return "\033[34m";
        case ANSI_MAGENTA: return "\033[35m";
        case ANSI_CYAN:    return "\033[36m";
        case ANSI_GRAY:    return "\033[38;5;244m";
    }
    return "";
}

} // namespace
} // namespace
