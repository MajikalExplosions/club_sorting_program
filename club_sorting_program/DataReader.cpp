//  Copyright © 2019 MajikalExplosions. All rights reserved.


#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

#include "DataReader.h"
#include "Student.h"
#include "Club.h"

#define MAX_CHOICES 10

using namespace std;

DataFile::DataFile (string filePath) {
    if (filePath.empty()) return;
    m_file_name = filePath.substr(filePath.find_last_of("/\\") + 1);
    m_file_path = filePath;
    m_clubs = vector<Club>();
    m_students = vector<Student>();
    m_sh = -1;
}

void DataFile::readFile() {
    ifstream data(m_file_path);
    vector<vector<string>> data_table;
    //read .TSV file
    string line;
    while(getline(data,line))
    {
        stringstream lineStream(line);
        string cell;
        vector<string> parsedRow;
        while(getline(lineStream,cell,'\t'))
        {
            cell = remove_unnecessary_chars(cell);
            if (cell.length() <= 1) parsedRow.push_back("");//useless values.  No point keeping them.
            else parsedRow.push_back(cell);
        }
        
        data_table.push_back(parsedRow);
    }
    
    //Find column headings
    int COL_NAME = -1;
    int COL_GRADE = -1;
    int COL_CHOICE_START_A = -1;
    int NUM_CHOICES_A = -1;
    int COL_CHOICE_START_B = -1;
    int NUM_CHOICES_B = -1;
    
    for (int col_num = 0; col_num < data_table[0].size(); col_num++) {
        string col = to_uppercase(data_table[0][col_num]);
        
        col.erase(remove(col.begin(), col.end(), '"'), col.end());
        if (col.find("NAME") != -1) {
            //This is the student name column
            COL_NAME = col_num;
        }
        else if (col.find("GRADE") != -1) {
            //This is the grade column
            COL_GRADE = col_num;
        }
        else if ((col.find("FIRST") != -1 || col.find("1st") != -1) && (col.find(" A") != -1 || col.find("A ") != -1)) {
            //This is the first club column
            COL_CHOICE_START_A = col_num;
            NUM_CHOICES_A = 1;
        }
        else if ((col.find("FIRST") != -1 || col.find("1st") != -1) && (col.find(" B") != -1 || col.find("B ") != -1)) {
            //This is the first club column
            COL_CHOICE_START_B = col_num;
            NUM_CHOICES_B = 1;
        }
        else if ((col.find(" A") != -1 || col.find("A ") != -1) && col.find("CHOICE") != -1) {
            NUM_CHOICES_A++;
        }
        else if ((col.find(" B") != -1 || col.find("B ") != -1) && col.find("CHOICE") != -1) {
            NUM_CHOICES_B++;
        }
    }
    //Create Student and Club objects
    for (int row = 1; row < data_table.size(); row++) {
        if (data_table[row][COL_NAME].compare("") == 0) break;
        m_students.push_back(Student(data_table[row][COL_NAME]));
        
        if (to_uppercase(data_table[row][COL_GRADE]).compare("FRESHMAN") == 0 || to_uppercase(data_table[row][COL_GRADE]).compare("FRESHMEN") == 0 || to_uppercase(data_table[row][COL_GRADE]).compare("9") == 0) {
            m_students[m_students.size() - 1].setGrade(9);
        }
        else if (to_uppercase(data_table[row][COL_GRADE]).compare("SOPHOMORE") == 0 || to_uppercase(data_table[row][COL_GRADE]).compare("10") == 0) {
            m_students[m_students.size() - 1].setGrade(10);
        }
        else if (to_uppercase(data_table[row][COL_GRADE]).compare("JUNIOR") == 0 || to_uppercase(data_table[row][COL_GRADE]).compare("11") == 0) {
            m_students[m_students.size() - 1].setGrade(11);
        }
        else if (to_uppercase(data_table[row][COL_GRADE]).compare("SENIOR") == 0 || to_uppercase(data_table[row][COL_GRADE]).compare("12") == 0) {
            m_students[m_students.size() - 1].setGrade(12);
        }
        
        
        for (int c = COL_CHOICE_START_A; c < COL_CHOICE_START_A + NUM_CHOICES_A; c++) {
            bool in = false;
            Club cl = Club("");
            for (int v = 0; v < m_clubs.size(); v++) {
                if (m_clubs[v].getName().compare(data_table[row][c]) == 0) {
                    in = true;
                    cl = m_clubs[v];
                    m_students[m_students.size() - 1].addChoiceA(v);
                }
            }
            if (! in) {
                cl = Club(data_table[row][c]);
                m_clubs.push_back(cl);
                if (cl.getName().find("STUDY") != -1 && cl.getName().find("HALL") != -1 && m_sh == -1) m_sh = m_clubs.size() - 1;
                m_students[m_students.size() - 1].addChoiceA(m_clubs.size() - 1);
            }
            cl.setA(true);
        }
        
        for (int c = COL_CHOICE_START_B; c < COL_CHOICE_START_B + NUM_CHOICES_B; c++) {
            bool in = false;
            Club cl = Club("");
            for (int v = 0; v < m_clubs.size(); v++) {
                if (m_clubs[v].getName().compare(data_table[row][c]) == 0) {
                    in = true;
                    cl = m_clubs[v];
                    m_students[m_students.size() - 1].addChoiceB(v);
                }
            }
            if (! in) {
                cl = Club(data_table[row][c]);
                m_clubs.push_back(cl);
                if (to_uppercase(cl.getName()).find("STUDY") != -1 && to_uppercase(cl.getName()).find("HALL") != -1 && m_sh == -1) {
                    m_sh = m_clubs.size() - 1;
                }
                m_students[m_students.size() - 1].addChoiceB(m_clubs.size() - 1);
            }
            cl.setB(true);
        }
    }
    
    //add study hall if it doesn't exist
    
    if (m_sh == -1) {
        m_sh = m_clubs.size();
        m_clubs.push_back(Club("STUDY HALL"));
    }
    
    //for (int i = 0; i < m_clubs.size(); i++) cout << m_clubs[i].getName() << endl;
}

