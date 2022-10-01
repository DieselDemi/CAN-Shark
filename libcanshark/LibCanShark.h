#ifndef XDFCREATOR_LIBCANSHARK_H
#define XDFCREATOR_LIBCANSHARK_H

#include <QThread>
#include <QMutex>

class LibCanShark : public QThread {
    Q_OBJECT
public:
    explicit LibCanShark(QObject *parent = nullptr);
    ~LibCanShark() override;

    void connect(const QString &portName, int waitTimeout);
    void disconnect();
    void startRecording();
    void stopRecording();
    void sendFirmwareUpdate(QByteArray& data);

    void run() Q_DECL_OVERRIDE;

private:
    QString portname;
    QByteArray responseData;
    int waitTimeout = 0;
    QMutex mutex;
    bool quit = false;
    bool bRecording = false;
    bool connected = false;
    bool bStartRecording = false;
    bool bStopRecording = false;

signals:
    void response(const QString& s);
    void message(const QString& s);
    void error(const QString& s);
    void warn(const QString& s);
};


#endif //XDFCREATOR_LIBCANSHARK_H
