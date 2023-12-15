#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QInputDialog>

class DeliverablesManager : public QWidget {
    Q_OBJECT

public:
    DeliverablesManager(QWidget *parent = nullptr);

private slots:
    void addDeliverable();
    void updateDeliverable();
    void saveToFile();
    void importFromFile();
    void updateTable();
    void cellClicked(int row, int column);

private:
    QTableWidget *table;
    QList<QMap<QString, QString>> deliverables;
};

DeliverablesManager::DeliverablesManager(QWidget *parent)
    : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);

    QPushButton *addButton = new QPushButton("Add", this);
    QPushButton *updateButton = new QPushButton("Update", this);
    QPushButton *saveButton = new QPushButton("Save", this);

    table = new QTableWidget(this);
    table->setColumnCount(3);
    table->setHorizontalHeaderLabels({"Name", "Status", "Notes"});
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    connect(addButton, &QPushButton::clicked, this, &DeliverablesManager::addDeliverable);
    connect(updateButton, &QPushButton::clicked, this, &DeliverablesManager::updateDeliverable);
    connect(saveButton, &QPushButton::clicked, this, &DeliverablesManager::saveToFile);
    connect(table, &QTableWidget::cellClicked, this, &DeliverablesManager::cellClicked);

    layout->addWidget(addButton);
    layout->addWidget(updateButton);
    layout->addWidget(saveButton);
    layout->addWidget(table);

    // Load existing data (for demonstration purposes)
    deliverables.append({{"Name", "Task 1"}, {"Status", "Not Implemented"}, {"Notes", ""}});
    deliverables.append({{"Name", "Task 2"}, {"Status", "Implemented"}, {"Notes", "Completed on time"}});
    deliverables.append({{"Name", "Task 3"}, {"Status", "Partially Implemented"}, {"Notes", "In progress"}});

    updateTable();
}

void DeliverablesManager::addDeliverable() {
    QString name = QInputDialog::getText(this, "Enter deliverable name", "Name:");
    if (name.isEmpty())
        return;

    QMap<QString, QString> newDeliverable = {{"Name", name}, {"Status", "Not Implemented"}, {"Notes", ""}};
    deliverables.append(newDeliverable);
    updateTable();
}

void DeliverablesManager::updateDeliverable() {
    int row = table->currentRow();
    if (row < 0)
        return;

    QString name = QInputDialog::getText(this, "Enter updated deliverable name", "Name:", QLineEdit::Normal,
                                         deliverables[row]["Name"]);
    if (name.isEmpty())
        return;

    deliverables[row]["Name"] = name;
    updateTable();
}

void DeliverablesManager::saveToFile() {
    QString filePath = QFileDialog::getSaveFileName(this, "Save File", "", "CSV Files (*.csv)");
    if (filePath.isEmpty())
        return;

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        stream << "Name,Status,Notes\n";

        for (const auto &deliverable : deliverables) {
            stream << deliverable["Name"] << "," << deliverable["Status"] << "," << deliverable["Notes"] << "\n";
        }

        file.close();
    }
}

void DeliverablesManager::importFromFile() {
    QString filePath = QFileDialog::getOpenFileName(this, "Open File", "", "CSV Files (*.csv)");
    if (filePath.isEmpty())
        return;

    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        stream.readLine(); // Skip the header

        deliverables.clear();
        while (!stream.atEnd()) {
            QString line = stream.readLine();
            QStringList fields = line.split(',');

            if (fields.size() == 3) {
                deliverables.append({{"Name", fields[0].trimmed()},
                                     {"Status", fields[1].trimmed()},
                                     {"Notes", fields[2].trimmed()}});
            }
        }

        file.close();
        updateTable();
    }
}

void DeliverablesManager::updateTable() {
    table->setRowCount(deliverables.size());

    for (int i = 0; i < deliverables.size(); ++i) {
        const auto &deliverable = deliverables[i];
        table->setItem(i, 0, new QTableWidgetItem(deliverable["Name"]));
        table->setItem(i, 1, new QTableWidgetItem(deliverable["Status"]));
        table->setItem(i, 2, new QTableWidgetItem(deliverable["Notes"]));
    }
}

void DeliverablesManager::cellClicked(int row, int column) {
    Q_UNUSED(column);
    table->setCurrentCell(row, 0);
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    DeliverablesManager deliverablesManager;
    deliverablesManager.show();

    return app.exec();
}

#include "main.moc"
