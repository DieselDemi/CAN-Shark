#ifndef CANSHARK_FORMINSPECT_H
#define CANSHARK_FORMINSPECT_H

#include <QWidget>
#include "qhexview.h"
#include "document/buffer/qmemorybuffer.h"

namespace dd::forms {
    QT_BEGIN_NAMESPACE
    namespace Ui { class FormInspect; }
    QT_END_NAMESPACE

    class FormInspect : public QWidget {
    Q_OBJECT

    public:
        explicit FormInspect(const QByteArray& byteArray, QWidget *parent = nullptr);

        ~FormInspect() override;

    private:
        Ui::FormInspect *ui;

        QHexDocument* m_document = nullptr;
        QHexView* m_hexview = nullptr;
    };
} // dd::forms

#endif //CANSHARK_FORMINSPECT_H
