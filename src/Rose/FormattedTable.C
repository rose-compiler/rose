#include <Rose/FormattedTable.h>

#include <Rose/CommandLine.h>
#include <Rose/StringUtility/Convert.h>
#include <Rose/StringUtility/Escape.h>
#include <Rose/StringUtility/SplitJoin.h>

namespace Rose {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CellProperties
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FormattedTable::CellProperties::CellProperties() {}

FormattedTable::CellProperties::CellProperties(const Sawyer::Optional<Color::HSV> &foreground,
                                               const Sawyer::Optional<Color::HSV> &background,
                                               const Sawyer::Optional<Alignment> &alignment)
    : foreground_(foreground), background_(background), alignment_(alignment) {}

const Sawyer::Optional<Color::HSV>&
FormattedTable::CellProperties::foreground() const {
    return foreground_;
}

FormattedTable::CellProperties&
FormattedTable::CellProperties::foreground(const Sawyer::Optional<Color::HSV> &color) {
    foreground_ = color;
    return *this;
}

const Sawyer::Optional<Color::HSV>&
FormattedTable::CellProperties::background() const {
    return background_;
}

FormattedTable::CellProperties&
FormattedTable::CellProperties::background(const Sawyer::Optional<Color::HSV> &color) {
    background_ = color;
    return *this;
}

const Sawyer::Optional<FormattedTable::Alignment>&
FormattedTable::CellProperties::alignment() const {
    return alignment_;
}

FormattedTable::CellProperties&
FormattedTable::CellProperties::alignment(const Sawyer::Optional<Alignment> &a) {
    alignment_ = a;
    return *this;
}

FormattedTable::CellProperties
FormattedTable::CellProperties::merge(const CellProperties &a, const CellProperties &b) {
    CellProperties retval;
    retval.foreground_ = a.foreground_.orElse(b.foreground_);
    retval.background_ = a.background_.orElse(b.background_);
    retval.alignment_ = a.alignment_.orElse(b.alignment_);
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

FormattedTable::Format
FormattedTable::format() const {
    return format_;
}

void
FormattedTable::format(Format fmt) {
    format_ = fmt;
}

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

const std::string&
FormattedTable::get(size_t rowIdx, size_t columnIdx) const {
    if (rowIdx < cells_.size() && columnIdx < cells_[rowIdx].size()) {
        return cells_[rowIdx][columnIdx];
    } else {
        static const std::string empty;
        return empty;
    }
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

const FormattedTable::CellProperties&
FormattedTable::columnHeaderProperties(size_t rowIdx, size_t columnIdx) const {
    if (rowIdx < columnHeaderProps_.size() && columnIdx < columnHeaderProps_[rowIdx].size()) {
        return columnHeaderProps_[rowIdx][columnIdx];
    } else {
        static const CellProperties empty(Sawyer::Nothing(), Sawyer::Nothing(), Alignment::CENTER);
        return empty;
    }
}

void
FormattedTable::columnHeaderProperties(size_t rowIdx, size_t columnIdx, const CellProperties &props) {
    if (rowIdx >= columnHeaderProps_.size())
        columnHeaderProps_.resize(rowIdx + 1);
    if (columnIdx >= columnHeaderProps_[rowIdx].size())
        columnHeaderProps_[rowIdx].resize(columnIdx + 1);
    columnHeaderProps_[rowIdx][columnIdx] = props;
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
    if (Format::PLAIN == format_ && !widths.empty()) {
        out <<indentation_ <<"+";
        for (size_t j = 0; j < widths.size(); ++j)
            out <<"-" <<std::string(widths[j], '-') <<"-+";
        out <<"\n";
    }
}

std::string
FormattedTable::cellPropertiesBegin(const CellProperties &props) const {
    std::string retval;
    switch (format_) {
        case Format::PLAIN:
            if (Rose::CommandLine::genericSwitchArgs.colorization.isEnabled()) {
                if (auto hsv = props.foreground())
                    retval += hsv->toAnsi(Color::Layer::FOREGROUND);
                if (auto hsv = props.background())
                    retval += hsv->toAnsi(Color::Layer::BACKGROUND);
            }
            break;

        case Format::HTML:
            if (auto hsv = props.foreground())
                retval += "color: " + hsv->toHtml() + ";";
            if (auto hsv = props.background())
                retval += "background-color: " + hsv->toHtml() + ";";
            break;

        case Format::CSV:
        case Format::SHELL:
            break;
    }
    return retval;
}

std::string
FormattedTable::cellPropertiesEnd(const CellProperties&) const {
    std::string retval;
    switch (format_) {
        case Format::PLAIN:
            if (Rose::CommandLine::genericSwitchArgs.colorization.isEnabled())
                retval = "\033[0m";
            break;

        case Format::HTML:
            break;

        case Format::CSV:
        case Format::SHELL:
            break;
    }
    return retval;
}

void
FormattedTable::printRow(std::ostream &out, const std::vector<size_t> &widths, const std::vector<CellProperties> &props,
                         const std::vector<std::string> &row) const {
    switch (format_) {
        case Format::PLAIN: {
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
                    const std::string s = i < columnLines[j].size() ? columnLines[j][i] : "";
                    out <<" " <<cellPropertiesBegin(props[j]);
                    switch (props[j].alignment().orElse(Alignment::LEFT)) {
                        case Alignment::LEFT:
                            out <<StringUtility::leftJustify(s, widths[j]);
                            break;
                        case Alignment::RIGHT:
                            out <<StringUtility::rightJustify(s, widths[j]);
                            break;
                        case Alignment::CENTER:
                            out <<StringUtility::centerJustify(s, widths[j]);
                            break;
                    }
                    out <<cellPropertiesEnd(props[j]) <<" |";
                }
                out <<"\n";
            }
            break;
        }

        case Format::HTML: {
            out <<"<tr>\n";
            for (size_t j = 0; j < widths.size(); ++j) {
                out <<"<td style=\"";
                switch (props[j].alignment().orElse(Alignment::LEFT)) {
                    case Alignment::LEFT:
                        out <<"text-align: left;";
                        break;
                    case Alignment::RIGHT:
                        out <<"text-align: right;";
                        break;
                    case Alignment::CENTER:
                        out <<"text-align: center;";
                        break;
                }
                out <<cellPropertiesBegin(props[j]);
                out <<"\">";
                const std::string s = row[j].empty() ? "&nbsp;" : row[j];
                out <<s <<"</td>\n";
            }
            out <<"</tr>\n";
            break;
        }

        case Format::CSV: {
            // See RFC 4180 [https://datatracker.ietf.org/doc/html/rfc4180]
            for (size_t j = 0; j < widths.size(); ++j)
                out <<(j?",":"") <<StringUtility::csvEscape(row[j]);
            out <<"\n";
            break;
        }

        case Format::SHELL: {
            // See enum for documentation
            for (size_t j = 0; j < widths.size(); ++j) {
                bool needQuotes = false;
                for (char ch: row[j]) {
                    if (!::isalnum(ch) && !strchr("_-+./", ch)) {
                        needQuotes = true;
                        break;
                    }
                }
                out <<(j ? "\t" : "") <<(needQuotes ? "$'" + StringUtility::cEscape(row[j],'\'') + "'" : row[j]);
            }
            out <<"\n";
            break;
        }
    }
}

void
FormattedTable::print(std::ostream &out) const {
    std::vector<size_t> widths = computeColumnWidths();
    if (widths.empty())
        return;

    if (Format::HTML == format_)
        out <<"<table border=\"1\">\n";

    // Headers
    if (!columnHeaders_.empty() && format_ != Format::CSV && format_ != Format::SHELL) {
        printHorizontalRule(out, widths);
        for (size_t i = 0; i < columnHeaders_.size(); ++i) {
            std::vector<CellProperties> props;
            props.reserve(widths.size());
            for (size_t j = 0; j < widths.size(); ++j)
                props.push_back(columnHeaderProperties(i, j));
            printRow(out, widths, props, columnHeaders_[i]);
        }
    }

    // Body
    printHorizontalRule(out, widths);
    for (size_t i = 0; i < cells_.size(); ++i)
        printRow(out, widths, props_[i], cells_[i]);
    printHorizontalRule(out, widths);

    if (Format::HTML == format_)
        out <<"<table>\n";
}

} // namespace

std::ostream& operator<<(std::ostream &out, const Rose::FormattedTable &table) {
    table.print(out);
    return out;
}
