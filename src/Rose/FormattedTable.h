#ifndef ROSE_StringUtility_FormattedTable_H
#define ROSE_StringUtility_FormattedTable_H

#include <Rose/Color.h>
#include <string>
#include <vector>

namespace Rose {

/** Class for printing nicely formattated tables.
 *
 *  A table is a two-dimensional grid containing cells. A @ref FormattedTable object holds information about how to
 *  format cells, functions to populate the cells, and functions to print the table. */
class FormattedTable {
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Public types
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** Properties for a particular cell. */
    class CellProperties {
        Sawyer::Optional<Color::HSV> foreground_;
        Sawyer::Optional<Color::HSV> background_;

    public:
        /** Property: Foreground color.
         *
         * @{ */
        const Sawyer::Optional<Color::HSV>& foreground() const;
        void foreground(const Sawyer::Optional<Color::HSV>&);
        /** @} */

        /** Property: Background color.
         *
         * @{ */
        const Sawyer::Optional<Color::HSV>& background() const;
        void background(const Sawyer::Optional<Color::HSV>&);
        /** @} */

        /** Create new properties by merging two properties.
         *
         *  The return value has values from object @p a or object @p b, whichever is set, in that order. */
        static CellProperties merge(const CellProperties &a, const CellProperties &b);
    };

private:
    std::vector<std::vector<std::string>> cells_;         // data
    std::vector<std::vector<CellProperties>> props_;      // data properties
    std::vector<std::vector<std::string>> columnHeaders_; // titles for columns
    std::string indentation_;                             // to be printed before each line of output

public:
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors, etc.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** Construct an empty table. */
    FormattedTable();

    /** Construct a table with a certain number of cells. */
    FormattedTable(size_t nRows, size_t nColumns);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Table-wide properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** Property: Indentation.
     *
     *  This property holds a string that is printed at the beginning of every line of output.
     *
     * @{ */
    const std::string& indentation() const;
    void indentation(const std::string&);
    /** @} */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // FormattedTable data
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** Resize the table data.
     *
     *  Making a table smaller will remove data cells but not row and column properties. Making a table
     *  larger will insert cells with empty data, and if necessary create default row and column properties.
     *
     * @{ */
    void resize(size_t nRows, size_t nColumns);
    void resize(size_t nRows);
    /** @} */

    /** Number of rows or columns of data.
     *
     *  See also, @ref resize.
     *
     * @{ */
    size_t nRows() const;
    size_t nColumns() const;
    /** @} */

    /** Insert data into a table.
     *
     *  If the row and/or column number is outside the current size of the table, then the table is automatically resized, but
     *  it's more efficient to resize the table before adding data.
     *
     * @{ */
    void insert(size_t rowIdx, size_t columnIdx, const std::string &repr);
    void insert(size_t rowIdx, size_t columnIdx, const std::string &repr, const CellProperties&);
    /** @} */

    /** Properties for a data cell.
     *
     *  When inserting properties, the table is automatically extended.
     *
     * @{ */
    const CellProperties& cellProperties(size_t rowIdx, size_t columnIdx) const;
    void cellProperties(size_t rowIdx, size_t columnIdx, const CellProperties&);
    /** @} */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // FormattedTable columns
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** Property: Number of column header rows.
     *
     *  Column headers appear above the data and the number of column header rows is independent of the number of data rows
     *  that's returned by @ref nRows.  When the number of column header rows is decreased, the affected column header names
     *  are deleted; when the number of rows is increased, the new titles are set to empty strings.
     *
     * @{ */
    size_t nColumnHeaderRows() const;
    void nColumnHeaderRows(size_t);
    /** @} */

    /** Property: Name for column headers.
     *
     *  Column headers occupy one or more rows, but the rows do not count as data. Column information is extended as necessary
     *  when new column headers are added. When querying a column header that doesn't exist, an empty string is returned.
     *
     * @{ */
    const std::string& columnHeader(size_t rowIdx, size_t columnIdx) const;
    void columnHeader(size_t rowIdx, size_t columnIdx, const std::string &title);
    /** @} */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Printing
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** Print a table. */
    void print(std::ostream&) const;

private:
    std::string ansiPropertiesBegin(const CellProperties&) const;
    std::string ansiPropertiesEnd(const CellProperties&) const;
    void printHorizontalRule(std::ostream&, const std::vector<size_t> &widths) const;
    void printRow(std::ostream&, const std::vector<size_t> &widths, const std::vector<CellProperties> &props,
                  const std::vector<std::string> &row, std::string(*justify)(const std::string&, size_t, char)) const;
    std::vector<size_t> computeColumnWidths() const;

};

} // namespace

std::ostream& operator<<(std::ostream &out, const Rose::FormattedTable &table);

#endif
