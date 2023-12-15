#include <QtWidgets>
#include <QFile>
#include <QTextStream>

class Deliverable {
public:
    QString name;
    QString status;
    QString notes;
};

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr)
        : QMainWindow(parent), addButton(this), updateButton(this), saveButton(this), deliverableTable(this) {
        setWindowTitle("Deliverables Manager");

        setupUI();
        connectActions();
        loadFromFile(); // Load existing data when the application starts
    }

private slots:
    void addDeliverable() {
        // Get user input for the new deliverable
        QString name = QInputDialog::getText(this, "Add Deliverable", "Enter deliverable name:");
        if (name.isEmpty()) return;

        // Set the default status to "Not Implemented"
        Deliverable newDeliverable{ name, "Not Implemented", "" };

        // Add the new deliverable to the list and update the table
        deliverables.append(newDeliverable);
        updateTable();
    }

    void updateDeliverable() {
        // Get the selected row
        int row = deliverableTable.currentRow();
        if (row < 0 || row >= deliverables.size()) return;

        // Get user input for the updated deliverable
        QString name = QInputDialog::getText(this, "Update Deliverable", "Enter updated deliverable name:", QLineEdit::Normal, deliverables[row].name);
        if (name.isEmpty()) return;

        // Update the deliverable
        deliverables[row].name = name;

        // Update the table
        updateTable();
    }

    void saveToFile() {
        // Get the file path for saving
        QString filePath = QFileDialog::getSaveFileName(this, "Save File", "", "CSV Files (*.csv)");
        if (filePath.isEmpty()) return;

        // Open the file for writing
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream stream(&file);

            // Write header
            stream << "Name,Status,Notes\n";

            // Write deliverables
            for (const Deliverable& deliverable : deliverables) {
                stream << deliverable.name << "," << deliverable.status << "," << deliverable.notes << "\n";
            }

            file.close();
        }
    }

private:
    void setupUI() {
        // Create UI components
        addButton.setText("Add");
        updateButton.setText("Update");
        saveButton.setText("Save");

        // Create table headers
        deliverableTable.setColumnCount(3);
        QStringList headers = { "Name", "Status", "Notes" };
        deliverableTable.setHorizontalHeaderLabels(headers);

        // Set up layout
        QVBoxLayout *mainLayout = new QVBoxLayout;
        mainLayout->addWidget(&addButton);
        mainLayout->addWidget(&updateButton);
        mainLayout->addWidget(&saveButton);
        mainLayout->addWidget(&deliverableTable);

        QWidget *centralWidget = new QWidget(this);
        centralWidget->setLayout(mainLayout);
        setCentralWidget(centralWidget);
    }

    void connectActions() {
        // Connect button clicks to slots
        connect(deliverableTable.selectionModel(), &QItemSelectionModel::selectionChanged, [this]() {
            // Enable the "Update" button only when a row is selected
            updateButton.setEnabled(!deliverableTable.selectedItems().isEmpty());
        });

        connect(&addButton, &QPushButton::clicked, this, &MainWindow::addDeliverable);
        connect(&updateButton, &QPushButton::clicked, this, &MainWindow::updateDeliverable);
        connect(&saveButton, &QPushButton::clicked, this, &MainWindow::saveToFile);
    }

    void updateTable() {
        // Update the table with the current deliverables data
        deliverableTable.clearContents();
        deliverableTable.setRowCount(deliverables.size());

        for (int row = 0; row < deliverables.size(); ++row) {
            const Deliverable& deliverable = deliverables[row];

            QTableWidgetItem *nameItem = new QTableWidgetItem(deliverable.name);
            QTableWidgetItem *statusItem = new QTableWidgetItem(deliverable.status);
            QTableWidgetItem *notesItem = new QTableWidgetItem(deliverable.notes);

            deliverableTable.setItem(row, 0, nameItem);
            deliverableTable.setItem(row, 1, statusItem);
            deliverableTable.setItem(row, 2, notesItem);
        }
    }

    void loadFromFile() {
        // Load deliverables from a file (if exists)
        QString filePath = QFileDialog::getOpenFileName(this, "Open File", "", "CSV Files (*.csv)");

        if (filePath.isEmpty()) return;

        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream stream(&file);
            // Skip the header line
            stream.readLine();

            while (!stream.atEnd()) {
                QString line = stream.readLine();
                QStringList parts = line.split(',');

                if (parts.size() == 3) {
                    Deliverable deliverable{
                        parts[0].trimmed(),
                        parts[1].trimmed(),
                        parts[2].trimmed()
                    };

                    deliverables.append(deliverable);
                }
            }

            file.close();
            updateTable();
        }
    }

    QList<Deliverable> deliverables;
    QPushButton addButton;
    QPushButton updateButton;
    QPushButton saveButton;
    QTableWidget deliverableTable;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}

#include "main.moc"
