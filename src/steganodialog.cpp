#include <KLocale>
#include <KMessageBox>
#include <KIcon>
#include <KAction>
#include <KActionCollection>
#include <KStandardAction>

#include <QPropertyAnimation>
#include <KDE/Plasma/WindowEffects>
#include <KWindowSystem>

#include <QPushButton>
#include <QProgressDialog>
#include <QPropertyAnimation>
#include <QFileDialog>

// include the header file of the dialog
#include "steganodialog.h"

SteganoDialog::SteganoDialog( QWidget *parent ) : KXmlGuiWindow( parent ) {

    // create the user interface, the parent widget is "widget"
    steganoUI = new Ui::Stegano;
    QWidget* widget = new QWidget(this);
    this->steganoUI->setupUi(widget);
    this->setCentralWidget(widget);
    this->setupActions();
    
    this->setWindowIcon(KIcon("stegano"));

    // other KDialog options
    setCaption( i18n("Stegano") );

    connect( &stegano, SIGNAL( keyChanged(QString) ), 
             this->steganoUI->tHash, SLOT( setText(QString) ) 
    );
    connect( &stegano, SIGNAL( useCryptChanged(bool) ), 
             this->steganoUI->cUseEncryption, SLOT( setChecked(bool) ) 
    );
    connect( &stegano, SIGNAL( sourceMediaChanged(KUrl) ), 
             this->steganoUI->preview, SLOT( showPreview(KUrl) ) 
    );

    connect( this->steganoUI->tPassword, SIGNAL( userTextChanged(const QString&) ), 
             &stegano, SLOT( setPassword(const QString&) ) 
    );
    connect( this->steganoUI->cUseEncryption, SIGNAL( toggled(bool) ), 
             &stegano, SLOT( setUseCrypt(bool) ) 
    );
    connect( this->steganoUI->cFile, SIGNAL( textChanged(QString) ), 
             &stegano, SLOT( setSourceMedia(QString) ) 
    );
    connect( this->steganoUI->buttonHide, SIGNAL( clicked() ), 
             this, SLOT( hideData() ) 
    );

    connect( this->steganoUI->buttonUnhide, SIGNAL( clicked() ), 
             this, SLOT( unhideData() ) 
    );

    connect( this->steganoUI->buttonSave, SIGNAL( clicked() ), 
             this, SLOT( saveMedia() ) 
    );
    
    if(!stegano.isEncryptionSupported()) {
        this->steganoUI->groupEncryption->setEnabled(false);
    }
}

void SteganoDialog::setupActions() {

    KAction* action1 = new KAction(this);
    action1->setText(i18n("&Open Media"));
    action1->setIcon(KIcon("document-open"));
    action1->setShortcut(Qt::CTRL + Qt::Key_O);
    this->actionCollection()->addAction("open", action1);
    connect(action1, SIGNAL(triggered(bool)),
            this->steganoUI->cFile->button(), SLOT( click() )
    );

    action1 = new KAction(this);
    action1->setText(i18n("&Save as"));
    action1->setIcon(KIcon("document-save"));
    action1->setShortcut(Qt::CTRL + Qt::Key_S);
    this->actionCollection()->addAction("save as", action1);
    connect(action1, SIGNAL(triggered(bool)),
            this, SLOT( saveMedia() )
    );

    KStandardAction::quit(
        this , SLOT( close() ),
        actionCollection()
    );
    this->setupGUI();
}


void SteganoDialog::hideData() {

    if(this->steganoUI->cFile->text().isEmpty()) {
        this->steganoUI->cFile->setFocus();
        QMessageBox::information(this, "No media selected", "Please select an image to hide something");
        return;
    }

    if(this->steganoUI->tMessageText->toPlainText().isEmpty()) {
        this->steganoUI->tMessageText->setFocus();
        QMessageBox::information(this, "Nothing to hide", "Please type a message or add a file to hide.");
        return;
    }

    stegano.setSourceMedia(this->steganoUI->cFile->text());

    QProgressDialog progress("Hide Data...", "Cancel", 0, 100, this);
    progress.setWindowModality(Qt::WindowModal);
    QByteArray data = this->steganoUI->tMessageText->toPlainText().toUtf8();
    stegano.hideData(data, &progress);
    progress.close();
}

void SteganoDialog::unhideData() {

    if(this->steganoUI->cFile->text().isEmpty()) {
        this->steganoUI->cFile->setFocus();
        QMessageBox::information(this, "No Media given!", "Please give some Media to hide into..");
        return;
    }

    stegano.setSourceMedia(this->steganoUI->cFile->text());

    QProgressDialog progress("Unhide Data...", "Cancel", 0, 100, this);
    progress.setWindowModality(Qt::WindowModal);
    QByteArray data = stegano.unhideData(&progress);
    this->steganoUI->tMessageText->setPlainText(QString::fromUtf8(data));
    progress.close();
}


void SteganoDialog::saveMedia() {
    if(!stegano.sourceMedia()) {
        QMessageBox::information(this, "No Media!", "There is no Media to Save yet!");
        return;
    }

    QString filename = QFileDialog::getSaveFileName(this, "Save Media", this->steganoUI->cFile->text(), "Stegano Media (*.png)");
    //QString filename = cFile->text();
    if(!filename.isEmpty()) {
        stegano.sourceMedia()->save(filename);
        QMessageBox::information(this, "Media Saved", "The Stegano Media was saved to file!");
    }
}

void SteganoDialog::testingAnimations() {

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

void SteganoDialog::slideDown(QWidget* widget) {
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
