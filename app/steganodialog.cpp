#include <KDebug>
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
#include <QMimeData>
#include <QDropEvent>

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
        this, SLOT( onChanged() ) 
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
        this, SLOT( onChanged() ) 
    );
    
    connect( this->steganoUI->openMediaButton, SIGNAL( clicked() ),
        this, SLOT( sourceMediaChange() )
    );
    
    connect(this, SIGNAL(modificationChanged(bool)),
        this->steganoUI->messageText->document(), SLOT(setModified(bool))
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
    this->actionOpenMedia = action1;

    action1 = new KAction(this);
    action1->setText(i18n("&Save as"));
    action1->setIcon(KIcon("document-save"));
    action1->setShortcut(Qt::CTRL + Qt::Key_S);
    this->actionCollection()->addAction("save as", action1);
    connect(action1, SIGNAL(triggered(bool)),
        this, SLOT( saveMedia() )
    );
    connect(this, SIGNAL(modificationChanged(bool)),
        action1, SLOT(setEnabled(bool)) 
    );
    action1->setEnabled(false);
    this->actionSaveAs = action1;    

    action1 = new KAction(KIcon("document-decrypt"), i18n("&Protect"), this);
    action1->setToolTip(i18n("Turn password protection on or off"));
    this->actionCollection()->addAction("protect", action1);
    connect(action1, SIGNAL(triggered(bool)),
        this->steganoUI->encryptionToggle, SLOT(setChecked(bool))
    );
    action1->setCheckable(true);
    action1->setEnabled(false);
    this->actionProtect = action1;    

    action1 = new KAction(this);
    action1->setText(i18n("&Decrypt"));
    action1->setIcon(KIcon("document-preview-archive"));
    //action1->setShortcut(Qt::CTRL + Qt::Key_S);
    this->actionCollection()->addAction("unhide", action1);
    connect(action1, SIGNAL(triggered(bool)),
        this, SLOT( unhideData() )
    );
    action1->setEnabled(false);
    this->actionUnhide = action1;

    KStandardAction::quit(
        this , SLOT( close() ),
        actionCollection()
    );
    this->setupGUI();
    this->setAcceptDrops(true);
}

void SteganoDialog::dropEvent ( QDropEvent* event) {
    kDebug()<<"foo bar";
    if(!this->isValidMimeData(event->mimeData())) {
        return;
    }
    
    QList<QUrl> urls = event->mimeData()->urls();
    QUrl url = urls.last();
    
    kDebug()<<url.path();
    sourceMediaChange(url.path());
    
    event->acceptProposedAction();
}

void SteganoDialog::dragEnterEvent ( QDragEnterEvent* event) {
    if(!this->isValidMimeData(event->mimeData())) {
        return;
    }
    event->acceptProposedAction();
}

void SteganoDialog::dragLeaveEvent ( QDragLeaveEvent* event) {
    event->accept();
}

void SteganoDialog::dragMoveEvent ( QDragMoveEvent* event) {
    if(!this->isValidMimeData(event->mimeData())) {
        return;
    }
    event->acceptProposedAction();
}

bool SteganoDialog::isValidMimeData ( const QMimeData* mime ) {
    if(!mime->hasUrls()) {
        return false;
    }
    return true;
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
        emit(modificationChanged(false));
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
        sourceMediaChange(info.dir().absolutePath());
        generalGroup.writePathEntry(cfgKey, info.dir().absolutePath());        
    }
}

void SteganoDialog::sourceMediaChange ( QString absolutePath ) {
    stegano.setSourceMedia(absolutePath);
    this->setToHideFlag();
    this->steganoUI->messageText->clear();
    this->steganoUI->mediaWidget->setCurrentWidget(this->steganoUI->previewMediaPage);
    this->steganoUI->messageTab->setEnabled(true);
    this->actionUnhide->setEnabled(true);
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

void SteganoDialog::onChanged() {
    QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedStates));
    long chars_max = this->stegano.getMaximumMessageSize() / 2;
    long chars_used = chars_max - this->steganoUI->messageText->toPlainText().length();
    QString size = QString("%L1 characters left").arg(chars_used);
    this->statusBar()->clearMessage();
    this->statusBar()->showMessage(size);
    
    if ( chars_max > 0 ) {
        int chars_percent = 100 - (chars_used * 100 ) / chars_max;
        this->steganoUI->capacityBar->setValue( chars_percent );
        if( chars_percent > 80 ) {
            QPalette p = this->steganoUI->capacityBar->palette();
            p.setColor(QPalette::Highlight, QColor(Qt::red));
            this->steganoUI->capacityBar->setPalette(p);
        } else {
            this->steganoUI->capacityBar->setPalette(QPalette());
        }
    }
    
    this->actionProtect->setEnabled(true);
    this->actionSaveAs->setEnabled(true);
    QTextDocument* document = this->steganoUI->messageText->document();
    if(document) {
        emit(modificationChanged(document->isModified()));
    }
}

void SteganoDialog::toggleEncryption(bool enabled) {
    this->stegano.setUseCrypt(enabled);
    this->steganoUI->encryptionWidget->setVisible(enabled); 
    this->steganoUI->passwordText->setFocus();
    this->steganoUI->encryptionToggle->setIcon(KIcon(enabled ? "document-encrypt" : "document-decrypt"));
    this->actionProtect->setIcon(this->steganoUI->encryptionToggle->icon());
    this->actionProtect->setText(i18n(enabled ? "Unprotect" : "Protect"));
    emit(modificationChanged(true));
}


#include "../build/src/steganodialog.moc"

}   
