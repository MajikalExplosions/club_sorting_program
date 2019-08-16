//  Copyright © 2019 MajikalExplosions. All rights reserved.

#include "Student.h"




Student::Student(string name) {
    m_name = name;
}

int Student::getGrade() {
    return m_grade;
}

void Student::setGrade(int g) {
    m_grade = g;
}

void Student::addChoiceA(int c) {
    m_choicesA.push_back(c);
}

void Student::addChoiceB(int c) {
    m_choicesB.push_back(c);
}

void Student::setAWeek(int a) {
    m_finalA = a;
}

void Student::setBWeek(int b) {
    m_finalA = b;
}

int Student::getAWeek() {
    return m_finalA;
}

int Student::getBWeek() {
    return m_finalB;
}

string Student::getName() {
    return m_name;
}

vector<int> Student::getChoicesA() {
    return m_choicesA;
}

vector<int> Student::getChoicesB() {
    return m_choicesB;
}
