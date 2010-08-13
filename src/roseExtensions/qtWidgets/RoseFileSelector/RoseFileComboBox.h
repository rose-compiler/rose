#ifndef ROSEFILECOMBOBOX_H
#define ROSEFILECOMBOBOX_H


#include <QComboBox>

class SgProject;
class AstFilterInterface;

/**
 *      \brief ComboBox which displays all files associated with an SgProject
 *  User can select a file, and according signals are emitted
 */
class RoseFileComboBox : public QComboBox
{
        Q_OBJECT

        Q_PROPERTY(bool DispOnlyFilename READ displayOnlyFilename WRITE setDisplayOnlyFilename)

        public:
                RoseFileComboBox(QWidget * parent=NULL);
                virtual ~RoseFileComboBox();

                void setProject(SgProject * proj);

                bool displayOnlyFilename()      const    { return dispOnlyFilename; }
                void setDisplayOnlyFilename(bool on);

        signals:
                /// Emitted when user selects a file
                void selectedFileChanged( int fileId );
                void selectedFileChanged( const QString & filename );
        void selectedFileChanged( AstFilterInterface* filter );

        protected slots:
                void curIndexChanged (int index);

        protected:
                SgProject * project;

                /// If false the whole path is displayed, otherwise filename only
                bool dispOnlyFilename;
};

#endif
