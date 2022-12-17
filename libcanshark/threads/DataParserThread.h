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

        void stop();

    protected:
        void run() Q_DECL_OVERRIDE;

    private:
        QMutex mutex;
        bool running = false;

        bool bAppendToPacket = false;
        QString packetHexString;
        QList<QString> packetHexStrings;
        QList<data::RecordItem> packets;

        template<typename T>
        std::vector<T> hex2bytes(const std::string& s)
        {
            constexpr size_t width = sizeof(T) * 2;
            std::vector<T> v;
            v.reserve((s.size() + width - 1) / width);
            for (auto it = s.crbegin(); it < s.crend(); it += width)
            {
                try {
                    auto begin = std::min(s.crend(), it + width).base();
                    auto end = it.base();
                    std::string slice(begin, end);
                    T value = std::stoul(slice, 0, 16);
                    v.push_back(value);
                } catch(std::exception& e) {
                    return {};
                }
            }
            return v;
        }


    public slots:
        void serialDataReceived(const QString& data);

    signals:
        void dataReady(QList<dd::libcanshark::data::RecordItem>&);
    };

} // threads

#endif //CANSHARK_DATAPARSERTHREAD_H
