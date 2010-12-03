#include <KLocale>
#include <KMessageBox>
 
#include <QPropertyAnimation> 
#include <KDE/Plasma/WindowEffects>
#include <KWindowSystem>

#include <QPushButton>
#include <QProgressDialog>
#include <QPropertyAnimation>

// include the header file of the dialog
#include "steganodialog.h"
 
SteganoDialog::SteganoDialog( QWidget *parent ) : KDialog( parent ){
   QWidget *widget = new QWidget( this );
 
    // create the user interface, the parent widget is "widget"
    ui.setupUi(widget); // this is the important part
 
    // set the widget with all its gui elements as the dialog's
    // main widget
    setMainWidget( widget );
 
    // other KDialog options
    setCaption( i18n("Stegano") );
    setButtons( NULL );
 
    // Example Signal/Slot connection using widgets in your UI.
    // Note that you have to prepend "ui." when referring
    // to your UI elements.
//    connect( ui.bHide, SIGNAL( clicked() ),
//             this, SLOT( slotButtonClicked() ) );
    connect( &stegano, SIGNAL( keyChanged(QString) ),
             ui.tHash, SLOT( setText(QString) ) );
    connect( &stegano, SIGNAL( useCryptChanged(bool) ),
             ui.cUseEncryption, SLOT( setChecked(bool) ) );
    connect( &stegano, SIGNAL( sourceMediaChanged(KUrl) ),
             ui.preview, SLOT( showPreview(KUrl) ) );

    connect( ui.tPassword, SIGNAL( userTextChanged(const QString&) ),
             &stegano, SLOT( setPassword(const QString&) ) );
    connect( ui.cUseEncryption, SIGNAL( toggled(bool) ),
             &stegano, SLOT( setUseCrypt(bool) ) );
    connect( ui.cFile, SIGNAL( textChanged(QString) ),
             &stegano, SLOT( setSourceMedia(QString) ) );
    connect( ui.bHide, SIGNAL( clicked() ),
             this, SLOT( hideData() ) );
    
    connect( ui.bUnhide, SIGNAL( clicked() ),
	     this, SLOT( testingAnimations() ) );
}

void SteganoDialog::hideData(){
	QProgressDialog progress("Hide Data...", "Abort", 0, 100);
	progress.setWindowModality(Qt::WindowModal);
	progress.show();

	QPropertyAnimation *animation = new QPropertyAnimation(&progress, "geometry");
	animation->setDuration(10000);
	animation->setStartValue(progress.geometry());
	QRect dest = progress.geometry();
	dest.setLeft(dest.left() + 100);
	animation->setEndValue(dest);
	animation->start();	
  
	stegano.setSourceMedia(ui.cFile->text());
	stegano.hideData(ui.tMessageText->toPlainText().toUtf8());
}

void SteganoDialog::testingAnimations(){

  /**
  KWindowSystem::setOnAllDesktops(winId(), true);
  Plasma::WindowEffects::slideWindow(this, Plasma::TopEdge);
  KWindowSystem::forceActiveWindow(winId());
  */
  
  QFrame *widget = new QFrame(this);
  widget->setGeometry(0, 0, 200, 200);
//  widget->adjustSize();
//  widget->setFrameStyle(QFrame::Panel | QFrame::Raised);
//  widget->setLineWidth(2);
  widget->raise();
  widget->show();

  this->slideDown(widget);   
  
}

void SteganoDialog::slideDown(QWidget* widget){
  QWidget* slide = widget;
  //slide->move(0, -slide->height() - slide->height()/2);
  slide->setGeometry(0, -slide->height(), slide->width(), slide->height());
  QPropertyAnimation *animation = new QPropertyAnimation(slide, "pos");
  animation->setDuration(750);
  animation->setStartValue(slide->pos());
  animation->setEndValue(QPoint(slide->x(),0));
  animation->setEasingCurve(QEasingCurve::InQuart);
  animation->start();
}

/*
void SteganoDialog::slotButtonClicked() {
    KMessageBox::information( this, 
                              i18n("You pressed the button!" ),
                              i18n( "Hooray!" ) );
}

#include "../build/src/steganodialog.moc"
*/