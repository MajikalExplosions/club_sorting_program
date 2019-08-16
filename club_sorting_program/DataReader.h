//  Copyright © 2019 MajikalExplosions. All rights reserved.

#ifndef DataReader_h
#define DataReader_h

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include "Club.h"
#include "Student.h"

class DataFile {
    string m_file_name, m_file_path;
    vector<Club> m_clubs;
    vector<Student> m_students;
    int m_sh;
private:
    string remove_unnecessary_chars(string);
    string to_uppercase(string);
public:
    DataFile(string);
    void readFile();
    vector<Club> getClubs();
    vector<Student> getStudents();
    int getStudyHall();
    void outputResults();
};

#endif
