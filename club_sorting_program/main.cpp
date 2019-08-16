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


#define MAX_FILES 40
#define CLUB_CAPACITY 20

using namespace lemon;
using namespace std;

bool has_suffix(string, string);
void show_error(string, int);
void process_a();
void process_b();

DataFile df("");

int main() {
    //Display starting instructions
    cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
    cout << "CSUS Club Sorting Program by MajikalExplosions\n\nInstructions are in the Google Doc located at https://docs.google.com/document/d/1dU4ZycZOI1EdE42cmntatw8ZZDbrYttpQWUMLISw1rY/edit?usp=sharing" << endl;
    cout << "Enter the data file path and press Enter to continue." << endl;
    string fp;
    getline(cin, fp);
    cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
    
    if (! ((has_suffix(fp, ".tsv") || has_suffix(fp, ".TSV")))) {show_error(string("File is not a .TSV"), 10);}
    
    df = DataFile(fp);
    df.readFile();
    srand(std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1));
    
    process_a();
    process_b();
    return 0;
}

bool has_suffix(string str, string suffix)
{
    return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

void show_error(string desc, int code) {
    cout << "[ERROR] " << desc << " (Error Code " << code << ")" << endl;
    exit(0);
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
    for (int i = 0; i < df.getStudents().size(); i++) {
        student_nodes.push_back(flowGraph.addNode());
        student_arcs.push_back(flowGraph.addArc(start, student_nodes.back()));
    }
    
    //Add club nodes and arcs
    for (int i = 0; i < df.getClubs().size(); i++) {
        club_nodes.push_back(flowGraph.addNode());
        club_arcs.push_back(flowGraph.addArc(club_nodes.back(), end));
    }
    
    //Add connection arcs
    for (int s = 0; s < df.getStudents().size(); s++) {
        vector<int> h1;
        vector<ListDigraph::Arc> h2;
        connection_ids.push_back(h1);
        connection_arcs.push_back(h2);
        vector<int> choices;
        
        //first, for optimization, if the first is AB week and 2nd is A week then swap the two
        if (df.getClubs()[df.getStudents()[s].getChoicesA()[0]].isAB() && df.getClubs()[df.getStudents()[s].getChoicesA()[1]].isA()) {
            int t = df.getStudents()[s].getChoicesA()[0];
            df.getStudents()[s].getChoicesA()[0] = df.getStudents()[s].getChoicesA()[1];
            df.getStudents()[s].getChoicesA()[1] = t;
        }
        
        //add in connection arcs
        for (int i = 0; i < df.getStudents()[s].getChoicesA().size(); i++) {
            bool isAdded = false;
            for (int c = 0; c < choices.size(); c++) {
                if (df.getStudents()[s].getChoicesA()[i] == choices[c]) isAdded = true;
            }
            if (! isAdded && df.getStudents()[s].getChoicesA()[i] != df.getStudyHall()) {
                connection_arcs[s].push_back(flowGraph.addArc(student_nodes[s], club_nodes[df.getStudents()[s].getChoicesA()[i]]));
                connection_ids[s].push_back(df.getStudents()[s].getChoicesA()[i]);
                choices.push_back(df.getStudents()[s].getChoicesA()[i]);
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
    
    for (int i = 0; i < df.getStudents().size(); i++) {
        upper_map[student_arcs[i]] = 1;
        cost_map[student_arcs[i]] = 0;
    }
    
    for (int s = 0; s < df.getStudents().size(); s++) {
        
        
        //find cost of connection arcs
        
        double cost = 0.0001;
        
        for (int c = 0; c < df.getStudents()[s].getChoicesA().size(); c++) {
            double rnd = ((double) rand() / (RAND_MAX));
            double offset = 0;
            
            if (df.getStudents()[s].getGrade() == 12) {
                rnd *= 0.02;
                offset = 0.01;
            }
            else if (df.getStudents()[s].getGrade() == 11) {
                rnd *= 0.03;
                offset = 0.015;
            }
            else if (df.getStudents()[s].getGrade() == 10) {
                rnd *= 0.04;
                offset = 0.02;
            }
            else if (df.getStudents()[s].getGrade() == 9) {
                rnd *= 0.05;
                offset = 0.025;
            }
            else rnd *= 10;
            ListDigraph::Arc arc1 = connection_arcs[s][c];
            upper_map[arc1] = 1;
            cost_map[arc1] = cost * (rnd + 1 + offset);
            cost *= 100;
        }
    }
    for (int i = 0; i < df.getClubs().size(); i++) {
        upper_map[club_arcs[i]] = CLUB_CAPACITY;
        cost_map[club_arcs[i]] = 0;
    }
    
    upper_map[club_arcs[df.getStudyHall()]] = CLUB_CAPACITY;
    cost_map[club_arcs[df.getStudyHall()]] = 0;
    
    //use lemon to process flow
    NetworkSimplex<ListDigraph, int, double> flow(flowGraph);
    flow.upperMap(upper_map);
    flow.costMap(cost_map);
    flow.stSupply(start, end, df.getStudents().size());
    NetworkSimplex<ListDigraph, int, double>::ProblemType pt = flow.run();
    
    if (pt == NetworkSimplex<ListDigraph>::INFEASIBLE || pt == NetworkSimplex<ListDigraph>::UNBOUNDED) show_error("Could not find optimal club assignments.", 20);
    
    for (int s = 0; s < df.getStudents().size(); s++) {
        for (int c = 0; c < connection_arcs[s].size(); c++) {
            int f = flow.flow(connection_arcs[s][c]);
            if (f == 1) {
                df.getStudents()[s].setAWeek(connection_ids[s][c]);
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
    for (int i = 0; i < df.getStudents().size(); i++) {
        student_nodes.push_back(flowGraph.addNode());
        student_arcs.push_back(flowGraph.addArc(start, student_nodes.back()));
    }
    
    //Add club nodes and arcs
    for (int i = 0; i < df.getClubs().size(); i++) {
        club_nodes.push_back(flowGraph.addNode());
        club_arcs.push_back(flowGraph.addArc(club_nodes.back(), end));
    }
    
    //Add connection arcs
    for (int s = 0; s < df.getStudents().size(); s++) {
        vector<int> h1;
        vector<ListDigraph::Arc> h2;
        connection_ids.push_back(h1);
        connection_arcs.push_back(h2);
        vector<int> choices;
        
        //add in connection arcs
        for (int i = 0; i < df.getStudents()[s].getChoicesB().size(); i++) {
            bool isAdded = false;
            for (int c = 0; c < choices.size(); c++) {
                if (df.getStudents()[s].getChoicesB()[i] == choices[c]) isAdded = true;
            }
            if (df.getStudents()[s].getAWeek() == df.getStudents()[s].getChoicesB()[i]) isAdded = true;
            if (! isAdded && df.getStudents()[s].getChoicesB()[i] != df.getStudyHall()) {
                connection_arcs[s].push_back(flowGraph.addArc(student_nodes[s], club_nodes[df.getStudents()[s].getChoicesB()[i]]));
                connection_ids[s].push_back(df.getStudents()[s].getChoicesB()[i]);
                choices.push_back(df.getStudents()[s].getChoicesB()[i]);
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
    
    for (int i = 0; i < df.getStudents().size(); i++) {
        upper_map[student_arcs[i]] = 1;
        cost_map[student_arcs[i]] = 0;
    }
    
    for (int s = 0; s < df.getStudents().size(); s++) {
        
        
        //find cost of connection arcs
        
        double cost = 0.0001;
        
        for (int c = 0; c < df.getStudents()[s].getChoicesB().size(); c++) {
            double rnd = ((double) rand() / (RAND_MAX));
            double offset = 0;
            
            if (df.getStudents()[s].getGrade() == 12) {
                rnd *= 0.02;
                offset = 0.01;
            }
            else if (df.getStudents()[s].getGrade() == 11) {
                rnd *= 0.03;
                offset = 0.015;
            }
            else if (df.getStudents()[s].getGrade() == 10) {
                rnd *= 0.04;
                offset = 0.02;
            }
            else if (df.getStudents()[s].getGrade() == 9) {
                rnd *= 0.05;
                offset = 0.025;
            }
            else rnd *= 10;
            ListDigraph::Arc arc1 = connection_arcs[s][c];
            upper_map[arc1] = 1;
            cost_map[arc1] = cost * (rnd + 1 + offset);
            cost *= 100;
        }
    }
    for (int i = 0; i < df.getClubs().size(); i++) {
        upper_map[club_arcs[i]] = CLUB_CAPACITY;
        cost_map[club_arcs[i]] = 0;
    }
    
    upper_map[club_arcs[df.getStudyHall()]] = CLUB_CAPACITY;
    cost_map[club_arcs[df.getStudyHall()]] = 0;
    
    //use lemon to process flow
    NetworkSimplex<ListDigraph, int, double> flow(flowGraph);
    flow.upperMap(upper_map);
    flow.costMap(cost_map);
    flow.stSupply(start, end, df.getStudents().size());
    NetworkSimplex<ListDigraph, int, double>::ProblemType pt = flow.run();
    
    if (pt == NetworkSimplex<ListDigraph>::INFEASIBLE || pt == NetworkSimplex<ListDigraph>::UNBOUNDED) show_error(string("Could not find optimal club assignments."), 20);
    
    for (int s = 0; s < df.getStudents().size(); s++) {
        for (int c = 0; c < connection_arcs[s].size(); c++) {
            int f = flow.flow(connection_arcs[s][c]);
            if (f == 1) {
                df.getStudents()[s].setBWeek(connection_ids[s][c]);
                break;
            }
        }
    }
}
