#ifndef STEGANODIALOG_H
#define STEGANODIALOG_H

#include <KXmlGuiWindow>

// include the automatically generated header file for the ui-file
#include "steganocore.h"
#include "../build/src/ui_stegano.h"

class SteganoDialog : public KXmlGuiWindow {

    Q_OBJECT

public:
    SteganoDialog( QWidget *parent=0 );
    virtual ~SteganoDialog() {}

public slots:
    void slideDown(QWidget* );

private slots:
    void hideData();
    void unhideData();
    void testingAnimations();
    void saveMedia();

private:
    Ui::Stegano* steganoUI;
    SteganoCore stegano;
    
    void setupActions();
};

#endif
