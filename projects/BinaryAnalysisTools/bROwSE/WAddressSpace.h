#ifndef bROwSE_WAddressSpace_H
#define bROwSE_WAddressSpace_H

#include <Color.h>
#include <bROwSE/bROwSE.h>
#include <Sawyer/IndexedList.h>
#include <Sawyer/Optional.h>
#include <Wt/WContainerWidget>
#include <Wt/WPen>

namespace bROwSE {

class MyPainterWidget;

/** Visual representation of an address space.
 *
 *  An address space is a horizontal graphical bar that represents each address in a contiguous interval and assigns colors to
 *  the address. One or two mappings can be displayed at once stacked on on top of each other. A gutter area exists above and
 *  below that can be used to point to certain areas. */
class WAddressSpace: public Wt::WContainerWidget {
public:
    typedef Sawyer::Container::IntervalMap<AddressInterval, double> HeatMap;

    struct Properties {
        Wt::WPen pen;
        Wt::WPen highlightPen;
    };

private:
    MyPainterWidget *painterWidget_;
    int barWidth_;                                      // length of the bar graph
    int horizontalMargin_;                              // size of horizontal margins
    int topMargin_;                                     // size of top margin in pixels
    int topGutterHeight_;                               // size of the top gutter (pointer area)
    int barHeight_;                                     // total height of the bar graph inc. all bars
    int bottomGutterHeight_;                            // size of the bottom gutter (pointer area)
    int bottomMargin_;                                  // size of bottom margin in pixels
    int topGutterArrowWidth_;                           // size of base of arrow either side of point of arrow
    int bottomGutterArrowWidth_;                        // size of base of arrow either side of point of arrow
    AddressInterval displayedDomain_;                   // addresses displayed by the map
    Sawyer::Container::IndexedList<HeatMap> maps_;      // the maps to display
    Sawyer::Container::IndexedList<Rose::Color::Gradient> gradients_; // color gradient for each map
    Sawyer::Container::IndexedList<Properties> properties_;
    Sawyer::Container::IndexedList<AddressIntervalSet> highlights_; // areas to highlight
    Rose::Color::HSV borderColor_;                      // border color
    Rose::Color::HSV bgColor_;                          // background color
    HeatMap topGutterMap_;
    Rose::Color::Gradient topGutterGradient_;
    HeatMap bottomGutterMap_;
    Rose::Color::Gradient bottomGutterGradient_;
    Wt::Signal<rose_addr_t, Wt::WMouseEvent> topGutterClicked_;
    Wt::Signal<rose_addr_t, Wt::WMouseEvent> bottomGutterClicked_;
    Wt::Signal<rose_addr_t, size_t, Wt::WMouseEvent> barGraphClicked_;

public:
    explicit WAddressSpace(Wt::WContainerWidget *parent=NULL)
        : Wt::WContainerWidget(parent), painterWidget_(NULL),
          barWidth_(980), horizontalMargin_(10),
          topMargin_(2), topGutterHeight_(8), barHeight_(30), bottomGutterHeight_(8), bottomMargin_(2),
          topGutterArrowWidth_(3), bottomGutterArrowWidth_(3),
          borderColor_(Rose::Color::HSV_BLACK), bgColor_(Rose::Color::HSV_BLACK) {
        init();
    }

    /** Initialize with segment and function information.
     *
     *  The top bar represents the memory segments and the second (bottom) bar represents all functions. */
    void insertSegmentsAndFunctions(const P2::Partitioner&);

    /** Property: bar graph size.
     *
     * @{ */
    std::pair<int, int> barGraphSize() const { return std::make_pair(barWidth_, barHeight_); }
    void barGraphSize(int width, int height) { barWidth_=std::max(1, width), barHeight_=std::max(1, height); }
    /** @} */

    /** Property: margins.
     *
     * @{ */
    int horizontalMargin() const { return horizontalMargin_; }
    void horizontalMargin(int w) { horizontalMargin_ = std::max(0, w); }
    int topMargin() const { return topMargin_; }
    void topMargin(int h) { topMargin_ = std::max(0, h); }
    int bottomMargin() const { return bottomMargin_; }
    void bottomMargin(int h) { bottomMargin_ = std::max(0, h); }
    /** @} */

