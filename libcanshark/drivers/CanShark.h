#ifndef CANSHARK_CANSHARK_H
#define CANSHARK_CANSHARK_H

#include <QObject>
#include <QSerialPort>
#include "../threads/RecordingThread.h"
#include "../threads/FirmwareUpdateThread.h"

namespace dd::libcanshark::drivers {

    class CanShark : public QObject {
    Q_OBJECT

    public:
        explicit CanShark(QObject *parent = nullptr);

        ~CanShark() override;

        virtual bool startRecording(const QString& serialPortName, size_t max_messages) = 0;

        virtual bool stopRecording() = 0;

        virtual bool updateFirmware(QString const &firmwareUpdateFileName, const QString& selectedDevicePortName) = 0;

        static QList<std::tuple<QString, QString>> getAvailablePorts();

    protected:
        libcanshark::threads::RecordingThread* ptr_recordingThread = nullptr;
        threads::FirmwareUpdateThread* ptr_firmwareUpdateThread = nullptr;

        QString m_serialPortName;

    signals:

        void statusMessage(QString const &message);

        void errorMessage(QString const &message);

        void dataReady(QList<data::RecordItem> &data);

        void updateComplete(threads::FirmwareUpdateThreadStatus status);
    };

} // drivers

#endif //CANSHARK_CANSHARK_H
