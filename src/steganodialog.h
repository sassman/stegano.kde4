#ifndef STEGANODIALOG_H
#define STEGANODIALOG_H
 
#include <KDialog>
 
// include the automatically generated header file for the ui-file
#include "steganocore.h"
#include "../build/src/ui_stegano.h"

class SteganoDialog : public KDialog, Ui::Stegano {

	Q_OBJECT

	public:
		SteganoDialog( QWidget *parent=0 );
		virtual ~SteganoDialog() {}
	
	public slots:
		void slideDown(QWidget* );

	private slots:
		void hideData();
		void testingAnimations();
		void saveMedia();

	private:
		SteganoCore stegano;
};
 
#endif