    /** Property: gutter heights.
     *
     * @{ */
    int topGutterHeight() const { return topGutterHeight_; }
    void topGutterHeight(int h) { topGutterHeight_ = std::max(0, h); }
    int bottomGutterHeight() const { return bottomGutterHeight_; }
    void bottomGutterHeight(int h) { bottomGutterHeight_ = std::max(0, h); }
    /** @} */

    /** Property: gutter arrow width.
     *
     *  These measurements are the amount by which the base of a gutter arrow extends to the left and right of the point of the
     *  arrow.
     *
     * @{ */
    int topGutterArrowWidth() const { return topGutterArrowWidth_; }
    void topGutterArrowWidth(int w) { topGutterArrowWidth_ = std::max(0, w); }
    int bottomGutterArrowWidth() const { return bottomGutterArrowWidth_; }
    void bottomGutterArrowWidth(int w) { bottomGutterArrowWidth_ = std::max(0, w); }
    /** @} */

    /** Total width in pixels. */
    int totalWidth() const { return horizontalMargin_ + barWidth_ + horizontalMargin_; }

    /** Total height in pixels. */
    int totalHeight() const { return topMargin_ + topGutterHeight_ + barHeight_ + bottomGutterHeight_ + bottomMargin_; }

    /** Domain over which the address map is displayed.
     *
     *  If the domain is empty then it will span all addresses in the heat maps (i.e., it will be the hull of the unions of the
     *  individual hulls.)
     *
     * @{ */
    AddressInterval displayedDomain() const;
    void displayedDomain(const AddressInterval &domain) { displayedDomain_ = domain; }
    /** @} */

    /** Maximum domain.
     *
     *  This is the hull of the union of the individual hulls and might be larger that the displayed domain. */
    AddressInterval fullDomain() const;

    /** Property: border color.
     *
     *  The color of the box drawn around the widget.  The default is black.
     *
     * @{ */
    const Rose::Color::HSV& borderColor() const { return borderColor_; }
    void borderColor(const Rose::Color::HSV &c) { borderColor_ = c; }
    /** @} */

    /** Property: background color.
     *
     *  The color to fill the background of the map area. This color will show through for areas that are not mapped. The
     *  default is black.
     *
     * @{ */
    const Rose::Color::HSV &bgColor() const { return bgColor_; }
    void bgColor(const Rose::Color::HSV &c) { bgColor_ = c; }
    /** @} */

    /** Number of maps defined. */
    size_t nMaps() const { return maps_.size(); }

    /** Reference to specified map.
     *
     *  If the index is greater than the largest stored map index then new maps are created along with its gradient (assuming
     *  this object is not const, in which case an exception is thrown).
     *
     * @{ */
    const HeatMap& map(size_t idx) const { return maps_[idx]; }
    HeatMap& map(size_t idx);
    /** @} */

    /** Reference to specified highlight set.
     *
     *  If the index is greater than the largest stored index then new maps are created along with its gradient (assuming
     *  this object is not const, in which case an exception is thrown).
     *
     * @{ */
    const AddressIntervalSet& highlights(size_t idx) const { return highlights_[idx]; }
    AddressIntervalSet& highlights(size_t idx);
    /** @} */

    /** Reference to specified gradient.
     *
     *  Returns the gradient for the specified heat map.
     *
     * @{ */
    const Rose::Color::Gradient& gradient(size_t idx) const { return gradients_[idx]; }
    Rose::Color::Gradient& gradient(size_t idx);
    /** @} */

    /** Reference to properties for specified map.
     *
     * @{ */
    const Properties& properties(size_t idx) const { return properties_[idx]; }
    Properties& properties(size_t idx);
    /** @} */

    /** Gutter maps.
     *
     * @{ */
    const HeatMap& topGutterMap() const { return topGutterMap_; }
    HeatMap& topGutterMap() { return topGutterMap_; }
    const HeatMap& bottomGutterMap() const { return bottomGutterMap_; }
    HeatMap& bottomGutterMap() { return bottomGutterMap_; }
    /** @} */

