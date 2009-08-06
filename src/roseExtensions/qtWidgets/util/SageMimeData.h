
#ifndef SAGE_MIME_H
#define SAGE_MIME_H

#include <QString>

const QString SG_NODE_MIMETYPE        = "application/SgNode";
const QString SG_NODE_BINARY_MIMETYPE = "application/SgNode-binary";
const QString SG_NODE_SOURCE_MIMETYPE = "application/SgNode-source";

class SgNode;

class QMimeData;

typedef std::vector<SgNode *> SgNodeVector;

/// creates MIME Data out of a SgNode
/// depending on the node, the base type (source or binary) gets set by finding
/// the appropriate SgFile
/// the other file gets set if the appropriate Link Attribute is set
QMimeData *createSageMimeData( SgNode *node );

/// extract the MIME data, returns NULL if not set
SgNode *getGeneralNode( const QMimeData *data );
SgNodeVector getSourceNodes( const QMimeData *data );
SgNodeVector getBinaryNodes( const QMimeData *data );

SgNodeVector getNodes( const QMimeData *data, const QString& type );

#endif
