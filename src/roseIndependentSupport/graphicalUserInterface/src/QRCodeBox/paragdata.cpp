#include "paragdata.h"

ParagData::ParagData()
{
    lastLengthForCompletion = 0;
    m_level = 0;
    m_mark = 0;
    m_open = true;
    m_blockStart = false;
    m_parsed = false;
}

ParagData::~ParagData()
{

}

void ParagData::join( QTextParagraphData* )
{
    // not implemented yet!!
}

void ParagData::clear()
{
    m_symbolList.clear();
}

void ParagData::add( int tp, const QChar& ch, int pos )
{
    m_symbolList << Symbol( tp, ch, pos );
}

int ParagData::level() const
{
    return m_level;
}

void ParagData::setLevel( int level )
{
    m_level = level;
}

uint ParagData::mark() const
{
    return m_mark;
}

void ParagData::setMark( uint mark )
{
    m_mark = mark;
}

bool ParagData::isOpen() const
{
    return m_open;
}

void ParagData::setOpen( bool open )
{
    m_open = open;
}

bool ParagData::isBlockStart() const
{
    return m_blockStart;
}

void ParagData::setBlockStart( bool blockStart )
{
    m_blockStart = blockStart;
}

bool ParagData::isParsed() const
{
    return m_parsed;
}

void ParagData::setParsed( bool parsed )
{
    m_parsed = parsed;
}
