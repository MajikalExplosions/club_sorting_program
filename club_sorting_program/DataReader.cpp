//  Copyright © 2019 MajikalExplosions. All rights reserved.


#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include "DataReader.h"
#include "Student.h"
#include "Club.h"

using namespace std;

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
