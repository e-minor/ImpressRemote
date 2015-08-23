#include "mytype.h"
#include <QFile>
#include <QByteArray>
#include <QUrl>
#include <QStandardPaths>
#include <QDir>
#include <QImage>



Impress::Impress(QObject *parent) :
    QObject(parent),
    m_hostAddr("127.0.0.1"),
    m_PIN("1111"),
    totalPages(1),
    curPage(0),
    m_message("")
{
    //connect_server();
}

Impress::~Impress() {

}

void Impress::emitConnectedState()
{
    qDebug() << "connected to libre impress server";

    emit connectedToImpressServer();

    pair();
}

void Impress::readMsg()
{
    char buff[1024];
    QString line;
    int cnt;

    while(tcpSocket->canReadLine())
    {
        qDebug() << "canReadLine, bytes: " << tcpSocket->bytesAvailable();

        cnt = tcpSocket->readLine(buff, 1024);

        line = QString::fromUtf8(buff);

        if(line == "\n") continue;

        qDebug() << line;

        if (line.indexOf("LO_SERVER_VALIDATING_PIN") >= 0)
        {
            emit shouldSetPINinImpress(m_PIN);
        }
        else if (line.indexOf("LO_SERVER_SERVER_PAIRED") >= 0)
        {
            emit paired();

            start();

            //qDebug() << "print note 0";
            //qDebug() << getNote(0);
        }
        else if (line.indexOf("LO_SERVER_INFO") >= 0)
        {
            status = ST_READ_STARTED_OR_NOT;
            //tcpSocket->readLine(buff, 1024);
        }
        else if(line.indexOf("slideshow_started") >= 0)
        {
            tcpSocket->readLine(buff, 1024);
            line = QString::fromUtf8(buff);
            totalPages = line.toInt();

            tcpSocket->readLine(buff, 1024);
            line = QString::fromUtf8(buff);
            curPage = line.toInt();

            emit reportPageNumberGot(totalPages, curPage);

            if (status == ST_READ_STARTED_OR_NOT)
            {
                emit reportStartupStatus(true);
                status = ST_IDLE;
            }

        }
        else if(line.indexOf("slide_updated") >= 0)
        {
            tcpSocket->readLine(buff, 1024);
            line = QString::fromUtf8(buff);
            qint32 p = line.toInt();


            emit pageUpdated(p);

        }
        else if (line.indexOf("slide_preview") >= 0)
        {
            status = ST_READ_PNG_PAGE;

            /*tcpSocket->readLine(buff, 1024);
            line = QString::fromUtf8(buff);
            int pageNum = line.toInt();

            QString fn = QString("preview_of_page_%1.png").arg(pageNum);
            fn = getFilePath(fn);

            QByteArray png_base64;
            do
            {
                int cnt = tcpSocket->readLine(buff, 1024);
                line = QString::fromUtf8(buff);
                //png_base64 += line;
                png_base64.append(buff, cnt);
            }while(line.indexOf('\n')<0);

            png_base64 = png_base64.left(png_base64.length()-1);

            //qDebug() << png_base64;

            QImage image;
            image.loadFromData(QByteArray::fromBase64(png_base64), "PNG");
            qDebug() << fn;
            image.save(fn, "PNG");*/

        }
        else if (line.indexOf("slide_notes") >= 0)
        {
            status = ST_READ_NOTE_PAGE;
            /*tcpSocket->readLine(buff, 1024);
            line = QString::fromUtf8(buff);
            qint32 pageNum = line.remove('\n').toInt();
            QString a = QString("notes_of_page_%1.txt").arg(pageNum);
            QFile file(getFilePath(a));
            file.open(QIODevice::WriteOnly);

            do
            {
                int cnt = tcpSocket->readLine(buff, 1024);
                line = QString::fromUtf8(buff);
                qDebug() << line;
                file.write(buff, cnt);
            }while(line.indexOf('\n')<0);
            file.close();*/
        }else
        {
            if (status == ST_READ_NOTE_PAGE)
            {
                //tcpSocket->readLine(buff, 1024);
                //line = QString::fromUtf8(buff);
                qint32 pageNum = line.remove('\n').toInt();
                QString a = QString("notes_of_page_%1.txt").arg(pageNum);
                m_file = new QFile(getFilePath(a));
                m_file->open(QIODevice::WriteOnly);
                status = ST_READ_NOTE;
            }
            else if (status == ST_READ_NOTE)
            {
                //int cnt = tcpSocket->readLine(buff, 1024);
                //line = QString::fromUtf8(buff);
                qDebug() << line;
                m_file->write(buff, cnt);
                if (line.indexOf('\n') >= 0)
                {
                    status = ST_IDLE;
                    m_file->close();
                }
            }
            else if(status == ST_READ_PNG_PAGE)
            {

                //tcpSocket->readLine(buff, 1024);
                //line = QString::fromUtf8(buff);
                int pageNum = line.toInt();

                QString fn = QString("preview_of_page_%1.png").arg(pageNum);
                m_pngFileName = getFilePath(fn);

                png_base64 = new QByteArray();

                status = ST_READ_PNG;
            }
            else if (status == ST_READ_PNG)
            {
                //int cnt = tcpSocket->readLine(buff, 1024);
                //line = QString::fromUtf8(buff);
                //png_base64 += line;
                png_base64->append(buff, cnt);
                if(line.indexOf('\n') >= 0)
                {
                    QByteArray ar = png_base64->left(png_base64->length()-1);

                    QImage image;
                    image.loadFromData(QByteArray::fromBase64(ar), "PNG");
                    qDebug() << m_pngFileName;
                    image.save(m_pngFileName, "PNG");

                    status = ST_IDLE;

                    delete png_base64;

                }
            }
        }
    }
}

