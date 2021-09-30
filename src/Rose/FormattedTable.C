#include <Rose/FormattedTable.h>

#include <Rose/CommandLine.h>
#include <Rose/StringUtility/Convert.h>
#include <Rose/StringUtility/SplitJoin.h>

namespace Rose {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CellProperties
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const Sawyer::Optional<Color::HSV>&
FormattedTable::CellProperties::foreground() const {
    return foreground_;
}

void
FormattedTable::CellProperties::foreground(const Sawyer::Optional<Color::HSV> &color) {
    foreground_ = color;
}

const Sawyer::Optional<Color::HSV>&
FormattedTable::CellProperties::background() const {
    return background_;
}

void
FormattedTable::CellProperties::background(const Sawyer::Optional<Color::HSV> &color) {
    background_ = color;
}

FormattedTable::CellProperties
FormattedTable::CellProperties::merge(const CellProperties &a, const CellProperties &b) {
    CellProperties retval;
    retval.foreground_ = a.foreground_.orElse(b.foreground_);
    retval.background_ = a.background_.orElse(b.background_);
    return retval;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FormattedTable constructors
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FormattedTable::FormattedTable() {}

FormattedTable::FormattedTable(size_t nRows, size_t nColumns) {
    resize(nRows, nColumns);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FormattedTable table-wide properties
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const std::string&
FormattedTable::indentation() const {
    return indentation_;
}

void
FormattedTable::indentation(const std::string &s) {
    indentation_ = s;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FormattedTable data
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
FormattedTable::resize(size_t nr, size_t nc) {
    if (nr != nRows() || nc != nColumns()) {
        cells_.resize(nr);
        props_.resize(nr);
        for (size_t i = 0; i < nr; ++i) {
            cells_[i].resize(nc);
            props_[i].resize(nc);
        }
    }
}

void
FormattedTable::resize(size_t nRows) {
    resize(nRows, nColumns());
}

size_t
FormattedTable::nRows() const {
    return cells_.size();
}

size_t
FormattedTable::nColumns() const {
    return cells_.empty() ? 0 : cells_[0].size();
}

void
FormattedTable::insert(size_t rowIdx, size_t columnIdx, const std::string &repr) {
    if (rowIdx >= nRows() || columnIdx >= nColumns())
        resize(std::max(rowIdx+1, nRows()), std::max(columnIdx+1, nColumns()));
    cells_[rowIdx][columnIdx] = repr;
}

void
FormattedTable::insert(size_t rowIdx, size_t columnIdx, const std::string &repr, const CellProperties &props) {
    insert(rowIdx, columnIdx, repr);
    cellProperties(rowIdx, columnIdx, props);
}

const FormattedTable::CellProperties&
FormattedTable::cellProperties(size_t rowIdx, size_t columnIdx) const {
    if (rowIdx < nRows() && columnIdx < nColumns()) {
        return props_[rowIdx][columnIdx];
    } else {
        static const CellProperties empty;
        return empty;
    }
}

void
FormattedTable::cellProperties(size_t rowIdx, size_t columnIdx, const CellProperties &props) {
    if (rowIdx >= nRows() || columnIdx >= nColumns())
        resize(std::max(rowIdx+1, nRows()), std::max(columnIdx+1, nColumns()));
    props_[rowIdx][columnIdx] = props;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

size_t
FormattedTable::nColumnHeaderRows() const {
    return columnHeaders_.size();
}

void
FormattedTable::nColumnHeaderRows(size_t n) {
    columnHeaders_.resize(n);
}

void
FormattedTable::columnHeader(size_t rowIdx, size_t columnIdx, const std::string &title) {
    if (rowIdx >= nColumnHeaderRows())
        nColumnHeaderRows(rowIdx + 1);
    if (columnIdx >= columnHeaders_[rowIdx].size())
        columnHeaders_[rowIdx].resize(columnIdx + 1);
    columnHeaders_[rowIdx][columnIdx] = title;
}

const std::string&
FormattedTable::columnHeader(size_t rowIdx, size_t columnIdx) const {
    if (rowIdx >= columnHeaders_.size() || columnIdx >= columnHeaders_[rowIdx].size()) {
        static const std::string empty;
        return empty;
    } else {
        return columnHeaders_[rowIdx][columnIdx];
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FormattedTable printing
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<size_t>
FormattedTable::computeColumnWidths() const {
    std::vector<size_t> widths(nColumns(), 0);

    // Widths of headers
    for (auto row: columnHeaders_) {
        for (size_t j = 0; j < row.size() && j < nColumns(); ++j) {
            for (const std::string &line: StringUtility::split("\n", row[j]))
                widths[j] = std::max(widths[j], line.size());
        }
    }

    // Widths of data
    for (auto row: cells_) {
        for (size_t j = 0; j < row.size(); ++j) {
            for (const std::string &line: StringUtility::split("\n", row[j]))
                widths[j] = std::max(widths[j], line.size());
        }
    }
    return widths;
}

void
FormattedTable::printHorizontalRule(std::ostream &out, const std::vector<size_t> &widths) const {
    if (!widths.empty()) {
        out <<indentation_ <<"+";
        for (size_t j = 0; j < widths.size(); ++j)
            out <<"-" <<std::string(widths[j], '-') <<"-+";
        out <<"\n";
    }
}

std::string
FormattedTable::ansiPropertiesBegin(const CellProperties &props) const {
    std::string retval;
    if (Rose::CommandLine::genericSwitchArgs.colorization.isEnabled()) {
        if (auto hsv = props.foreground())
            retval += hsv->toAnsi(Color::Layer::FOREGROUND);
        if (auto hsv = props.background())
            retval += hsv->toAnsi(Color::Layer::BACKGROUND);
    }
    return retval;
}

std::string
FormattedTable::ansiPropertiesEnd(const CellProperties &props) const {
    if (Rose::CommandLine::genericSwitchArgs.colorization.isEnabled()) {
        return "\033[0m";
    } else {
        return "";
    }
}

void
FormattedTable::printRow(std::ostream &out, const std::vector<size_t> &widths, const std::vector<CellProperties> &props,
                         const std::vector<std::string> &row, std::string(*justify)(const std::string&, size_t, char)) const {
    std::vector<std::vector<std::string>> columnLines;
    size_t maxCellHeight = 0;
    for (const std::string &cell: row) {
        columnLines.push_back(StringUtility::split("\n", cell));
        maxCellHeight = std::max(maxCellHeight, columnLines.back().size());
    }

    // i == 0 means we print a line even if all columns of the row are empty
    for (size_t i = 0; i == 0 || i < maxCellHeight; ++i) {
        out <<indentation_ <<"|";
        for (size_t j = 0; j < widths.size(); ++j) {
            out <<" "
                <<ansiPropertiesBegin(props[j])
                <<justify(i < columnLines[j].size() ? columnLines[j][i] : "", widths[j], ' ')
                <<ansiPropertiesEnd(props[j])
                <<" |";
        }
        out <<"\n";
    }
}

void
FormattedTable::print(std::ostream &out) const {
    std::vector<size_t> widths = computeColumnWidths();
    if (widths.empty())
        return;

    if (!columnHeaders_.empty()) {
        printHorizontalRule(out, widths);
        std::vector<CellProperties> props(widths.size());
        for (size_t i = 0; i < columnHeaders_.size(); ++i)
            printRow(out, widths, props, columnHeaders_[i], StringUtility::centerJustify);
    }

    printHorizontalRule(out, widths);
    for (size_t i = 0; i < cells_.size(); ++i)
        printRow(out, widths, props_[i], cells_[i], StringUtility::leftJustify);
    printHorizontalRule(out, widths);
}

} // namespace

std::ostream& operator<<(std::ostream &out, const Rose::FormattedTable &table) {
    table.print(out);
    return out;
}
