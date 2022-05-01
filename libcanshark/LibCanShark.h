#ifndef XDFCREATOR_LIBCANSHARK_H
#define XDFCREATOR_LIBCANSHARK_H

#include <QThread>
#include <QMutex>

class LibCanShark : public QThread {
public:
    explicit LibCanShark(QObject *parent = nullptr);
    ~LibCanShark() override;

    void connect(const QString &portName, int waitTimeout, const QString& response);

    void run() Q_DECL_OVERRIDE;

signals:
    void request(const QString& s);
    void error(const QString& s);
    void timeout(const QString& s);

private:
    QString portname;
    QString response;
    int waitTimeout;
    QMutex mutex;
    bool quit;
};


#endif //XDFCREATOR_LIBCANSHARK_H
