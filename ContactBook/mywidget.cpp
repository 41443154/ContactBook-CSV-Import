
#include "mywidget.h"
#include "ui_mywidget.h"

#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QStringList>
#include <QStringConverter>
#include <QTableWidgetItem>
#include <QMessageBox>

// Convert a value to CSV format
static QString csvEscape(QString value)
{
    if (value.contains('"') ||
        value.contains(',') ||
        value.contains('\n') ||
        value.contains('\r')) {

        value.replace("\"", "\"\"");
        value = "\"" + value + "\"";
    }

    return value;
}

// Read fields from one CSV line
static QStringList parseCsvLine(const QString &line)
{
    QStringList result;
    QString field;
    bool inQuotes = false;

    for (int i = 0; i < line.size(); i++) {

        QChar ch = line.at(i);

        if (ch == '"') {

            if (inQuotes &&
                i + 1 < line.size() &&
                line.at(i + 1) == '"') {

                field += '"';
                i++;
            }
            else {
                inQuotes = !inQuotes;
            }
        }
        else if (ch == ',' && !inQuotes) {

            result << field;
            field.clear();
        }
        else {
            field += ch;
        }
    }

    result << field;
    return result;
}

// Constructor
MyWidget::MyWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MyWidget)
{
    ui->setupUi(this);

    ui->tableWidget->setColumnCount(4);

    QStringList headers;

    headers << QStringLiteral("\u5B78\u865F")
            << QStringLiteral("\u73ED\u7D1A")
            << QStringLiteral("\u59D3\u540D")
            << QStringLiteral("\u96FB\u8A71");

    ui->tableWidget->setHorizontalHeaderLabels(headers);

    // Empty table when application starts
    ui->tableWidget->setRowCount(0);
}

// Destructor
MyWidget::~MyWidget()
{
    delete ui;
}

// Button 1: Add contact
void MyWidget::on_pushButton_clicked()
{
    QString id = ui->lineEdit->text();
    QString studentClass = ui->lineEdit_2->text();
    QString name = ui->lineEdit_3->text();
    QString phone = ui->lineEdit_4->text();

    if (id.trimmed().isEmpty() ||
        name.trimmed().isEmpty()) {

        QMessageBox::warning(
            this,
            "Warning",
            "Please enter student ID and name."
            );

        return;
    }

    int row = ui->tableWidget->rowCount();

    ui->tableWidget->insertRow(row);

    ui->tableWidget->setItem(
        row, 0, new QTableWidgetItem(id));

    ui->tableWidget->setItem(
        row, 1, new QTableWidgetItem(studentClass));

    ui->tableWidget->setItem(
        row, 2, new QTableWidgetItem(name));

    ui->tableWidget->setItem(
        row, 3, new QTableWidgetItem(phone));

    // Clear input boxes
    ui->lineEdit->clear();
    ui->lineEdit_2->clear();
    ui->lineEdit_3->clear();
    ui->lineEdit_4->clear();
}

// Button 2: Export CSV
void MyWidget::on_pushButton_2_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(
        this,
        "Export CSV",
        "contacts.csv",
        "CSV Files (*.csv)"
        );

    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);

    if (!file.open(QIODevice::WriteOnly |
                   QIODevice::Text)) {

        QMessageBox::warning(
            this,
            "Error",
            "Cannot create CSV file."
            );

        return;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);

    out << "ID,Class,Name,Phone\n";

    int totalRows = ui->tableWidget->rowCount();

    for (int row = 0; row < totalRows; row++) {

        QStringList data;

        for (int col = 0; col < 4; col++) {

            QTableWidgetItem *item =
                ui->tableWidget->item(row, col);

            QString value;

            if (item != nullptr) {
                value = item->text();
            }

            data << csvEscape(value);
        }

        out << data.join(",") << "\n";
    }

    out.flush();
    file.close();

    QMessageBox::information(
        this,
        "Success",
        "CSV exported successfully!"
        );
}

// Button 3: Import CSV
void MyWidget::on_pushButton_3_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "Import CSV",
        QString(),
        "CSV Files (*.csv)"
        );

    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly |
                   QIODevice::Text)) {

        QMessageBox::warning(
            this,
            "Error",
            "Cannot open CSV file."
            );

        return;
    }

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);

    QList<QStringList> importedRows;

    bool firstLine = true;

    while (!in.atEnd()) {

        QString line = in.readLine();

        if (line.trimmed().isEmpty()) {
            continue;
        }

        if (firstLine &&
            line.startsWith(QChar(0xFEFF))) {
            line.remove(0, 1);
        }

        QStringList data = parseCsvLine(line);

        // Skip CSV header
        if (firstLine) {

            firstLine = false;

            if (data.size() == 4 &&
                (data.at(0) == "ID" ||
                 data.at(0) ==
                     QStringLiteral("\u5B78\u865F"))) {

                continue;
            }
        }

        if (data.size() != 4) {

            QMessageBox::warning(
                this,
                "Error",
                "CSV must contain exactly 4 columns."
                );

            return;
        }

        importedRows.append(data);
    }

    file.close();

    // Clear old data
    ui->tableWidget->setRowCount(0);

    // Insert imported contacts
    for (const QStringList &data : importedRows) {

        int row = ui->tableWidget->rowCount();

        ui->tableWidget->insertRow(row);

        for (int col = 0; col < 4; col++) {

            QTableWidgetItem *item =
                new QTableWidgetItem(data.at(col));

            ui->tableWidget->setItem(
                row,
                col,
                item
                );
        }
    }

    QMessageBox::information(
        this,
        "Success",
        "CSV imported successfully!"
        );
}
