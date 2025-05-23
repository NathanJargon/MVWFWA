#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QString>
#include <QElapsedTimer>
#include <fstream>
#include <sstream>
#include <queue>
#include <limits>
#include <algorithm>
#include <QTableWidgetItem>

const int INF = std::numeric_limits<int>::max();

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    selectedFile("") {
    ui->setupUi(this);

    connect(ui->runButton, &QPushButton::clicked, this, &MainWindow::onRunButtonClicked);
    connect(ui->fileButton, &QPushButton::clicked, this, &MainWindow::onFileButtonClicked);
    connect(ui->tableWidget1, &QTableWidget::itemChanged, this, &MainWindow::onTable1ItemChanged); // Add this line
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::onFileButtonClicked() {
    QString fileName = QFileDialog::getOpenFileName(this, "Select Dataset File", "", "CSV Files (*.csv);;All Files (*)");
    if (!fileName.isEmpty()) {
        selectedFile = fileName;

        QFileInfo fileInfo(fileName);
        ui->fileButton->setText(fileInfo.fileName());

        QMessageBox::information(this, "Dataset File Selected",
                                 "File selected: " + fileName + "\n\n"
                                 "Ensure the file is formatted as follows:\n"
                                 "Course,Credits,Prerequisites\n"
                                 "Example:\n"
                                 "MMW,3,\n"
                                 "PurCom,3,\n"
                                 "ArtApp,3,PurCom\n");
    }
}

// Add this member to your MainWindow class:
// std::vector<std::string> courseOrder;

void MainWindow::onTable1ItemChanged(QTableWidgetItem* item) {
    if (item->column() == 3) { // "Taken" column
        QString courseName = ui->tableWidget1->item(item->row(), 0)->text();
        if (item->checkState() == Qt::Checked) {
            takenCourses.insert(courseName.toStdString());
        } else {
            takenCourses.erase(courseName.toStdString());
        }
    }
}

std::map<std::string, Course> MainWindow::readCurriculum(const std::string& filename) {
    std::map<std::string, Course> courses;
    courseOrder.clear(); // <-- clear previous order
    std::ifstream file(filename);
    std::string line;

    if (!file.is_open()) {
        QMessageBox::critical(this, "File Error", "Unable to open the file: " + QString::fromStdString(filename));
        return courses;
    }

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        if (line[0] == '\xEF' && line[1] == '\xBB' && line[2] == '\xBF') {
            line = line.substr(3);
        }

        std::stringstream ss(line);
        std::string name, creditsStr, prereqStr;

        std::getline(ss, name, ',');
        std::getline(ss, creditsStr, ',');
        std::getline(ss, prereqStr, ',');

        name.erase(name.find_last_not_of(" \t\n\r") + 1);
        creditsStr.erase(creditsStr.find_last_not_of(" \t\n\r") + 1);
        prereqStr.erase(prereqStr.find_last_not_of(" \t\n\r") + 1);

        int credits = 0;
        try {
            credits = std::stoi(creditsStr);
        } catch (const std::invalid_argument&) {
            QMessageBox::warning(this, "Invalid Data",
                                 "Invalid credits value for course: " + QString::fromStdString(name) +
                                 "\nDefaulting credits to 0.");
        }

        Course course;
        course.name = name;
        course.credits = credits;

        std::stringstream prereqStream(prereqStr);
        std::string prereq;
        while (std::getline(prereqStream, prereq, ';')) {
            if (!prereq.empty()) {
                course.prerequisites.push_back(prereq);
            }
        }

        courses[name] = course;
        courseOrder.push_back(name); // <-- preserve order
    }

    return courses;
}

// Helper: Try all combinations of available courses to get as close as possible to 24 or 23 credits
std::vector<std::vector<std::string>> MainWindow::groupCoursesBySemester(const std::map<std::string, Course>& courses) {
    const int maxCreditsPerSemester = 24; // or 23 if you prefer
    std::map<std::string, int> inDegree;
    std::map<std::string, std::vector<std::string>> graph;
    std::vector<std::vector<std::string>> semesters;

    for (const auto& [course, data] : courses) {
        inDegree[course] = 0;
    }
    for (const auto& [course, data] : courses) {
        for (const auto& prereq : data.prerequisites) {
            graph[prereq].push_back(course);
            inDegree[course]++;
        }
    }

    std::set<std::string> completed;
    while (completed.size() < courses.size()) {
        // Find all available courses in CSV order
        std::vector<std::string> available;
        for (const auto& name : courseOrder) {
            if (inDegree[name] == 0 && completed.find(name) == completed.end() && takenCourses.find(name) == takenCourses.end()) {
                available.push_back(name);
            }
        }

        // Greedily select courses up to the credit limit
        std::vector<std::string> semester;
        int creditSum = 0;
        for (const auto& name : available) {
            int c = courses.at(name).credits;
            if (creditSum + c <= maxCreditsPerSemester) {
                semester.push_back(name);
                creditSum += c;
            }
        }

        if (semester.empty()) {
            // If no course fits, just take the first available (to avoid infinite loop)
            if (!available.empty()) {
                semester.push_back(available[0]);
            } else {
                break;
            }
        }

        semesters.push_back(semester);

        // Mark as completed and update inDegree
        for (const auto& course : semester) {
            completed.insert(course);
            for (const auto& neighbor : graph[course]) {
                inDegree[neighbor]--;
            }
        }
    }

    return semesters;
}

