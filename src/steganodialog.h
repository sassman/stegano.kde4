#ifndef STEGANODIALOG_H
#define STEGANODIALOG_H
 
#include <KDialog>
 
// include the automatically generated header file for the ui-file
#include "stegano.h"
#include "../build/src/ui_stegano.h"

class SteganoDialog : public KDialog
{
    Q_OBJECT
    public:
        SteganoDialog( QWidget *parent=0 );
        virtual ~SteganoDialog() {}
	
public slots:
		void slideDown(QWidget* );
  
	
    private slots:
		void hideData();
		void testingAnimations();
    private:
        // accessor to the ui. we can access all gui elements
        // specified in Designer. If mydialog.ui contains a
        // button "myButton", we will be able to access it
        // with ui.myButton in the cpp file.
        Ui::Stegano ui;
	Stegano stegano;
};
 
#endif
