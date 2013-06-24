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
#include <KStatusBar>
#include <KMessageBox>

// include the header file of the dialog
#include "steganodialog.h"

namespace Stegano {

SteganoDialog::SteganoDialog( QWidget *parent ) : 
    KXmlGuiWindow( parent ),
    isHidden(false),
    fileFilterSource(i18n("Stegano Media (*.png *.jpg *.jpeg *.xpm)")),
    //fileFilterTarget("Stegano Media (*.jpg *.jpeg *.xpm);;Stegano Media Windows Compatible(*.png)")
    fileFilterTarget(i18n("Stegano Media (*.png)"))
{

    // create the user interface, the parent widget is "widget"
    //steganoUI = new Ui::SteganoClean;
    //this->wrapperWidget = new QStackedWidget(this);
    //this->steganoUI->setupUi((QStackedWidget*) this->wrapperWidget);
    //this->setCentralWidget(this->wrapperWidget);
    
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
    
    // encryption on / off
    this->steganoUI->encryptionWidget->setVisible(false);
    connect( this->steganoUI->encryptionToggle, SIGNAL( toggled(bool) ), 
        this, SLOT( toggleEncryption(bool) ) 
    );
    // TODO toggle the icon to document-encrypt

    connect( this->steganoUI->passwordText, SIGNAL( userTextChanged(const QString&) ), 
        &stegano, SLOT( setPassword(const QString&) ) 
    );
    connect( this->steganoUI->messageText, SIGNAL( textChanged() ), 
        this, SLOT( setToHideFlag() ) 
    );
    connect( this->steganoUI->messageText, SIGNAL( textChanged() ), 
        this, SLOT( showCharacters() ) 
    );
    
    connect( this->steganoUI->openMediaButton, SIGNAL( clicked() ),
        this, SLOT( sourceMediaChange() )
    );
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
    this->openMediaAction = action1;

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

    if(this->steganoUI->messageText->toPlainText().isEmpty()) {
        this->noValidMessage();
        return false;
    }

    QProgressDialog progress(i18n("Hide Data..."), i18n("Cancel"), 0, 100, this);
    progress.setWindowModality(Qt::WindowModal);
    stegano.hideData(this->steganoUI->messageText->toPlainText(), &progress);
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
    QString message = stegano.unhideData(&progress);
    this->steganoUI->messageText->setPlainText(message);
    progress.close();
    this->steganoUI->mainWidget->setCurrentWidget(this->steganoUI->messageTab);
    this->steganoUI->messageText->setFocus();
    return true;
}


void SteganoDialog::saveMedia() {

    if(!this->isHidden && !this->hideData()) {
        return;
    }

    KSharedConfigPtr config = KGlobal::config();
    KConfigGroup generalGroup( config, "General" );
    QString cfgKey("lastOpenDir");

    QString filename = QFileDialog::getSaveFileName(
        this, 
        i18n("Save Media"), 
        generalGroup.readPathEntry(cfgKey, QString("")),
        this->fileFilterTarget
    );
    QFileInfo info(filename);
    QDir      dir = info.dir().absolutePath();
    if( !info.exists() || info.isWritable() ) {
        stegano.sourceMedia()->save(filename);
        KMessageBox::information(
            this, 
            i18n("The Stegano Media was saved to file"),
            i18n("Media Saved") 
        );
    }else {
        this->noValidPermissionsOnMedia();
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

    KSharedConfigPtr config = KGlobal::config();
    KConfigGroup generalGroup( config, "General" );
    QString cfgKey("lastOpenDir");
    
    QString filename = QFileDialog::getOpenFileName(
        this, 
        i18n("Open Media for Message"), 
        generalGroup.readPathEntry(cfgKey, QString("")),
        this->fileFilterSource
    );
    QFileInfo info(filename);
    if( info.isReadable() ) {
        stegano.setSourceMedia(filename);
        this->setToHideFlag();
        generalGroup.writePathEntry(cfgKey, info.dir().absolutePath() );
        this->steganoUI->messageText->clear();
        this->steganoUI->mediaWidget->setCurrentWidget(this->steganoUI->previewMediaPage);
        this->steganoUI->messageTab->setEnabled(true);
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
void SteganoDialog::noValidPermissionsOnMedia() {
    KMessageBox::information(
        this, 
        i18n("You are not allowed to perform this operation on the selected media"),
        i18n("This operation is not allowed")
    );
}
void SteganoDialog::noValidMessage() {
    this->steganoUI->messageText->setFocus();
    KMessageBox::information(
        this, 
        i18n("Please type a message or add a file that can be hidden in the source media"),
        i18n("Nothing to hide")
    );
}

void SteganoDialog::showCharacters() {
    QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedStates));
    long chars_max = this->stegano.getMaximumMessageSize() / 2;
    long chars_used = chars_max - this->steganoUI->messageText->toPlainText().length();
    QString size = QString("%L1 characters left").arg(chars_used);
    this->statusBar()->clearMessage();
    this->statusBar()->showMessage(size);
    
    int chars_percent = 100 - (chars_used * 100 ) / chars_max;
    if ( chars_max > 0 && chars_percent >= 0) {
        this->steganoUI->capacityBar->setValue( chars_percent );
        if( chars_percent > 80 ) {
            QPalette p = this->steganoUI->capacityBar->palette();
            p.setColor(QPalette::Highlight, QColor(Qt::red));
            this->steganoUI->capacityBar->setPalette(p);
        } else {
            this->steganoUI->capacityBar->setPalette(QPalette());
        }
    }
}

void SteganoDialog::toggleEncryption(bool enabled) {
    this->stegano.setUseCrypt(enabled);
    this->steganoUI->encryptionWidget->setVisible(enabled); 
    this->steganoUI->passwordText->setFocus();
    this->steganoUI->encryptionToggle->setIcon(KIcon(enabled ? "document-encrypt" : "document-decrypt"));
}


#include "../build/src/steganodialog.moc"

}   
