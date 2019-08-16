//  Copyright © 2019 MajikalExplosions. All rights reserved.

#include <string>
#include <vector>
#include "Club.h"

using namespace std;

Club::Club(string name) {
    m_club_name = name;
}

void Club::setA(bool b) {
    m_a = b;
}

void Club::setB(bool b) {
    m_b = b;
}

string Club::getName() {
    return m_club_name;
}

bool Club::isA() {
    return m_a && ! m_b;
}

bool Club::isB() {
    return m_b && ! m_a;
}

bool Club::isAB() {
    return m_a && m_b;
}

bool Club::inA() {
    return m_a;
}

bool Club::inB() {
    return m_b;
}

void Club::addMember(int s) {
    m_members.push_back(s);
}

vector<int> Club::getMembers() {
    return m_members;
}
