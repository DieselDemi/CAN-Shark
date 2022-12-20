
#ifndef CANSHARK_HARDWAREPOLLINGTHREAD_H
#define CANSHARK_HARDWAREPOLLINGTHREAD_H

#include <QThread>

namespace dd::libcanshark::threads {

            class HardwarePollingThread : public QThread{
                Q_OBJECT
            protected:
                void run() override;

            private:
                QList<std::tuple<QString, QString>> currentPorts;

            signals:
                void hardwareChanged();
            };

        } // threads

#endif //CANSHARK_HARDWAREPOLLINGTHREAD_H
