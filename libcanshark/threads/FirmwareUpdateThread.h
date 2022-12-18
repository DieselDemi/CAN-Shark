#ifndef CANSHARK_FIRMWAREUPDATETHREAD_H
#define CANSHARK_FIRMWAREUPDATETHREAD_H

#include <QThread>
#include <QSerialPort>
#include <QMutex>

namespace dd::libcanshark::threads {
    enum class FirmwareUpdateThreadStatus {
        Success,
        Fail
    };

    class FirmwareUpdateThread : public QThread {
        Q_OBJECT
    public:
        FirmwareUpdateThread(QString fileName, QString portName);

    private:
        QString m_fileName;
        QString m_serialPortName;

        void run() override;

    signals:
        void finished(FirmwareUpdateThreadStatus status, const QString& message = "");
        void progressMessage(const QString& message);
    };


} // threads

#endif //CANSHARK_FIRMWAREUPDATETHREAD_H