vector<Club> DataFile::getClubs() {
    return m_clubs;
}

int DataFile::getStudyHall() {
    return m_sh;
}

vector<Student> DataFile::getStudents() {
    return m_students;
}

string DataFile::remove_unnecessary_chars(string str) {
    char char_array[str.length() + 1];
    int added = 0;
    for(int i = 0; i < str.length(); i++)
    {
        //0xc3 and 0xbc are hacks for that funny U in "Cru" because that thing is in UTF-8.  i hate you guys
        if (((int)str.at(i) <= 126 && (int)str.at(i) >= 32) || str.at(i) == (char)0xc3 || str.at(i) == (char)0xbc) {//Anything that isn't displayed is useless.
            char_array[added++] = str.at(i);
        }
    }
    
    char temp[added + 1];
    
    for (int i = 0; i < added; i++) {
        temp[i] = char_array[i];
    }
    temp[added] = 0;
    string n = string(temp);
    if (n.at(0) == ' ') n = n.substr(1);
    if (n.at(n.size() - 1) == ' ') n = n.substr(0, n.size() - 1);
    return n;
}

string DataFile::to_uppercase(string str) {
    
    char char_array[str.length() + 1];
    int added = 0;
    for(int i = 0; i < str.length(); i++)
    {
        if (((int)str.at(i) <= 90 && (int)str.at(i) >= 65) || ((int)str.at(i) <= 122 && (int)str.at(i) >= 97)) {
            //Letters
            char_array[added++] = toupper(str.at(i));
        }
        else if (((int)str.at(i) <= 64 && (int)str.at(i) >= 32) || ((int)str.at(i) <= 126 && (int)str.at(i) >= 123)) {
            //Other useful characters
            char_array[added++] = str.at(i);
        }
    }
    
    char temp[added + 1];
    
    for (int i = 0; i < added; i++) {
        temp[i] = char_array[i];//I don't want all those null values.
    }
    
    temp[added] = 0;
    return string(temp);
}


void DataFile::outputResults() {
    
    //output results to .tsv file
    ofstream f_out;
    f_out.open(m_file_path + "_results.tsv");
    f_out << "Name\tA Week\tB Week";
    for (int i = 0; i < m_students.size(); i++) {
        cout << m_students[i].getName() << " " << m_students[i].getAWeek() << endl;
        f_out << "\n" << m_students[i].getName() << "\t" << m_clubs[m_students[i].getAWeek()].getName() << "\t" << m_clubs[m_students[i].getBWeek()].getName();
    }
    f_out.close();
    
    
    vector<int> total_count_a, total_count_b;
    
    for (int i = 0; i < MAX_CHOICES; i++) {
        total_count_a.push_back(0);
        total_count_b.push_back(0);
    }
    
    //find total counts
    
    for (int i = 0; i < m_students.size(); i++) {
        
        for (int c = 0; c < m_students[i].getChoicesA().size(); c++) {
            if (c == m_students[i].getAWeek()) {
                total_count_a[c]++;
                break;
            }
        }
        
        for (int c = 0; c < m_students[i].getChoicesB().size(); c++) {
            if (c == m_students[i].getBWeek()) {
                total_count_b[c]++;
                break;
            }
        }
    }
    
    //output results to .tsv file
    f_out.open(m_file_path + "_statistics.tsv");
    f_out << "Statistics\nChoice #\tA Week Students\tB Week Students";
    for (int i = 0; i < MAX_CHOICES; i++) {
        f_out << "\n" << i + 1 << "\t" << total_count_a[i] << "\t" << total_count_b[i];
    }
    f_out.close();
}
