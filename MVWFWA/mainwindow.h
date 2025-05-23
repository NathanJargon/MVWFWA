#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <map>
#include <vector>
#include <QTableWidgetItem>
#include <string>

struct Course {
    std::string name;
    int credits;
    std::vector<std::string> prerequisites;
    std::set<std::string> takenCourses;
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onFileButtonClicked();
    void onRunButtonClicked();
    void onTable1ItemChanged(QTableWidgetItem* item); // Add this line
    void onSaveButtonClicked(); // Add this line

private:
    Ui::MainWindow *ui;
    QString selectedFile;

    std::vector<std::string> courseOrder;
    std::set<std::string> takenCourses;
    std::map<std::string, Course> readCurriculum(const std::string& filename);
    void saveCurriculumWithTaken(const std::string& filename, const std::map<std::string, Course>& courses); // <-- Add this line!
    std::vector<std::vector<std::string>> groupCoursesBySemester(const std::map<std::string, Course>& courses);
    std::vector<std::vector<std::string>> groupCoursesBySemesterShortest(const std::map<std::string, Course>& courses);
    void populateTable1(const std::map<std::string, Course>& courses);
    void populateTable2(const std::vector<std::vector<std::string>>& semesters, const std::map<std::string, Course>& courses);
    std::vector<std::vector<int>> creditAwareFloydWarshall(const std::map<std::string, Course>& courses);

};

#endif // MAINWINDOW_H