    /** Gutter gradients.
     *
     * @{ */
    const Rose::Color::Gradient& topGutterGradient() const { return topGutterGradient_; }
    Rose::Color::Gradient& topGutterGradient() { return topGutterGradient_; }
    const Rose::Color::Gradient& bottomGutterGradient() const { return bottomGutterGradient_; }
    Rose::Color::Gradient& bottomGutterGradient() { return bottomGutterGradient_; }
    /** @} */

    /** Clear all maps and gradients.
     *
     *  Removes all maps and gradients, but leaves other properties alone. */
    void clear();

    /** Cause to be redrawn. */
    void redraw(Wt::WPaintDevice*);
    void redraw();

    /** Initialize with memory map information.
     *
     *  Initializes the specified map with information about address intervals.  If an @p idx is specified (not -1) then that
     *  layer is used in the address map, otherwise a new layer is created. In any case, the layer used is returned.  If @p
     *  showStarts is true then the starting address of each function will be added to the gutter (top gutter for layer zero,
     *  bottom gutter for other layers).  The gutter is cleared first. */
    size_t insert(const Rose::BinaryAnalysis::MemoryMap::Ptr&, size_t idx=0, bool showStarts=true);

    /** Initialize with function addresses.
     *
     *  Initilializes the map with addresses for the specified functions.  The @p partitioner is used to determine the function
     *  extents.  If the function has an ATTR_Heat attribute (which must be of type @c double and should be between zero and
     *  one) then that is used as the lookup in the corresponding color gradient.  The @p idx determines which layer of the bar
     *  graph is used for these functions and is also the return value.  If the @p idx is -1 then a new layer is allocated. If
     *  @p showStarts is true then the starting address of each function will be added to the gutter (top gutter for layer
     *  zero, bottom gutter otherwise). The gutter is cleared first. */
    size_t insert(const P2::Partitioner &partitioner, const std::vector<P2::Function::Ptr> &functions,
                  size_t idx=1, bool showStarts=true);

    /** Zoom in or out based on mouse click. */
    void zoomInOut(const Wt::WMouseEvent&);

    /** Track mouse motion in the bar. */
    void trackMotion(const Wt::WMouseEvent&);

    /** Signal emitted when the top gutter area is clicked.
     *
     *  The signal arguments are the virtual address and the mouse event. */
    Wt::Signal<rose_addr_t, Wt::WMouseEvent>& topGutterClicked() { return topGutterClicked_; }

    /** Signal emitted when the bottom gutter area is clicked.
     *
     *  The signal arguments are the virtual address and the mouse event. */
    Wt::Signal<rose_addr_t, Wt::WMouseEvent>& bottomGutterClicked() { return bottomGutterClicked_; }

    /** Signal emitted when the bar graph is clicked.
     *
     *  The signal arguments are the virtual address, the bar graph index, and the mouse event. */
    Wt::Signal<rose_addr_t, size_t, Wt::WMouseEvent>& barGraphClicked() { return barGraphClicked_; }

private:
    void init();

    /** Computes zoomed interval.  Computes a new interval that does not extend beyond @p limit and which includes @p center
     *  (or closest possible given @p limit).  The new interval will contain @p size addresses (or closest possible given @p
     *  limit). */
    static AddressInterval centerInterval(rose_addr_t center, rose_addr_t size, const AddressInterval &limits);

    /** Bar positions in widget space.
     *
     *  The bars may extend to the left and/or right edges of the widget without regard for margins if we're displaying a
     *  subset of the whole space.  This is different than barGraphSize, which returns the area between the margins and which
     *  is a property that can be set.
     *
     * @{ */
    int barGraphLeft() const;
    int barGraphRight() const;
    /** @} */

    /** Convert widget coordinates to virtual address.
     *
     *  Returns an address or nothing. */
    Sawyer::Optional<rose_addr_t> widgetToVa(int x, int y) const;

    /** Performs some action in response to a mouse click. */
    void performClickAction(const Wt::WMouseEvent&);

    /** Instantiate subbars.
     *
     *  Instantiates all sub-bargraphs up to @p idx and initializes them to default values. */
    void instantiateSubBar(size_t idx);
};

} // namespace
#endif
