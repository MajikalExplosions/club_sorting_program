//  Copyright © 2019 MajikalExplosions. All rights reserved.

//  "If it ain't broke, don't fix it."
//  -Bert Lance

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <deque>
#include <algorithm>
#include <string>
#include <cstdlib>
#include "DataReader.h"
#include "Student.h"
#include <dirent.h>
#include <unistd.h>
#include <lemon/network_simplex.h>
#include <lemon/list_graph.h>

#define CLUB_CAPACITY 20

using namespace lemon;
using namespace std;

bool has_suffix(string, string);
void show_error(string, int);
void dlog(string);
void process_a();
void process_b();

DataFile df("");
bool debug_mode;

int main() {
    debug_mode = false;
    //Display starting instructions
    cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
    cout << "CSUS Club Sorting Program v1.1.1 by MajikalExplosions\n\nInstructions are in Google Drive at https://drive.google.com/drive/folders/1HdzcqUyV1fmEUPqvuYxvMsm8z0VeFbsf" << endl << endl;
    cout << "Enter the data file path and press Enter to continue." << endl;
    string fp;
    getline(cin, fp);
    cout << endl;
    if (fp.at(fp.size() - 1) == ' ') fp = fp.substr(0, fp.size() - 1);
    if (! ((has_suffix(fp, ".tsv") || has_suffix(fp, ".TSV")))) show_error(string("File is not a .TSV"), 10);
    fp.erase(std::remove(fp.begin(), fp.end(), '\\'), fp.end());
    dlog("Uh oh.");
    df = DataFile(fp);
    df.readFile();
    dlog("Read file.");
    srand(std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1));
    process_a();
    process_b();
    df.outputResults();
    cout << "\n\n\nFinished sorting students into clubs. You may now close this window.\n\n";
    return 0;
}

