# MVWFWAlgorithm

This project implements the **Modified Vertex-Weight Floyd-Warshall Algorithm** using a Qt-based GUI. The application allows users to load a dataset, process course prerequisites, and generate a semester-wise course schedule.

## Features
- Load course data from a CSV file.
- Display course details (name, credits, prerequisites) in a table.
- Generate and display semester-wise course schedules.
- Measure and display execution time.

## How to Use
1. **Load Dataset**: Click the "Dataset File" button to select a CSV file.
   - The file should be formatted as:
     ```
     Course,Credits,Prerequisites
     Example:
     MMW,3,
     PurCom,3,
     ArtApp,3,PurCom
     ```
2. **Run Algorithm**: Click the "Run" button to process the data and generate the schedule.
3. **View Results**:
   - **Table 1**: Displays all course details.
   - **Table 2**: Displays the semester-wise course schedule.

## Requirements
- **Qt Framework** (6.x or later)
- C++17 or later

## Build Instructions
1. Open the project in **Qt Creator**.
2. Build the project in **Release Mode**.
3. Use `windeployqt` to package the executable with its dependencies.

## License
This project is licensed under the MIT License.