void Impress::sendMsg(QString msg)
{
    QByteArray array (msg.toStdString().c_str());
    tcpSocket->write(array);
}

void Impress::connect_server()
{
    tcpSocket = new QTcpSocket(this);
    tcpSocket->connectToHost(m_hostAddr, 1599);
    connect(tcpSocket, SIGNAL(connected()),this, SLOT(emitConnectedState()));
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readMsg()));

    qDebug() << "connect request issued! ";
}

void Impress::pair()
{

    sendMsg("LO_SERVER_CLIENT_PAIR\nubuntu Phone\n"+m_PIN+"\n\n");
}

void Impress::start()
{
    sendMsg("presentation_start\n\n");
}

void Impress::stop()
{
    sendMsg("presentation_stop\n\n");
}

void Impress::next_page()
{
    sendMsg("transition_next\n\n");
}

void Impress::prev_page()
{
    sendMsg("transition_previous\n\n");
}

void Impress::goto_page(int page)
{
    sendMsg("goto_slide\n"+QString(page)+"\n\n");
}


QString Impress::getFilePath(const QString filename) const
{
    QString writablePath = QStandardPaths::
            writableLocation(QStandardPaths::DataLocation);
    qDebug() << "writablePath: " << writablePath;

    QString absolutePath = QDir(writablePath).absolutePath();
    qDebug() << "absoluePath: " << absolutePath;

    // We need to make sure we have the path for storage
    QDir dir(absolutePath);
    if ( dir.mkdir(absolutePath) ) {
        qDebug() << "Successfully created the path!";
    }

    QString path = absolutePath + "/" + filename;

    qDebug() << "path: " << path;

    return path;
}

QString Impress::getNotesPath(int pageNum)
{
    return getFilePath(QString("notes_of_page_%1.txt").arg(pageNum));
}

QString Impress::getPreviewPath(int pageNum)
{
    return getFilePath(QString("preview_of_page_%1.png").arg(pageNum));
}

QString Impress::getNote(int pageNum)
{
    QString path = getNotesPath(pageNum);
    QFile file(path);
    file.open(QIODevice::ReadOnly);
    //QTextStream out(&file);
    QString content;
    content = QString(file.readAll());
    //out >> content;
    file.close();
    return content;
}

