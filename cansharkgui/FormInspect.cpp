#include "FormInspect.h"
#include "ui_FormInspect.h"

namespace dd::forms {
    FormInspect::FormInspect(const QByteArray& byteArray, QWidget *parent) :
            QWidget(parent), ui(new Ui::FormInspect) {
        ui->setupUi(this);

        // Connect the close button to the close function
        connect(ui->closeButton, &QPushButton::pressed, this, &FormInspect::close);

        this->m_document = QHexDocument::fromMemory<QMemoryBuffer>(byteArray);
        this->m_hexview = new QHexView();
        this->m_hexview->setDocument(this->m_document);
        this->ui->mainLayout->addWidget(this->m_hexview);
    }

    FormInspect::~FormInspect() {
        delete ui;
        delete m_hexview;
        delete m_document;
    }
} // dd::forms
