#ifndef CANSHARK_CANSHARK_H
#define CANSHARK_CANSHARK_H

#include <QObject>
#include <QSerialPort>
#include "../DataParserThread.h"

namespace dd::libcanshark::drivers {

    class CanShark : public QObject {
    Q_OBJECT

    public:
        explicit CanShark(libcanshark::threads::DataParserThread* dataThread, QObject *parent = nullptr);

        ~CanShark() override;

        virtual bool openConnection(QString const &portName) = 0;

        virtual bool closeConnection() = 0;

        virtual bool startRecording(size_t max_messages) = 0;

        virtual bool stopRecording() = 0;

        virtual bool updateFirmware(QString const &firmwareUpdateFileName) = 0;

        QList<std::tuple<QString, QString>>& getAvailablePorts();

    protected:
        libcanshark::threads::DataParserThread *m_dataThread = nullptr;

        QSerialPort *m_serial = nullptr;
        bool b_recording = false;
        size_t st_max_messages = 0;
        size_t st_recorded_message_count = 0;

        bool b_ready = false;

    protected slots:
        void readData();

        void serialError(QSerialPort::SerialPortError error);

    signals:

        void statusMessage(QString const &message);

        void errorMessage(QString const &message);

        void serialDataReceived(const QString &data);
    };

} // drivers

#endif //CANSHARK_CANSHARK_H