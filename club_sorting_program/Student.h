//  Copyright © 2019 MajikalExplosions. All rights reserved.

#ifndef Student_h
#define Student_h

#include <string>
#include <vector>

using namespace std;

class Club;

class Student {
    string m_student_name;
    vector<Club *> choices;
    Club * a;
    Club * b;
public:
    Student(string);
    
    void addChoice(Club);
    void setAWeek(Club);
    void setBWeek(Club);
    Club getAWeek();
    Club getBWeek();
    string getName();
    vector<Club> getChoices();
};

#endif
