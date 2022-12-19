#ifndef CANSHARK_RECORDINGTHREAD_H
#define CANSHARK_RECORDINGTHREAD_H

#include <QThread>
#include <QMutex>
#include <QSerialPort>

#include "../RecordItem.h"

namespace dd::libcanshark::threads {
    class RecordingThread : public QThread {
        Q_OBJECT

    public:
        explicit RecordingThread(QObject* parent = nullptr);

        bool openConnection(const QString& serialPortName);
        bool closeConnection();
        bool startRecording(size_t st_maxMessages);
        bool stopRecording();

    protected:
        void run() Q_DECL_OVERRIDE;

    private:
        QMutex m_mutex;
        QSerialPort m_serialPort;
        QString m_serialPortName;

        bool b_appendToPacket = false;

        QString m_packetHexString;
        QList<QString> m_packetHexStringsList;

        QList<data::RecordItem> m_canPackets;

        bool b_recording = false;
        size_t st_max_messages = 0;
        size_t st_recorded_message_count = 0;

    private slots:
        void serialDataReady();

    signals:
        void dataReady(QList<dd::libcanshark::data::RecordItem>&);
        void progressStatus(const QString& message, int progress = 0);
        void finished(const QString& message);
    };

} // threads

#endif //CANSHARK_RECORDINGTHREAD_H
