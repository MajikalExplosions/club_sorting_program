//  Copyright © 2019 MajikalExplosions. All rights reserved.

#ifndef DataReader_h
#define DataReader_h

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include "Club.h"

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

#endif
