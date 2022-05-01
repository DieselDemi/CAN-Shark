#include "LibCanShark.h"

LibCanShark::LibCanShark(QObject *parent) : QThread(parent) {

}

LibCanShark::~LibCanShark() {

}

void LibCanShark::connect(const QString &portName, int waitTimeout, const QString &response) {

}

void LibCanShark::run() {
    QThread::run();
}

void LibCanShark::request(const QString &s) {

}

void LibCanShark::error(const QString &s) {

}

void LibCanShark::timeout(const QString &s) {

}
