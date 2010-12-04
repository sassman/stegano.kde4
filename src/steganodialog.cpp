#include <KLocale>
#include <KMessageBox>

#include <QPropertyAnimation>
#include <KDE/Plasma/WindowEffects>
#include <KWindowSystem>

#include <QPushButton>
#include <QProgressDialog>
#include <QPropertyAnimation>
#include <QFileDialog>

// include the header file of the dialog
#include "steganodialog.h"

SteganoDialog::SteganoDialog( QWidget *parent ) : KDialog( parent ) {

	// create the user interface, the parent widget is "widget"
	this->setupUi(this); // this is the important part

	// other KDialog options
	setCaption( i18n("Stegano") );
	setButtons( NULL );

	connect( &stegano, SIGNAL( keyChanged(QString) ), this->tHash, SLOT( setText(QString) ) );
	connect( &stegano, SIGNAL( useCryptChanged(bool) ), this->cUseEncryption, SLOT( setChecked(bool) ) );
	connect( &stegano, SIGNAL( sourceMediaChanged(KUrl) ), this->preview, SLOT( showPreview(KUrl) ) );

	connect( this->tPassword, SIGNAL( userTextChanged(const QString&) ), &stegano, SLOT( setPassword(const QString&) ) );
	connect( this->cUseEncryption, SIGNAL( toggled(bool) ), &stegano, SLOT( setUseCrypt(bool) ) );
	connect( this->cFile, SIGNAL( textChanged(QString) ), &stegano, SLOT( setSourceMedia(QString) ) );
	connect( this->buttonHide, SIGNAL( clicked() ), this, SLOT( hideData() ) );

	connect( this->buttonUnhide, SIGNAL( clicked() ), this, SLOT( unhideData() ) );

	connect( this->buttonSave, SIGNAL( clicked() ), this, SLOT( saveMedia() ) );
	
}

void SteganoDialog::hideData() {

	if(this->cFile->text().isEmpty()) {
		this->cFile->setFocus();
		QMessageBox::information(this, "No Media given!", "Please give some Media to hide into..");
		return;
	}
	
	if(this->tMessageText->toPlainText().isEmpty()) {
		this->tMessageText->setFocus();
		QMessageBox::information(this, "Nothing to Hide!", "Please insert a Message to hide");
		return;
	}

	QProgressDialog progress("Hide Data...", "Cancel", 0, 100, this);
	progress.setWindowModality(Qt::WindowModal);
	progress.show();

	stegano.setSourceMedia(this->cFile->text());
	stegano.hideData(this->tMessageText->toPlainText().toUtf8(), &progress);
}

void SteganoDialog::unhideData() {

	if(this->cFile->text().isEmpty()) {
		this->cFile->setFocus();
		QMessageBox::information(this, "No Media given!", "Please give some Media to hide into..");
		return;
	}

	QProgressDialog progress("Unhide Data...", "Cancel", 0, 100, this);
	progress.setWindowModality(Qt::WindowModal);
	progress.show();

	stegano.setSourceMedia(this->cFile->text());
	QByteArray data = stegano.unhideData(&progress);
	this->tMessageText->setPlainText(QString::fromUtf8(data.data()));
}


void SteganoDialog::saveMedia() {
	if(!stegano.sourceMedia()) {
		QMessageBox::information(this, "No Media!", "There is no Media to Save yet!");
		return;
	}

// 	QString filename = QFileDialog::getSaveFileName(this, "Save Media", "/home/sassman/", "Stegano Media (*.png)");
	QString filename = cFile->text();
	if(!filename.isEmpty()) {
		stegano.sourceMedia()->save(filename);
		QMessageBox::information(this, "Media Saved", "The Stegano Media was saved to file!");
	}
}

void SteganoDialog::testingAnimations(){

	/* animation snippets
		*	QPropertyAnimation *animation = new QPropertyAnimation(&progress, "geometry");
		*	animation->setDuration(10000);
		*	animation->setStartValue(progress.geometry());
		*	QRect dest = progress.geometry();
		*	dest.setLeft(dest.left() + 100);
		*	animation->setEndValue(dest);
		*	animation->start();
		*/
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

#include "../build/src/steganodialog.moc"
