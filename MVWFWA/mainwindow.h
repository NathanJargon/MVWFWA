#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <map>
#include <vector>
#include <string>

struct Course {
    std::string name;
    int credits;
    std::vector<std::string> prerequisites;
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

private:
    Ui::MainWindow *ui;
    QString selectedFile;

    std::vector<std::string> courseOrder;
    std::map<std::string, Course> readCurriculum(const std::string& filename);
    std::vector<std::vector<std::string>> groupCoursesBySemester(const std::map<std::string, Course>& courses);
    std::vector<std::vector<std::string>> groupCoursesBySemesterShortest(const std::map<std::string, Course>& courses); // <-- Add this line
    void populateTable1(const std::map<std::string, Course>& courses);
    void populateTable2(const std::vector<std::vector<std::string>>& semesters, const std::map<std::string, Course>& courses);
};

#endif // MAINWINDOW_H