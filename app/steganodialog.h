#ifndef STEGANODIALOG_H
#define STEGANODIALOG_H

#include <KXmlGuiWindow>

// include the automatically generated header file for the ui-file
#include "steganocore.h"
#include "ui_stegano.h"

class SteganoDialog : public KXmlGuiWindow { Q_OBJECT

public:
    SteganoDialog( QWidget *parent=0 );
    virtual ~SteganoDialog() {}

public slots:
    void slideDown(QWidget* );

private slots:
    bool hideData();
    bool unhideData();
    void testingAnimations();
    void saveMedia();
    void setToHideFlag();
    void sourceMediaChange();
    void noValidSourceMedia();
    void noValidTargetMedia();
    void noValidMessage();
    void showSize();
    void showCharacters();

private:
    Ui::Stegano* steganoUI;
    SteganoCore stegano;
    bool        isHidden;
    QString     fileFilterSource;
    QString     fileFilterTarget;
    
    void setupActions();
};

#endif
