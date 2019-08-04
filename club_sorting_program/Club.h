//  Copyright © 2019 MajikalExplosions. All rights reserved.

#ifndef Club_h
#define Club_h

#include <string>
#include <vector>

using namespace std;

class Club {
    string m_club_name;
    int m_week;
public:
    Club(string, string);
    
    bool isA();
    bool isB();
    bool isAB();
    bool inA();
    bool inB();
};

#endif
