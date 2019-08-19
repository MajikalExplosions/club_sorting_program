//  Copyright © 2019 MajikalExplosions. All rights reserved.

#include "Student.h"
#include <iostream>





Student::Student(string name) {
    m_name = name;
    m_grade = -1;
    m_finalA = -1;
    m_finalB = -1;
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

void Student::setAWeek(int a1) {
    m_finalA = a1;
    //std::cout << "Set " << m_name << " club A to " << m_finalA << endl;
}

void Student::setBWeek(int b1) {
    m_finalB = b1;
    //std::cout << "Set " << m_name << " club B to " << m_finalB << " cA " << m_finalA << endl;
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
