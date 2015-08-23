#ifndef MYTYPE_H
#define MYTYPE_H

#include <QObject>
#include <QTcpSocket>

enum{ST_IDLE, ST_READ_STARTED_OR_NOT, ST_xxx};

class Impress : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString helloWorld READ helloWorld WRITE setHelloWorld NOTIFY helloWorldChanged )
    Q_PROPERTY(QString PIN READ PIN WRITE setPIN NOTIFY PINChanged)
    Q_PROPERTY(QString hostAddr READ hostAddr WRITE setHostAddr)
    Q_PROPERTY(qint32 totalPages READ getTotalPages)
    Q_PROPERTY(qint32 curPage READ getCurPage)

public:
    explicit Impress(QObject *parent = 0);
    ~Impress();

signals:
    void helloWorldChanged();
    void PINChanged();
    void connectedToImpressServer();
    void shouldSetPINinImpress(QString PIN);
    void paired();
    void reportStartupStatus(bool started);
    void reportPageNumberGot(qint32 total, qint32 cur);

public slots:
    void connect_server();
    void pair();
    void start();
    void stop();
    void next_page();
    void prev_page();
    void goto_page(int page);
    QString getNotesPath(int pageNum);
    QString getPreviewPath(int pageNum);
    QString getNote(int pageNum);



private slots:
    void emitConnectedState();
    void readMsg();


protected:
    QString helloWorld() { return m_message; }
    void setHelloWorld(QString msg) { m_message = msg; Q_EMIT helloWorldChanged(); }

    QString hostAddr() { return m_hostAddr; }
    void setHostAddr(QString addr) { m_hostAddr = addr; }

    QString PIN() { return m_PIN; }
    void setPIN(QString p) { m_PIN = p; emit PINChanged(); }

    qint32 getTotalPages() { return totalPages;}
    qint32 getCurPage() {return curPage;}

    void sendMsg(QString msg);

    QString getFilePath(const QString filename) const;

    QString m_message;
    QString m_hostAddr;
    QString m_PIN;

    QTcpSocket *tcpSocket;
    QString respStr;
    quint16 blockSize;

    int status;
    int totalPages;
    int curPage;
};

#endif // MYTYPE_H