std::vector<std::vector<std::string>> MainWindow::groupCoursesBySemesterShortest(const std::map<std::string, Course>& courses) {
    std::map<std::string, int> inDegree;
    std::map<std::string, std::vector<std::string>> graph;
    std::vector<std::vector<std::string>> semesters;

    for (const auto& [course, data] : courses) {
        inDegree[course] = 0;
    }
    for (const auto& [course, data] : courses) {
        for (const auto& prereq : data.prerequisites) {
            graph[prereq].push_back(course);
            inDegree[course]++;
        }
    }

    std::set<std::string> completed;
    while (completed.size() < courses.size()) {
        std::vector<std::string> available;
        for (const auto& name : courseOrder) {
            if (inDegree[name] == 0 && completed.find(name) == completed.end()) {
                available.push_back(name);
            }
        }

        if (available.empty()) break;

        semesters.push_back(available);

        for (const auto& course : available) {
            completed.insert(course);
            for (const auto& neighbor : graph[course]) {
                inDegree[neighbor]--;
            }
        }
    }

    return semesters;
}

void MainWindow::onRunButtonClicked() {
    if (selectedFile.isEmpty()) {
        QMessageBox::warning(this, "No File Selected", "Please select a dataset file using the 'Dataset File' button.");
        return;
    }

    QElapsedTimer timer;
    timer.start();

    auto courses = readCurriculum(selectedFile.toStdString());

    // Switch between the two by commenting/uncommenting:
    auto semesters = groupCoursesBySemester(courses); // Credit-limited
    // auto semesters = groupCoursesBySemesterShortest(courses); // Shortest possible

    qint64 elapsedTime = timer.elapsed();

    populateTable1(courses);
    populateTable2(semesters, courses);

    QString order = "Execution Time: " + QString::number(elapsedTime) + " ms";
    ui->orderLabel->setText(order);
}

void MainWindow::populateTable1(const std::map<std::string, Course>& courses) {
    ui->tableWidget1->clear();

    ui->tableWidget1->setRowCount(courses.size());
    ui->tableWidget1->setColumnCount(4);
    ui->tableWidget1->setHorizontalHeaderLabels({"Course", "Credits", "Prerequisites", "Taken"});

    int row = 0;
    for (const auto& [courseName, course] : courses) {
        ui->tableWidget1->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(course.name)));
        ui->tableWidget1->setItem(row, 1, new QTableWidgetItem(QString::number(course.credits)));

        QString prerequisites;
        for (const auto& prereq : course.prerequisites) {
            prerequisites += QString::fromStdString(prereq) + "; ";
        }
        if (!prerequisites.isEmpty()) {
            prerequisites.chop(2);
        }
        ui->tableWidget1->setItem(row, 2, new QTableWidgetItem(prerequisites));

        // Set checkbox state based on takenCourses
        QTableWidgetItem* takenItem = new QTableWidgetItem();
        if (takenCourses.count(course.name)) {
            takenItem->setCheckState(Qt::Checked);
        } else {
            takenItem->setCheckState(Qt::Unchecked);
        }
        ui->tableWidget1->setItem(row, 3, takenItem);

        row++;
    }

    ui->tableWidget1->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget1->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableWidget1->resizeRowsToContents();
}

void MainWindow::populateTable2(const std::vector<std::vector<std::string>>& semesters, const std::map<std::string, Course>& courses) {
    ui->tableWidget2->clear();

    // Set the number of rows and columns based on semesters and the maximum number of courses in a semester
    ui->tableWidget2->setRowCount(semesters.size());
    int maxCoursesInSemester = 0;
    for (const auto& semester : semesters) {
        maxCoursesInSemester = std::max(maxCoursesInSemester, static_cast<int>(semester.size()));
    }
    ui->tableWidget2->setColumnCount(maxCoursesInSemester + 2); // +1 for "Total Credits", +1 for "Semester Label"

    // Set headers
    QStringList headers;
    headers << "Semester";
    for (int i = 1; i <= maxCoursesInSemester; ++i) {
        headers << "Course " + QString::number(i);
    }
    headers << "Total Credits";
    ui->tableWidget2->setHorizontalHeaderLabels(headers);

    // Helper for ordinal numbers
    auto ordinal = [](int n) {
        if (n % 10 == 1 && n % 100 != 11) return QString::number(n) + "st";
        if (n % 10 == 2 && n % 100 != 12) return QString::number(n) + "nd";
        if (n % 10 == 3 && n % 100 != 13) return QString::number(n) + "rd";
        return QString::number(n) + "th";
    };

    int year = 1, term = 1, summerCount = 0;
    for (int i = 0; i < semesters.size(); ++i) {
        int totalCredits = 0;
        QString label;

        // Check for summer (CS331 only)
        if (semesters[i].size() == 1 && semesters[i][0] == "CS331") {
            summerCount++;
            label = QString::number(year + summerCount - 1) + "rd Year - Summer";
        } else {
            label = ordinal(year) + " Year - " + ordinal(term) + " Semester";
            term++;
            if (term > 2) {
                year++;
                term = 1;
            }
        }

        ui->tableWidget2->setItem(i, 0, new QTableWidgetItem(label));

        for (int j = 0; j < semesters[i].size(); ++j) {
            const auto& courseName = semesters[i][j];
            ui->tableWidget2->setItem(i, j + 1, new QTableWidgetItem(QString::fromStdString(courseName)));
            totalCredits += courses.at(courseName).credits;
        }

        // Add total credits to the last column
        ui->tableWidget2->setItem(i, maxCoursesInSemester + 1, new QTableWidgetItem(QString::number(totalCredits)));
    }

    // Stretch columns to fit the entire width
    ui->tableWidget2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget2->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget2->resizeRowsToContents();
}