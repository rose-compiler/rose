#include <Color.h>

#include <boost/algorithm/string/trim.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <cmath>
#include <CommandLine.h>
#include <cstdio>
#include <ctype.h>
#include <rose_isnan.h>
#include <StringUtility.h>

// Name space pollution cleanup
#ifdef _MSC_VER
  #undef RGB
#endif

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

std::string RGB::toAnsi(Layer::Flag layer) const {
    return Color::toAnsi(*this, layer);
}

std::string HSV::toAnsi(Layer::Flag layer) const {
    return Color::toAnsi(*this, layer);
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

HSV terminal(const HSV &color, const Colorization &output) {
    if (!output.isEnabled()) {
        return HSV_CLEAR;
    } else if (Theme::LIGHT_ON_DARK == output.theme.orElse(Theme::DARK_ON_LIGHT)) {
        return invertBrightness(color);
    } else {
        return color;
    }
}

std::string toHtml(const RGB &rgb) {
    // Microsoft doesn't define round(double) in <cmath>
    unsigned r = boost::numeric::converter<unsigned, double>::convert(clip(rgb.r())*255);
    unsigned g = boost::numeric::converter<unsigned, double>::convert(clip(rgb.g())*255);
    unsigned b = boost::numeric::converter<unsigned, double>::convert(clip(rgb.b())*255);

    return (boost::format("#%02x%02x%02x") % r % g % b).str();
}

std::string toAnsi(const RGB &rgb, Layer::Flag layer) {
    std::string retval;
    // ANSI doesn't support alpha, so we treat the color as either clear or opaque.
    if (rgb.a() >= 0.5) {
        if (Layer::FOREGROUND == layer) {
            retval = "\033[38;2;";
        } else if (Layer::BACKGROUND == layer) {
            retval = "\033[48;2;";
        }

        // Microsoft doesn't define round(double) in <cmath>
        unsigned r = boost::numeric::converter<unsigned, double>::convert(clip(rgb.r())*255);
        unsigned g = boost::numeric::converter<unsigned, double>::convert(clip(rgb.g())*255);
        unsigned b = boost::numeric::converter<unsigned, double>::convert(clip(rgb.b())*255);
        retval += (boost::format("%d;%d;%dm") % r % g % b).str();
    }
    return retval;
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Parsing

bool
Colorization::isEnabled() const {
    switch (enabled.orElse(Enabled::AUTO)) {
        case Enabled::ON:
            return true;
        case Enabled::OFF:
            return false;
        case Enabled::AUTO:
#ifdef _MSC_VER
            return false;
#else
            return 1 == isatty(1);
#endif
    }
    ASSERT_not_reachable("invalid Rose::Color::Colorization::enabled value");
}

Colorization
Colorization::merge(const Colorization &other) const {
    Colorization retval;
    retval.enabled = enabled.orElse(other.enabled);
    retval.theme = theme.orElse(other.theme);
    return retval;
}

std::string
ColorizationParser::docString() {
    return ("The argument is one or two words separated by a comma. If the word is \"off\", \"on\", or \"auto\" "
            "then output colorization is turned off, turned on, or made contingent upon standard output being a "
            "terminal. The words \"dark\" and \"light\" specify the theme: whether colors should be dark text on "
            "a light background, or light text on a dark background, respectively.");
}

Sawyer::CommandLine::ParsedValue
ColorizationParser::operator()(const char *input, const char **rest, const Sawyer::CommandLine::Location &loc) {
    Colorization val = parse(input, rest);
    std::string parsed(input, *rest-input);
    return Sawyer::CommandLine::ParsedValue(val, loc, parsed, valueSaver());
}

Colorization
ColorizationParser::parse(const char *input, const char **rest) {
    // Input is words (including white space) separated by commas.
    const char *end = input;
    while (isalnum(*end) || ' ' == *end || '\t' == *end || ',' == *end)
        ++end;
    std::string parsed(input, end);
    Colorization retval = parse(parsed);
    if (rest)
        *rest = end;
    return retval;
}

ColorizationParser::Ptr
colorizationParser(Colorization &storage) {
    return ColorizationParser::instance(Sawyer::CommandLine::TypedSaver<Colorization>::instance(storage));
}

Colorization
ColorizationParser::parse(const std::string &input) {
    Colorization retval;
    std::vector<std::string> words = StringUtility::split(",", input);

    BOOST_FOREACH (std::string word, words) {
        word = boost::trim_copy(word);
        if ("off" == word) {
            retval.enabled = Enabled::OFF;
        } else if ("on" == word) {
            retval.enabled = Enabled::ON;
        } else if ("auto" == word) {
            retval.enabled = Enabled::AUTO;
        } else if ("light" == word) {
            retval.theme = Theme::LIGHT_ON_DARK;
        } else if ("dark" == word) {
            retval.theme = Theme::DARK_ON_LIGHT;
        } else {
            throw std::range_error("invalid word \"" + StringUtility::cEscape(word) + "\"" +
                                   " when parsing \"" + StringUtility::cEscape(input) + "\"");
        }
    }
    return retval;
}

Sawyer::CommandLine::ParsedValues
ColorizationMerge::operator()(const Sawyer::CommandLine::ParsedValues &prev,
                              const Sawyer::CommandLine::ParsedValues &cur) {
    ASSERT_forbid(prev.empty());
    Colorization merged;
    BOOST_FOREACH (const Sawyer::CommandLine::ParsedValue &pv, prev) {
        const Colorization c = boost::any_cast<Colorization>(pv.value());
        if (c.enabled)
            merged.enabled = *c.enabled;
        if (c.theme)
            merged.theme = *c.theme;
    }
    BOOST_FOREACH (const Sawyer::CommandLine::ParsedValue &pv, cur) {
        const Colorization c = boost::any_cast<Colorization>(pv.value());
        if (c.enabled)
            merged.enabled = *c.enabled;
        if (c.theme)
            merged.theme = *c.theme;
    }
    Sawyer::CommandLine::ParsedValue pval = cur.front(); // so we keep the same location information
    pval.value(merged);
    Sawyer::CommandLine::ParsedValues pvals;
    pvals.push_back(pval);
    return pvals;
}

} // namespace
} // namespace
