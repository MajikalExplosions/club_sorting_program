//  Copyright © 2019 MajikalExplosions. All rights reserved.

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

Club::Club(string name, string w) {
    m_club_name = name;
    if (w.compare("A") == 0) m_week = 2;
    if (w.compare("B") == 0) m_week = 3;
    if (w.compare("AB") == 0) m_week = 6;
}

bool Club::isA() {
    return m_week == 2;
}

bool Club::isB() {
    return m_week == 3;
}

bool Club::isAB() {
    return m_week == 6;
}

bool Club::inA() {
    return m_week % 2 == 0;
}

bool Club::inB() {
    return m_week % 3 == 0;
}
