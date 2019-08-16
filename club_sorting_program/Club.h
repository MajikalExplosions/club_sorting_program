//  Copyright © 2019 MajikalExplosions. All rights reserved.

#ifndef Club_h
#define Club_h

#include <string>
#include <vector>
#include "Student.h"

using namespace std;


class Club {
    string m_club_name;
    bool m_a, m_b;
    vector<int> m_members;
public:
    Club(string);
    string getName();
    void setA(bool);
    void setB(bool);
    bool isA();
    bool isB();
    bool isAB();
    bool inA();
    bool inB();
    void addMember(int);
    vector<int> getMembers();//Perhaps change this to "int getMember(int);" ?
};

#endif
