#ifndef STEGANODIALOG_H
#define STEGANODIALOG_H

#include <KXmlGuiWindow>
#include <KAction>

// include the automatically generated header file for the ui-file
#include "steganocore.h"
#include "ui_stegano.h"
//#include "ui_stegano_clean.h"

class QFileInfo;
namespace Stegano {

class SteganoDialog : public KXmlGuiWindow { 
    Q_OBJECT

public:
    SteganoDialog( QWidget *parent=0 );
    virtual ~SteganoDialog() {}
    
signals:
    void modificationChanged(bool);

public slots:
    void slideDown(QWidget* );

private slots:
    bool hideData();
    bool unhideData();
    void testingAnimations();
    void saveMedia();
    void setToHideFlag();
    void sourceMediaChange();
    void sourceMediaChange(QString info);
    void noValidSourceMedia();
    void noValidTargetMedia();
    void noValidPermissionsOnMedia();
    void noValidMessage();
    void onChanged();
    void toggleEncryption(bool enabled);

private:
    Ui::Stegano* steganoUI;
    SteganoCore stegano;
    bool        isHidden;
    QString     fileFilterSource;
    QString     fileFilterTarget;
    
    KAction*    actionOpenMedia;
    KAction*    actionProtect;
    KAction*    actionUnhide;
    KAction*    actionSaveAs;
    
    void        setupActions();
    virtual void dropEvent ( QDropEvent* );
    virtual void dragEnterEvent ( QDragEnterEvent* );
    virtual void dragMoveEvent ( QDragMoveEvent* );
    virtual void dragLeaveEvent ( QDragLeaveEvent* );
    bool        isValidMimeData( const QMimeData* mime );
};

}

#endif
