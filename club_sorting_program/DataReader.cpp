//  Copyright © 2019 MajikalExplosions. All rights reserved.


#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include "Club.cpp"

using namespace std;

class DataFile {
    string m_file_name, m_file_path;
    vector<Club> m_clubs;
    vector<string> m_student_names;
    vector<vector<int>> m_club_choices;
    public:
    DataFile(string, string);
    void readFile();
    string choice(int, int);
};

DataFile::DataFile (string currentPath, string fileName) {
    m_file_name = fileName;
    m_file_path = currentPath.substr(0, currentPath.find_last_of("/\\"));
    m_clubs = vector<Club>();
    m_student_names = vector<string>();
}

void DataFile::readFile() {
    
}

string DataFile::choice(int student, int choice) {
    return "";
}
