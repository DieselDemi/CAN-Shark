#ifndef CANSHARK_DATAPARSERTHREAD_H
#define CANSHARK_DATAPARSERTHREAD_H

#include <QThread>
#include <QMutex>
#include "../RecordItem.h"

namespace dd::libcanshark::threads {
    class DataParserThread : public QThread {
        Q_OBJECT

    public:
        explicit DataParserThread(QObject* parent = nullptr);
        ~DataParserThread() override;

        void init();

        void startRecording(const QString& serialPort);
        void stopRecording();

    protected:
        void run() Q_DECL_OVERRIDE;

    private:
        QMutex mutex;
        bool running = false;

        bool bAppendToPacket = false;
        QString packetHexString;
        QList<QString> packetHexStrings;
        QList<data::RecordItem> packets;

    public slots:
        void serialDataReceived(const QString& data);

    signals:
        void dataReady(QList<dd::libcanshark::data::RecordItem>&);
    };

} // threads

#endif //CANSHARK_DATAPARSERTHREAD_H
