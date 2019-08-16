//  Copyright © 2019 MajikalExplosions. All rights reserved.

#ifndef Student_h
#define Student_h

#include <string>
#include <vector>

using namespace std;

class Club;

class Student {
    string m_name;
    int m_grade;
    vector<int> m_choicesA, m_choicesB;
    int m_finalA, m_finalB;
public:
    Student(string);
    
    void addChoiceA(int);
    void addChoiceB(int);
    void setAWeek(int);
    void setBWeek(int);
    void setGrade(int);
    int getAWeek();
    int getBWeek();
    int getGrade();
    string getName();
    vector<int> getChoicesA();//Perhaps change this to "int getChoice(int);" ?
    vector<int> getChoicesB();//Perhaps change this to "int getChoice(int);" ?
};

#endif
