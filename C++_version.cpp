#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QInputDialog>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>

class DeliverablesTracker : public QWidget {
    Q_OBJECT

public:
    DeliverablesTracker(QWidget *parent = nullptr) : QWidget(parent) {
        setupUi();
        loadDeliverables();
        renderDeliverables();

        connect(addButton, &QPushButton::clicked, this, &DeliverablesTracker::addDeliverable);
        connect(exportButton, &QPushButton::clicked, this, &DeliverablesTracker::exportData);
    }

private slots:
    void addDeliverable() {
        QString name = QInputDialog::getText(this, "Add Deliverable", "Enter the deliverable name:");
        QString status = QInputDialog::getItem(this, "Add Deliverable", "Select the deliverable status:",
                                              QStringList() << "implemented" << "partiallyImplemented" << "notImplemented");
        QString note = QInputDialog::getText(this, "Add Deliverable", "Enter the deliverable note:");

        deliverables.append({name, status, note});
        renderDeliverables();
        storeData();
    }

    void exportData() {
        QString fileName = QFileDialog::getSaveFileName(this, "Export Data", QDir::homePath(), "JSON Files (*.json)");

        if (!fileName.isEmpty()) {
            QFile file(fileName);
            if (file.open(QIODevice::WriteOnly)) {
                QJsonArray jsonArray;
                for (const auto &deliverable : deliverables) {
                    QJsonObject jsonObject;
                    jsonObject["name"] = deliverable.name;
                    jsonObject["status"] = deliverable.status;
                    jsonObject["note"] = deliverable.note;
                    jsonArray.append(jsonObject);
                }

                QJsonDocument jsonDoc(jsonArray);
                file.write(jsonDoc.toJson());
                file.close();
            } else {
                QMessageBox::critical(this, "Error", "Unable to open the file for writing.");
            }
        }
    }

private:
    struct Deliverable {
        QString name;
        QString status;
        QString note;
    };

    QVBoxLayout *layout;
    QLabel *titleLabel;
    QListWidget *deliverablesList;
    QPushButton *addButton;
    QPushButton *exportButton;

    QVector<Deliverable> deliverables;

    void setupUi() {
        layout = new QVBoxLayout(this);
        titleLabel = new QLabel("Admin Panel - Deliverables Tracker", this);
        deliverablesList = new QListWidget(this);
        addButton = new QPushButton("Add Deliverable", this);
        exportButton = new QPushButton("Export Data", this);

        layout->addWidget(titleLabel);
        layout->addWidget(deliverablesList);
        layout->addWidget(addButton);
        layout->addWidget(exportButton);

        setLayout(layout);
    }

    void renderDeliverables() {
        deliverablesList->clear();

        for (const auto &deliverable : deliverables) {
            QString statusStyle;
            if (deliverable.status == "implemented")
                statusStyle = "color: green; background-color: lightgreen;";
            else if (deliverable.status == "partiallyImplemented")
                statusStyle = "color: yellow; background-color: lightyellow;";
            else if (deliverable.status == "notImplemented")
                statusStyle = "color: red; background-color: lightcoral;";

            QString itemText = QString("<span style=\"%1\">%2 - %3</span>").arg(statusStyle).arg(deliverable.name).arg(deliverable.note);
            deliverablesList->addItem(itemText);
        }
    }

    void storeData() {
        QFile file("deliverables.json");
        if (file.open(QIODevice::WriteOnly)) {
            QJsonArray jsonArray;
            for (const auto &deliverable : deliverables) {
                QJsonObject jsonObject;
                jsonObject["name"] = deliverable.name;
                jsonObject["status"] = deliverable.status;
                jsonObject["note"] = deliverable.note;
                jsonArray.append(jsonObject);
            }

            QJsonDocument jsonDoc(jsonArray);
            file.write(jsonDoc.toJson());
            file.close();
        } else {
            QMessageBox::critical(this, "Error", "Unable to open the file for writing.");
        }
    }

    void loadDeliverables() {
        QFile file("deliverables.json");
        if (file.open(QIODevice::ReadOnly)) {
            QJsonDocument jsonDoc = QJsonDocument::fromJson(file.readAll());
            file.close();

            if (jsonDoc.isArray()) {
                QJsonArray jsonArray = jsonDoc.array();
                for (const auto &jsonValue : jsonArray) {
                    if (jsonValue.isObject()) {
                        QJsonObject jsonObject = jsonValue.toObject();
                        Deliverable deliverable;
                        deliverable.name = jsonObject["name"].toString();
                        deliverable.status = jsonObject["status"].toString();
                        deliverable.note = jsonObject["note"].toString();
                        deliverables.append(deliverable);
                    }
                }
            }
        }
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    DeliverablesTracker deliverablesTracker;
    deliverablesTracker.show();
    return app.exec();
}

#include "main.moc"
