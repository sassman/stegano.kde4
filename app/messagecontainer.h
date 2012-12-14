#ifndef MESSAGECONTAINER_H
#define MESSAGECONTAINER_H

#include <QString>
#include <QObject>
#include <QByteArray>
#include <QtCrypto/QtCrypto>

typedef uchar byte;

class IMessageContainer: public QObject {
    Q_OBJECT
public:
    virtual bool        isValidFormat(const QByteArray& bytes) = 0;

    virtual void        setText(const QString& text) = 0;
    virtual void        setBytes(const QByteArray& bytes) = 0;
    virtual QByteArray  bytes() = 0;
    virtual uint        length() = 0;
    virtual QString     text() = 0;
    virtual byte        version() = 0;
    virtual byte        terminator() = 0;
};

class MessageContainerBase : public IMessageContainer {
    Q_OBJECT
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

class MessageContainerWrapper : public IMessageContainer {
    Q_OBJECT
public:
    /**
     * TODO finalize that http://sourcemaking.com/design_patterns/decorator/cpp/2
     */
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
};

#endif // MESSAGECONTAINER_H