bool has_suffix(string fullString, string ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

void show_error(string desc, int code) {
    cout << "[ERROR] " << desc << " (Error Code " << code << ")" << endl;
    exit(0);
}

void dlog(string desc) {
    if (debug_mode) cout << "[DEBUG] " << desc << endl;
}

string to_uppercase(string str) {
    
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

void process_a() {
    
    ListDigraph flowGraph;
    
    ListDigraph::Node start = flowGraph.addNode(), end = flowGraph.addNode();
    
    vector<ListDigraph::Node> club_nodes, student_nodes;
    vector<ListDigraph::Arc> student_arcs, club_arcs;
    vector<vector<ListDigraph::Arc>> connection_arcs;
    vector<vector<int>> connection_ids;
    //Add student nodes and arcs
    for (int i = 0; i < df.m_students.size(); i++) {
        student_nodes.push_back(flowGraph.addNode());
        student_arcs.push_back(flowGraph.addArc(start, student_nodes.back()));
    }
    
    //Add club nodes and arcs
    for (int i = 0; i < df.m_clubs.size(); i++) {
        club_nodes.push_back(flowGraph.addNode());
        club_arcs.push_back(flowGraph.addArc(club_nodes.back(), end));
    }
    dlog("1");
    //Add connection arcs
    int total = 0;
    for (int s = 0; s < df.m_students.size(); s++) {
        vector<int> h1;
        vector<ListDigraph::Arc> h2;
        connection_ids.push_back(h1);
        connection_arcs.push_back(h2);
        vector<int> choices;
        //first, for optimization, if the first is AB week and 2nd is A week then swap the two
        if (df.m_clubs[df.m_students[s].getChoicesA()[0]].isAB() && df.m_clubs[df.m_students[s].getChoicesA()[1]].isA()) {
            int t = df.m_students[s].getChoicesA()[0];
            df.m_students[s].getChoicesA()[0] = df.m_students[s].getChoicesA()[1];
            df.m_students[s].getChoicesA()[1] = t;
        }
        
        //add in connection arcs
        for (int i = 0; i < df.m_students[s].getChoicesA().size(); i++) {
            bool isAdded = false;
            for (int c = 0; c < choices.size(); c++) {
                if (df.m_students[s].getChoicesA()[i] == choices[c]) isAdded = true;
            }
            if (! isAdded && df.m_students[s].getChoicesA()[i] != df.getStudyHall()) {
                connection_arcs[s].push_back(flowGraph.addArc(student_nodes[s], club_nodes[df.m_students[s].getChoicesA()[i]]));
                connection_ids[s].push_back(df.m_students[s].getChoicesA()[i]);
                choices.push_back(df.m_students[s].getChoicesA()[i]);
                total++;
            }
        }
        //this means they're club leader (supposedly)
        if (choices.size() == 1) continue;
        connection_ids[s].push_back(df.getStudyHall());
        connection_arcs[s].push_back(flowGraph.addArc(student_nodes[s], club_nodes[df.getStudyHall()]));
        total++;
    }
    
    dlog("2");
    //Add upper and cost maps
    ListDigraph::ArcMap<int> upper_map(flowGraph);
    ListDigraph::ArcMap<double> cost_map(flowGraph);
    
    for (int i = 0; i < df.m_students.size(); i++) {
        upper_map[student_arcs[i]] = 1;
        cost_map[student_arcs[i]] = 0;
    }
    for (int s = 0; s < df.m_students.size(); s++) {
        
        //find cost of connection arcs
        
        double cost = 0.0001;
        
        for (int c = 0; c < connection_arcs[s].size(); c++) {
            double rnd = ((double) rand() / (RAND_MAX));
            double offset = 0;
            
            if (df.m_students[s].getGrade() == 12) {
                rnd *= 0.02;
                offset = 0.01;
            }
            else if (df.m_students[s].getGrade() == 11) {
                rnd *= 0.02;
                offset = 0.02;
            }
            else if (df.m_students[s].getGrade() == 10) {
                rnd *= 0.02;
                offset = 0.03;
            }
            else if (df.m_students[s].getGrade() == 9) {
                rnd *= 0.02;
                offset = 0.04;
            }
            else rnd *= 10;
            
            ListDigraph::Arc arc1 = connection_arcs[s][c];
            upper_map[arc1] = 1;
            cost_map[arc1] = cost * (rnd + 1 + offset);
            cost *= 100;
            
        }
    }
    
    
    dlog("3");
    
    for (int i = 0; i < df.m_clubs.size(); i++) {
        upper_map[club_arcs[i]] = CLUB_CAPACITY;
        cost_map[club_arcs[i]] = 0;
    }
    
    upper_map[club_arcs[df.getStudyHall()]] = CLUB_CAPACITY * df.m_clubs.size();
    cost_map[club_arcs[df.getStudyHall()]] = 0;
    //use lemon to process flow
    NetworkSimplex<ListDigraph, int, double> flow(flowGraph);
    flow.upperMap(upper_map);
    flow.costMap(cost_map);
    flow.stSupply(start, end, df.m_students.size());
    NetworkSimplex<ListDigraph, int, double>::ProblemType pt = flow.run();
    if (pt == NetworkSimplex<ListDigraph>::INFEASIBLE || pt == NetworkSimplex<ListDigraph>::UNBOUNDED) show_error("Could not find optimal club assignments.", 20);
    
    
    dlog("4");
    
    for (int s = 0; s < df.m_students.size(); s++) {
        for (int c = 0; c < connection_arcs[s].size(); c++) {
            int f = flow.flow(connection_arcs[s][c]);
            if (f == 1) {
                df.m_students[s].setAWeek(connection_ids[s][c]);
                break;
            }
        }
    }
}

void process_b() {
    
    ListDigraph flowGraph;
    
    ListDigraph::Node start = flowGraph.addNode(), end = flowGraph.addNode();
    
    vector<ListDigraph::Node> club_nodes, student_nodes;
    vector<ListDigraph::Arc> student_arcs, club_arcs;
    vector<vector<ListDigraph::Arc>> connection_arcs;
    vector<vector<int>> connection_ids;
    
    //Add student nodes and arcs
    for (int i = 0; i < df.m_students.size(); i++) {
        student_nodes.push_back(flowGraph.addNode());
        student_arcs.push_back(flowGraph.addArc(start, student_nodes.back()));
    }
    
    //Add club nodes and arcs
    for (int i = 0; i < df.m_clubs.size(); i++) {
        club_nodes.push_back(flowGraph.addNode());
        club_arcs.push_back(flowGraph.addArc(club_nodes.back(), end));
    }
    
    //Add connection arcs
    for (int s = 0; s < df.m_students.size(); s++) {
        vector<int> h1;
        vector<ListDigraph::Arc> h2;
        connection_ids.push_back(h1);
        connection_arcs.push_back(h2);
        vector<int> choices;
        
        //add in connection arcs
        for (int i = 0; i < df.m_students[s].getChoicesB().size(); i++) {
            bool isAdded = false;
            for (int c = 0; c < choices.size(); c++) {
                if (df.m_students[s].getChoicesB()[i] == choices[c]) isAdded = true;
            }
            if (df.m_students[s].getAWeek() == df.m_students[s].getChoicesB()[i]) isAdded = true;
            if (! isAdded && df.m_students[s].getChoicesB()[i] != df.getStudyHall()) {
                connection_arcs[s].push_back(flowGraph.addArc(student_nodes[s], club_nodes[df.m_students[s].getChoicesB()[i]]));
                connection_ids[s].push_back(df.m_students[s].getChoicesB()[i]);
                choices.push_back(df.m_students[s].getChoicesB()[i]);
            }
        }
        //this means they're club leader (supposedly)
        if (choices.size() == 1) continue;
        connection_ids[s].push_back(df.getStudyHall());
        connection_arcs[s].push_back(flowGraph.addArc(student_nodes[s], club_nodes[df.getStudyHall()]));
    }
    //Add upper and cost maps
    ListDigraph::ArcMap<int> upper_map(flowGraph);
    ListDigraph::ArcMap<double> cost_map(flowGraph);
    
    for (int i = 0; i < df.m_students.size(); i++) {
        upper_map[student_arcs[i]] = 1;
        cost_map[student_arcs[i]] = 0;
    }
    
    for (int s = 0; s < df.m_students.size(); s++) {
        
        
        //find cost of connection arcs
        
        double cost = 0.0001;
        
        for (int c = 0; c < connection_arcs[s].size(); c++) {
            double rnd = ((double) rand() / (RAND_MAX));
            double offset = 0;
            
            if (df.m_students[s].getGrade() == 12) {
                rnd *= 0.02;
                offset = 0.01;
            }
            else if (df.m_students[s].getGrade() == 11) {
                rnd *= 0.02;
                offset = 0.02;
            }
            else if (df.m_students[s].getGrade() == 10) {
                rnd *= 0.02;
                offset = 0.03;
            }
            else if (df.m_students[s].getGrade() == 9) {
                rnd *= 0.02;
                offset = 0.04;
            }
            else rnd *= 10;
            
            ListDigraph::Arc arc1 = connection_arcs[s][c];
            upper_map[arc1] = 1;
            cost_map[arc1] = cost * (rnd + 1 + offset);
            cost *= 100;
        }
    }
    for (int i = 0; i < df.m_clubs.size(); i++) {
        upper_map[club_arcs[i]] = CLUB_CAPACITY;
        cost_map[club_arcs[i]] = 0;
    }
    upper_map[club_arcs[df.getStudyHall()]] = CLUB_CAPACITY * df.m_clubs.size();
    cost_map[club_arcs[df.getStudyHall()]] = 0;
    
    //use lemon to process flow
    NetworkSimplex<ListDigraph, int, double> flow(flowGraph);
    flow.upperMap(upper_map);
    flow.costMap(cost_map);
    flow.stSupply(start, end, df.m_students.size());
    NetworkSimplex<ListDigraph, int, double>::ProblemType pt = flow.run();
    
    if (pt == NetworkSimplex<ListDigraph>::INFEASIBLE || pt == NetworkSimplex<ListDigraph>::UNBOUNDED) show_error(string("Could not find optimal club assignments."), 20);
    
    for (int s = 0; s < df.m_students.size(); s++) {
        
        for (int c = 0; c < connection_arcs[s].size(); c++) {
            int f = flow.flow(connection_arcs[s][c]);
            if (f == 1) {
                df.m_students[s].setBWeek(connection_ids[s][c]);
                break;
            }
        }
    }
}
