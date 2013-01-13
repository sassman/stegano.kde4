#ifndef MESSAGECONTAINER_H
#define MESSAGECONTAINER_H

#include <QString>
#include <QStringList>
#include <QObject>
#include <QByteArray>
#include <QFile>
#include <QBuffer>
#include <QtDebug>
#include <QtCrypto/QtCrypto>
#include <quazip/quazip.h>

typedef uchar byte;

class ISteganoContainer {
public:
    virtual bool        isValidFormat(const QByteArray& bytes) = 0;
    virtual ~ISteganoContainer() {};
};
Q_DECLARE_INTERFACE(ISteganoContainer,
                    "biz.lubico.Stegano.ISteganoContainer/1.0")

class IMessageContainer : public ISteganoContainer{
public:
    virtual ~IMessageContainer() {};
    virtual void        setText(const QString& text) = 0;
    virtual void        setBytes(const QByteArray& bytes) = 0;
    virtual QByteArray  bytes() = 0;
    virtual uint        length() = 0;
    virtual QString     text() = 0;
    virtual byte        version() = 0;
    virtual byte        terminator() = 0;
    virtual void        saveRawFile(const QString& filename) = 0;
};
Q_DECLARE_INTERFACE(IMessageContainer,
                    "biz.lubico.Stegano.IMessageContainer/1.0")

class IDocumentContainer : public ISteganoContainer{
public: 
    virtual ~IDocumentContainer() {}
    virtual bool        addFile(QFile) = 0;
    virtual bool        removeFile(const QString& filename) = 0;
    virtual bool        extractFile(const QString& filename, QFile& target) = 0;
    virtual QStringList files() = 0;
    virtual int         count() = 0;
};
Q_DECLARE_INTERFACE(IDocumentContainer,
                    "biz.lubico.Stegano.IDocumentContainer/1.0")

class MessageContainerBase : public QObject, public IMessageContainer {
    Q_OBJECT
    Q_INTERFACES(IMessageContainer ISteganoContainer)
public:
                        MessageContainerBase();
    virtual             ~MessageContainerBase();

    virtual void        setText(const QString& text);
    virtual void        setBytes(const QByteArray& bytes);

    virtual QByteArray  bytes();
    
    virtual uint        length();
    virtual byte        version()       { return Version; }
    virtual byte        terminator()    { return Terminator; }
    virtual QString     text()          { return textBlock; }
    
    virtual void        saveRawFile(const QString& filename);
    
signals:
    void        textChanged(QString);
    void        bytesChanged(QByteArray);
    void        versionChanged(byte);

protected:
                        MessageContainerBase(byte version, byte terminator) : 
                            Version(version), 
                            Terminator(terminator) { }
    
    QString     textBlock;
    const byte  Version;
    const byte  Terminator;
    QByteArray  bytesBuffer;
    IMessageContainer *wrapMe;

};

class MessageContainerWrapper : public QObject, public IMessageContainer {
    Q_OBJECT
    Q_INTERFACES(IMessageContainer ISteganoContainer)
public:
    MessageContainerWrapper(IMessageContainer* wrapee);
    virtual ~MessageContainerWrapper();
    
    virtual QByteArray bytes()                          { return this->wrapee->bytes(); }
    virtual bool isValidFormat(const QByteArray& bytes) { return this->wrapee->isValidFormat(bytes); }
    virtual uint length()                               { return this->wrapee->length(); }                              
    virtual void setBytes(const QByteArray& bytes)      { this->wrapee->setBytes(bytes); }
    virtual void setText(const QString& text)           { this->wrapee->setText(text); }
    virtual byte terminator()                           { return this->wrapee->terminator(); }
    virtual QString text()                              { return this->wrapee->text(); }
    virtual byte version()                              { return this->wrapee->version(); }
    virtual void saveRawFile(const QString& filename)   { this->wrapee->saveRawFile(filename); }

protected:
    IMessageContainer* wrapee;    
};

class MessageContainerEncypted : public MessageContainerWrapper {
    Q_OBJECT
public:
    MessageContainerEncypted(IMessageContainer* wrapee, const QByteArray& key, const QString& algorithm);
    virtual             ~MessageContainerEncypted();
    virtual QByteArray  bytes();
    virtual bool        isValidFormat(const QByteArray& bytes);
    bool                isEncryptionSupported();
protected:
    byte                version;
    QByteArray          key;
    QString             algorithm;
    QString             algorithmType;
    QCA::Cipher*        cipher;
    
    QCA::Initializer*   init;
    QCA::Cipher*        createCipher(QCA::Direction direction);
};

class MessageContainerV0 : public MessageContainerBase {
    Q_OBJECT
public:
    MessageContainerV0();
    virtual ~MessageContainerV0();

    virtual bool        isValidFormat(const QByteArray& bytes);
};

class MessageContainerV1 : public MessageContainerBase {
    Q_OBJECT
public:
    MessageContainerV1();
    virtual ~MessageContainerV1();

    virtual bool        isValidFormat(const QByteArray& bytes);
    
protected:
    MessageContainerV1(byte version, byte terminator);
};

class MessageContainerV2 : public MessageContainerV1, public IDocumentContainer {
    Q_OBJECT
    Q_INTERFACES(IDocumentContainer)
public:
    MessageContainerV2();
    virtual ~MessageContainerV2();

    virtual bool        isValidFormat(const QByteArray& bytes);
    virtual QString     text();
    
    virtual bool        addFile(QFile file);
    virtual bool        removeFile(const QString& filename);
    virtual bool        extractFile(const QString& filename, QFile& target);
    virtual int         count();
    virtual QStringList files();
protected:
    QBuffer *buffer;
    QuaZip  *archive;
};

#endif // MESSAGECONTAINER_H
