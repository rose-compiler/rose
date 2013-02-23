#ifndef __x10_source_code_position_h__
#define __x10_source_code_position_h__

class X10SourceCodePosition {
    /* The minimum four fields.  */
    private:
        // std::string text;
        int line_number_start;
        int line_number_end;
        int column_number_start;
        int column_number_end;

    // I don't think I need the token type.
    // int type;
    public:
        X10SourceCodePosition (int line_start, int line_end );
        X10SourceCodePosition (int line_start, int line_end, int col_start, int col_end );

        // std::string getText();
        int getLineStart();
        int getLineEnd();
        int getColStart();
        int getColEnd();
};

#endif
