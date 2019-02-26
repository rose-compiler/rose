#include <rose.h>
#include <bROwSE/WAddressSpace.h>

#include <Wt/WPaintDevice>
#include <Wt/WPaintedWidget>
#include <Wt/WPainter>

using namespace Rose;
using namespace Rose::BinaryAnalysis;

namespace bROwSE {

class MyPainterWidget: public Wt::WPaintedWidget {
    WAddressSpace *parent_;

public:
    explicit MyPainterWidget(WAddressSpace *parent)
        : Wt::WPaintedWidget(parent), parent_(parent) {}

    void paintEvent(Wt::WPaintDevice *paintDevice) ROSE_OVERRIDE {
        parent_->redraw(paintDevice);
    }
};

void
WAddressSpace::init() {
    painterWidget_ = new MyPainterWidget(this);
    painterWidget_->resize(totalWidth(), totalHeight());
    painterWidget_->clicked().connect(this, &WAddressSpace::performClickAction);
    painterWidget_->mouseMoved().connect(this, &WAddressSpace::trackMotion);
    trackMotion(Wt::WMouseEvent());
}

void
WAddressSpace::performClickAction(const Wt::WMouseEvent &event) {
    const int topWidgetTop = topMargin_;
    const int barGraphTop = topWidgetTop + topGutterHeight_;
    const int bottomWidgetTop = barGraphTop + barHeight_;
    const int bottom = bottomWidgetTop + bottomGutterHeight_;
    rose_addr_t va = 0;

    if (event.widget().y >= topWidgetTop && event.widget().y < barGraphTop) {
        // Clicked in the top gutter
        if (widgetToVa(event.widget().x, event.widget().y).assignTo(va))
            topGutterClicked_.emit(va, event);

    } else if (event.widget().y >= barGraphTop && event.widget().y < bottomWidgetTop) {
        // Clicked in the bar graph
        zoomInOut(event);
        double mapHeight = (double)barHeight_ / maps_.size();
        size_t idx = floor((event.widget().y - barGraphTop) / mapHeight);
        ASSERT_require(idx < maps_.size());
        if (widgetToVa(event.widget().x, event.widget().y).assignTo(va))
            barGraphClicked_.emit(va, idx, event);
        
    } else if (event.widget().y >= bottomWidgetTop && event.widget().y < bottom) {
        // Clicked in the bottom gutter
        if (widgetToVa(event.widget().x, event.widget().y).assignTo(va))
            bottomGutterClicked_.emit(va, event);
    }
}

void
WAddressSpace::zoomInOut(const Wt::WMouseEvent &event) {
    rose_addr_t va = 0;
    if (widgetToVa(event.widget().x, event.widget().y).assignTo(va)) {
        AddressInterval dom = displayedDomain();
        AddressInterval newDom;
        double zoomFactor = 0.75;                           // amount by which to multiply the domain width when zooming in
        if (event.modifiers() == Wt::ControlModifier) {
            // zoom out
            AddressInterval whole = fullDomain();
            rose_addr_t newSize = whole.size();
            if ((rose_addr_t)round(newSize*zoomFactor) > dom.size()) // careful of overflow
                newSize = round(dom.size() / zoomFactor);
            newDom = centerInterval(va, newSize, whole);
        } else if (event.modifiers() == (Wt::ControlModifier | Wt::ShiftModifier)) {
            // zoom all the way out
            newDom = fullDomain();
        } else if (event.modifiers() == Wt::NoModifier) {
            // zoom in
            if (rose_addr_t newSize = round(dom.size() * zoomFactor))
                newDom = centerInterval(va, newSize, dom);
        }
        if (!newDom.isEmpty()) {
            displayedDomain(newDom);
            redraw();
        }
    }
}

void
WAddressSpace::trackMotion(const Wt::WMouseEvent &event) {
    rose_addr_t va = 0;
    if (widgetToVa(event.widget().x, event.widget().y).assignTo(va)) {
        painterWidget_->setToolTip(StringUtility::addrToString(va));
    } else {
        painterWidget_->setToolTip("Click zoom in; Ctrl zoom out; Shift+Ctrl reset");
    }
}

AddressInterval
WAddressSpace::centerInterval(rose_addr_t center, rose_addr_t size, const AddressInterval &limits) {
    // WARNING: be very careful of unsigned overflows!
    rose_addr_t lo=limits.least(), hi=limits.greatest();
    rose_addr_t halfSize = size/2;
    center = std::max(lo, std::min(center, hi));        // center must be within limits
    if (center<=halfSize || center-halfSize<=lo) {
        // keep lo where it is and possibly move hi down
        if (hi-lo > size)
            hi = lo + size;
    } else if (hi<=halfSize || center>=hi-halfSize) {
        // keep hi where it is and possibly move lo up
        if (hi-lo > size)
            lo = hi - size;
    } else {
        // move lo up and hi down
        lo = center - halfSize;
        hi = center + halfSize;
    }
    return AddressInterval::hull(lo, hi);
}

void
WAddressSpace::insertSegmentsAndFunctions(const P2::Partitioner &partitioner) {
    if (!partitioner.isDefaultConstructed()) {
        // Add the address map segments and indicate their starting positions in the top gutter.
        properties(0).pen = Wt::WPen(Wt::NoPen);
        insert(partitioner.memoryMap(), 0);

        // Add function extents and mark their start in the bottom gutter. Adjacent functions will merge into one colored area and
        // we don't try to separate them with
        insert(partitioner, partitioner.functions(), 1);
        Color::HSV functionColor = darken(Color::HSV_CYAN, 0.25);
        gradient(1).insert(0, functionColor);
        bottomGutterGradient().insert(0, functionColor);
        properties(1).pen = Wt::WPen(Wt::NoPen);
    }
}

size_t
WAddressSpace::insert(const MemoryMap::Ptr &mm, size_t idx, bool showStarts) {
    if ((size_t)-1 == idx)
        idx = maps_.size();

    // Colors
    gradient(idx).clear();
    gradient(idx).insert(0.00, darken(Color::HSV_RED, 0.25));               // no permissions               red
    gradient(idx).insert(0.05, fade(darken(Color::HSV_RED, 0.25), 0.75));   // write-only                   faded red
    gradient(idx).insert(0.10, darken(Color::HSV_GREEN, 0.15));             // executable                   green
    gradient(idx).insert(0.15, fade(darken(Color::HSV_GREEN, 0.15), 0.75)); // executable + writable        faded green
    gradient(idx).insert(0.20, Color::RGB(0.9, 0.8, 0));                    // read-only                    yellow
    gradient(idx).insert(0.25, fade(Color::RGB(0.9, 0.8, 0), 0.75));        // read + writable              faded yellow

    HeatMap &gutter = 0==idx ? topGutterMap() : bottomGutterMap();
    Color::Gradient &gutterGradient = 0==idx ? topGutterGradient() : bottomGutterGradient();
    if (showStarts) {
        gutter.clear();
        gutterGradient = gradient(idx);
    }
    
    BOOST_FOREACH (const MemoryMap::Node &node, mm->nodes()) {
        const AddressInterval &interval = node.key();
        const MemoryMap::Segment &segment = node.value();
        double g = 0;                                   // gradient position
        if ((segment.accessibility() & MemoryMap::EXECUTABLE) != 0) {
            g = 0.10;
        } else if (segment.accessibility() & MemoryMap::READABLE) {
            g = 0.20;
        }
        if ((segment.accessibility() & MemoryMap::WRITABLE) != 0)
            g += 0.05;
        map(idx).insert(interval, g);
        if (showStarts)
            gutter.insert(interval.least(), g);
    }
    return idx;
}

size_t
WAddressSpace::insert(const P2::Partitioner &partitioner, const std::vector<P2::Function::Ptr> &functions,
                      size_t idx, bool showStarts) {
    if ((size_t)-1 == idx)
        idx = maps_.size();

    HeatMap &gutter = 0==idx ? topGutterMap() : bottomGutterMap();
    BOOST_FOREACH (const P2::Function::Ptr &function, functions) {
        AddressIntervalSet whereDefined = partitioner.functionExtent(function);
        BOOST_FOREACH (const AddressInterval &interval, whereDefined.intervals()) {
            double value = function->attributeOrElse(ATTR_Heat, NAN);
            map(idx).insert(interval, value);
            if (showStarts)
                gutter.insert(function->address(), value);
        }
    }
    return idx;
}

int
WAddressSpace::barGraphLeft() const {
    return !fullDomain().isEmpty() && fullDomain().least() < displayedDomain().least() ? 0 : horizontalMargin_;
}

int
WAddressSpace::barGraphRight() const {
    return !fullDomain().isEmpty() && fullDomain().greatest() > displayedDomain().greatest() ?
        totalWidth() : totalWidth()-horizontalMargin_;
}

void
WAddressSpace::redraw() {
    painterWidget_->update();
}

void
WAddressSpace::redraw(Wt::WPaintDevice *paintDevice) {
    Wt::WPainter painter(paintDevice);

    int barLeft = barGraphLeft();
    int barRight = barGraphRight();
    int barWidth = barRight - barLeft;
    int barTop = topMargin_ + topGutterHeight_;

    if (borderColor().alpha() > 0.0)
        painter.setPen(Wt::WPen(toWt(borderColor())));

    // Draw the background for the map area.
    if (bgColor().alpha() > 0.0)
        painter.fillRect(barLeft, barTop, barWidth, barHeight_, Wt::WBrush(toWt(bgColor())));

    // Draw the box around the whole widget.
    if (borderColor().alpha() > 0.0) {
        painter.setPen(Wt::WPen(toWt(borderColor())));
        painter.drawRect(0, 0, totalWidth(), totalHeight());
    }

    if (!maps_.isEmpty()) {
        const AddressInterval dom = displayedDomain();

        // Draw each bar
        const double mapHeight = (double)barHeight_ / maps_.size();
        for (size_t i=0; i<maps_.size(); ++i) {
            // The filled area
            painter.setPen(properties(i).pen);
            const double y = barTop + i * mapHeight;
            BOOST_FOREACH (const WAddressSpace::HeatMap::Node &node, map(i).nodes()) {
                painter.setBrush(Wt::WBrush(toWt(gradient(i)(node.value()))));
                double x = barWidth * ((double)node.key().least() - dom.least()) / dom.size() + barLeft;
                double w = barWidth * (double)node.key().size() / dom.size();
                painter.drawRect(x, y, w, mapHeight);
            }

            // The highlight markers
            painter.setPen(properties(i).highlightPen);
            painter.setBrush(Wt::WBrush(Wt::NoBrush));
            BOOST_FOREACH (const AddressInterval &interval, highlights(i).intervals()) {
                double yh = y + mapHeight/2.0;
                double x = barWidth * ((double)interval.least() - dom.least()) / dom.size() + barLeft;
                double w = barWidth * (double)interval.size() / dom.size();
                painter.drawLine(x, yh, x+w, yh);
            }
        }

        // Top gutter
        BOOST_FOREACH (const WAddressSpace::HeatMap::Node &node, topGutterMap().nodes()) {
            Wt::WPainterPath path;
            double x = barWidth * ((double)node.key().least() - dom.least()) / dom.size() + barLeft;
            double y = topMargin_ + topGutterHeight_;
            path.moveTo(x, y);
            path.lineTo(x+topGutterArrowWidth_, y-topGutterHeight_);
            path.lineTo(x-topGutterArrowWidth_, y-topGutterHeight_);
            path.closeSubPath();
            painter.fillPath(path, Wt::WBrush(toWt(topGutterGradient()(node.value()))));
        }

        // Bottom gutter
        BOOST_FOREACH (const WAddressSpace::HeatMap::Node &node, bottomGutterMap().nodes()) {
            Wt::WPainterPath path;
            double x = barWidth * ((double)node.key().least() - dom.least()) / dom.size() + barLeft;
            double y = topMargin_ + topGutterHeight_ + barHeight_;
            path.moveTo(x, y);
            path.lineTo(x+bottomGutterArrowWidth_, y+bottomGutterHeight_);
            path.lineTo(x-bottomGutterArrowWidth_, y+bottomGutterHeight_);
            path.closeSubPath();
            painter.fillPath(path, Wt::WBrush(toWt(bottomGutterGradient()(node.value()))));
        }
    }
}

AddressInterval
WAddressSpace::displayedDomain() const {
    return displayedDomain_.isEmpty() ? fullDomain() : displayedDomain_;
}

AddressInterval
WAddressSpace::fullDomain() const {
    AddressInterval retval;
    BOOST_FOREACH (const HeatMap &map, maps_.values()) {
        if (retval.isEmpty()) {
            retval = map.hull();
        } else {
            retval = AddressInterval::hull(std::min(retval.least(), map.hull().least()),
                                           std::max(retval.greatest(), map.hull().greatest()));
        }
    }
    return retval;
}

Sawyer::Optional<rose_addr_t>
WAddressSpace::widgetToVa(int x, int y) const {
    AddressInterval dom = displayedDomain();
    if (dom.isEmpty())
        return Sawyer::Nothing();
    const int barLeft = barGraphLeft();
    const int barRight = barGraphRight();
    const int barWidth = barRight - barLeft;
    if (x < barLeft || x >= barRight)
        return Sawyer::Nothing();                       // not inside the bar
    rose_addr_t va = dom.least() + dom.size()*(double)(x - barLeft)/barWidth;
    return va;
}

void
WAddressSpace::instantiateSubBar(size_t idx) {
    for (size_t i=maps_.size(); i<=idx; ++i) {
        maps_.pushBack(HeatMap());
        ASSERT_require(maps_.backNode().id() == i);
        gradients_.pushBack(Color::Gradient());
        properties_.pushBack(Properties());
        properties_.backValue().highlightPen = Wt::WPen(Wt::yellow);
        highlights_.pushBack(AddressIntervalSet());
    }
}

WAddressSpace::HeatMap&
WAddressSpace::map(size_t idx) {
    instantiateSubBar(idx);
    return maps_[idx];
}

Color::Gradient&
WAddressSpace::gradient(size_t idx) {
    instantiateSubBar(idx);
    return gradients_[idx];
}

WAddressSpace::Properties&
WAddressSpace::properties(size_t idx) {
    instantiateSubBar(idx);
    return properties_[idx];
}

AddressIntervalSet&
WAddressSpace::highlights(size_t idx) {
    instantiateSubBar(idx);
    return highlights_[idx];
}

void
WAddressSpace::clear() {
    displayedDomain_ = AddressInterval();
    maps_.clear();
    gradients_.clear();
}

} // namespace

