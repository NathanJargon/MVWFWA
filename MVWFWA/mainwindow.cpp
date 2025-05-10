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

const int INF = std::numeric_limits<int>::max();

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    selectedFile("") {
    ui->setupUi(this);

    // Connect the buttons to their slots
    connect(ui->runButton, &QPushButton::clicked, this, &MainWindow::onRunButtonClicked);
    connect(ui->fileButton, &QPushButton::clicked, this, &MainWindow::onFileButtonClicked);
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

std::map<std::string, Course> MainWindow::readCurriculum(const std::string& filename) {
    std::map<std::string, Course> courses;
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
    }

    return courses;
}

std::vector<std::vector<std::string>> MainWindow::groupCoursesBySemester(const std::map<std::string, Course>& courses) {
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

    std::queue<std::string> readyQueue;
    for (const auto& [course, degree] : inDegree) {
        if (degree == 0) {
            readyQueue.push(course);
        }
    }

    while (!readyQueue.empty()) {
        std::vector<std::string> semester;
        int semesterCredits = 0;

        std::queue<std::string> nextQueue;
        while (!readyQueue.empty()) {
            std::string course = readyQueue.front();
            readyQueue.pop();

            int courseCredits = courses.at(course).credits;
            if (semesterCredits + courseCredits <= 24) {
                semester.push_back(course);
                semesterCredits += courseCredits;

                for (const auto& neighbor : graph[course]) {
                    inDegree[neighbor]--;
                    if (inDegree[neighbor] == 0) {
                        nextQueue.push(neighbor);
                    }
                }
            } else {
                nextQueue.push(course);
            }
        }

        semesters.push_back(semester);
        readyQueue = nextQueue;
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
    auto semesters = groupCoursesBySemester(courses);

    qint64 elapsedTime = timer.elapsed();

    // Populate both tables
    populateTable1(courses);
    populateTable2(semesters, courses);

    QString order = "Execution Time: " + QString::number(elapsedTime) + " ms";
    ui->orderLabel->setText(order);
}

void MainWindow::populateTable1(const std::map<std::string, Course>& courses) {
    ui->tableWidget1->clear();

    // Set the number of rows and columns
    ui->tableWidget1->setRowCount(3); // Rows: Course, Credits, Prerequisites
    ui->tableWidget1->setColumnCount(courses.size()); // One column per course

    // Set headers for columns (course names)
    QStringList headers;
    for (const auto& [courseName, course] : courses) {
        headers << QString::fromStdString(courseName);
    }
    ui->tableWidget1->setHorizontalHeaderLabels(headers);

    // Set headers for rows
    ui->tableWidget1->setVerticalHeaderLabels({"Course", "Credits", "Prerequisites"});

    // Populate the table
    int col = 0;
    for (const auto& [courseName, course] : courses) {
        // Row 1: Course Name
        ui->tableWidget1->setItem(0, col, new QTableWidgetItem(QString::fromStdString(course.name)));

        // Row 2: Credits
        ui->tableWidget1->setItem(1, col, new QTableWidgetItem(QString::number(course.credits)));

        // Row 3: Prerequisites
        QString prerequisites;
        for (const auto& prereq : course.prerequisites) {
            prerequisites += QString::fromStdString(prereq) + "; ";
        }
        if (!prerequisites.isEmpty()) {
            prerequisites.chop(2); // Remove trailing "; "
        }
        ui->tableWidget1->setItem(2, col, new QTableWidgetItem(prerequisites));

        col++;
    }

    // Stretch columns to fit the entire width
    ui->tableWidget1->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Stretch rows to fit the entire height
    ui->tableWidget1->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Resize rows to fit content
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
    ui->tableWidget2->setColumnCount(maxCoursesInSemester + 1); // +1 for the "Credits" column

    // Set headers
    QStringList headers;
    headers << "Semester";
    for (int i = 1; i <= maxCoursesInSemester; ++i) {
        headers << "Course " + QString::number(i);
    }
    headers << "Total Credits";
    ui->tableWidget2->setHorizontalHeaderLabels(headers);

    // Populate the table with semester data
    for (int i = 0; i < semesters.size(); ++i) {
        int totalCredits = 0;
        ui->tableWidget2->setItem(i, 0, new QTableWidgetItem("Semester " + QString::number(i + 1)));

        for (int j = 0; j < semesters[i].size(); ++j) {
            const auto& courseName = semesters[i][j];
            ui->tableWidget2->setItem(i, j + 1, new QTableWidgetItem(QString::fromStdString(courseName)));

            // Calculate total credits for the semester
            totalCredits += courses.at(courseName).credits;
        }

        // Add total credits to the last column
        ui->tableWidget2->setItem(i, maxCoursesInSemester + 1, new QTableWidgetItem(QString::number(totalCredits)));
    }

    // Stretch columns to fit the entire width
    ui->tableWidget2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->tableWidget2->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Resize rows to fit content
    ui->tableWidget2->resizeRowsToContents();
}