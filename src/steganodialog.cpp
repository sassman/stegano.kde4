#include <KLocale>
#include <KMessageBox>
#include <KIcon>
#include <KAction>
#include <KActionCollection>
#include <KStandardAction>

#include <QPropertyAnimation>
#include <QPushButton>
#include <QProgressDialog>
#include <QPropertyAnimation>
#include <QFileDialog>

#include <KDE/Plasma/WindowEffects>
#include <KWindowSystem>
#include <KMessageBox>

// include the header file of the dialog
#include "steganodialog.h"

SteganoDialog::SteganoDialog( QWidget *parent ) : 
    KXmlGuiWindow( parent ),
    isHidden(false),
    fileFilterSource(i18n("Stegano Media (*.png *.jpg *.jpeg *.xpm)")),
    //fileFilterTarget("Stegano Media (*.jpg *.jpeg *.xpm);;Stegano Media Windows Compatible(*.png)")
    fileFilterTarget(i18n("Stegano Media (*.png)"))
{

    // create the user interface, the parent widget is "widget"
    steganoUI = new Ui::Stegano;
    QWidget* widget = new QWidget(this);
    this->steganoUI->setupUi(widget);
    this->setCentralWidget(widget);
    this->setupActions();
    
    this->setWindowIcon(KIcon("stegano"));

    // other KDialog options
    setCaption( i18n("Stegano") );

    connect( &stegano, SIGNAL( sourceMediaChanged(KUrl) ), 
        this->steganoUI->preview, SLOT( showPreview(KUrl) ) 
    );
    connect( &stegano, SIGNAL( sourceMediaChanged(KUrl) ), 
        this, SLOT( showSize() ) 
    );
    connect( &stegano, SIGNAL( sourceMediaChanged(KUrl) ), 
        this, SLOT( showCharacters() ) 
    );
    connect( &stegano, SIGNAL( keyChanged(QString) ), 
        this->steganoUI->tHash, SLOT( setText(QString) ) 
    );
    connect( &stegano, SIGNAL( useCryptChanged(bool) ), 
        this->steganoUI->cUseEncryption, SLOT( setChecked(bool) ) 
    );


    connect( this->steganoUI->tPassword, SIGNAL( userTextChanged(const QString&) ), 
        &stegano, SLOT( setPassword(const QString&) ) 
    );
    connect( this->steganoUI->cUseEncryption, SIGNAL( toggled(bool) ), 
        &stegano, SLOT( setUseCrypt(bool) ) 
    );
    connect( this->steganoUI->tMessageText, SIGNAL( textChanged() ), 
        &stegano, SLOT( setToHideFlag() ) 
    );
    connect( this->steganoUI->tMessageText, SIGNAL( textChanged() ), 
        this, SLOT( showCharacters() ) 
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
        this, SLOT( sourceMediaChange() )
    );

    action1 = new KAction(this);
    action1->setText(i18n("&Save as"));
    action1->setIcon(KIcon("document-save"));
    action1->setShortcut(Qt::CTRL + Qt::Key_S);
    this->actionCollection()->addAction("save as", action1);
    connect(action1, SIGNAL(triggered(bool)),
        this, SLOT( saveMedia() )
    );

    action1 = new KAction(this);
    action1->setText(i18n("&Unhide"));
    action1->setIcon(KIcon("document-preview-archive"));
    //action1->setShortcut(Qt::CTRL + Qt::Key_S);
    this->actionCollection()->addAction("unhide", action1);
    connect(action1, SIGNAL(triggered(bool)),
        this, SLOT( unhideData() )
    );

    KStandardAction::quit(
        this , SLOT( close() ),
        actionCollection()
    );
    this->setupGUI();
}


bool SteganoDialog::hideData() {

    if(!this->stegano.isSourceMediaValid()) {
        this->noValidSourceMedia();
        return false;
    }

    if(this->steganoUI->tMessageText->toPlainText().isEmpty()) {
        this->noValidMessage();
        return false;
    }

    QProgressDialog progress(i18n("Hide Data..."), i18n("Cancel"), 0, 100, this);
    progress.setWindowModality(Qt::WindowModal);
    QByteArray data = this->steganoUI->tMessageText->toPlainText().toUtf8();
    stegano.hideData(data, &progress);
    progress.close();
    return this->isHidden = true;
}

bool SteganoDialog::unhideData() {

    if(!this->stegano.isSourceMediaValid()) {
        this->noValidSourceMedia();
        return false;
    }

    QProgressDialog progress(i18n("Unhide Data..."), i18n("Cancel"), 0, 100, this);
    progress.setWindowModality(Qt::WindowModal);
    QByteArray data = stegano.unhideData(&progress);
    this->steganoUI->tMessageText->setPlainText(QString::fromUtf8(data));
    progress.close();
    return true;
}


void SteganoDialog::saveMedia() {

    if(!this->isHidden && !this->hideData()) {
        return;
    }

    QString filename = QFileDialog::getSaveFileName(
        this, 
        i18n("Save Media"), 
        QString(),
        this->fileFilterTarget
    );
    if(!filename.isEmpty()) {   // TODO check for writing permissions
        stegano.sourceMedia()->save(filename);
        KMessageBox::information(
            this, 
            i18n("The Stegano Media was saved to file"),
            i18n("Media Saved") 
        );
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

void SteganoDialog::setToHideFlag() {
    this->isHidden = false;
}

void SteganoDialog::sourceMediaChange(){
    
    QString filename = QFileDialog::getOpenFileName(
        this, 
        i18n("Open Media for Message"), 
        QString(),
        this->fileFilterSource
    );
    if(!filename.isEmpty()) {
        stegano.setSourceMedia(filename);
        this->setToHideFlag();
    }
}
void SteganoDialog::noValidSourceMedia() {
    KMessageBox::information(
        this, 
        i18n("Please give some valid source media to extract data from it"),
        i18n("No valid source media found")
    );
}
void SteganoDialog::noValidTargetMedia() {
    KMessageBox::information(
        this, 
        i18n("Please select an image to hide the data in it"),
        i18n("No valid target media found")
    );
}
void SteganoDialog::noValidMessage() {
    this->steganoUI->tMessageText->setFocus();
    KMessageBox::information(
        this, 
        i18n("Please type a message or add a file that can be hidden in the source media"),
        i18n("Nothing to hide")
    );
}

void SteganoDialog::showSize() {
    QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedStates));
    QString size = QString("%L1 Byte").arg(this->stegano.getMaximumMessageSize());
    //this->steganoUI->lCharsAvailable->setText( size );
}
void SteganoDialog::showCharacters() {
    QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedStates));
    long chars_max = this->stegano.getMaximumMessageSize() / 2;
    long chars_used = chars_max - this->steganoUI->tMessageText->toPlainText().length();
    QString size = QString("%L1 characters left").arg(chars_used);
    this->steganoUI->lCharsAvailable->setText( size );
    
    int chars_percent = 0;
    if ( chars_max > 0 && (chars_percent = (chars_used * 100 ) / chars_max) > 0) {
        this->steganoUI->kcapacitybar->setValue( chars_percent );
        if( chars_percent < 20 ) {
            this->steganoUI->kcapacitybar->setStyleSheet("background-color: #f97b6e;");
        } else {
            this->steganoUI->kcapacitybar->setStyleSheet("");
        }
    }
}

#include "../build/src/steganodialog.moc"
