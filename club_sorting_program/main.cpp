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

#include <dirent.h>
#include <unistd.h>
#include <lemon/network_simplex.h>
#include <lemon/list_graph.h>


#include "DataReader.h"
#include "Club.h"

#define MAX_FILES 40
#define CLUB_CAPACITY 20

using namespace lemon;
using namespace std;

string files[MAX_FILES];
vector<vector<string>> data_table;
vector<string> a_week_results, b_week_results;
string current_file_name;
string current_path;
int COL_STUDENT_NAME, COL_EMAIL, COL_GRADE, COL_CHOICE_START, NUM_CHOICES, COL_WEEK_A, COL_WEEK_B;
bool usingEmail;

//Functions defined after main.
bool has_suffix(string str, string suffix);
int list_dir(const char *path);
void parseTSV(string filePath);
void process_a(vector<string> s);
void process_b(vector<string> s);
string remove_unnecessary_chars(string str);
string to_uppercase(string str);
void write_results_to_file(vector<string> s, string s2);
void write_statistics_to_file(vector<string> s, string s2);

int main(int argc, const char * argv[]) {
    //Display starting instructions
    cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
    cout << "[INSTRUCTIONS] Please place a .TSV file (spreadsheet) containing the club form responses in the same directory(folder) as this program.\n" <<
    "Additionally, make sure the columns include headings (Row A) that specify the contents of the row (For example, name, timestamp, email, grade, etc.)\n" <<
    "Lastly, please make sure the choices are at the right side of the file, with the leftmost column being the 1st choice.\n" <<
    "\nHere's an example:\n" <<
    "  Name  |     Email     |  Grade  | 1st Choice | 2nd Choice | 3rd Choice|" << endl <<
    "--------|---------------|---------|------------|------------|-----------|" << endl <<
    "Allison |afong@csus.org | Senior  |Days for Gi.|Yearbook    |Green Team |" << endl;
    
    cout << "Press Enter to continue.\n";
    cin.ignore();
    cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
    //Get path of files
    string currentPath(argv[0]);
    currentPath = currentPath.substr(0, currentPath.find_last_of("/\\"));
    current_path = currentPath;
    if (currentPath.find_last_of("/\\") < 0) {
        cout << "[ERROR] Path parsing failed. Does this program have a / or \\ in its name? (Error Code #195)\n" << endl;
        return 195;
    }
    
    //Get files
    int numFiles = list_dir(currentPath.c_str());
    
    //Process files
    bool processed = false;
    
    for (int fileNum = 0; fileNum < numFiles; fileNum++) {
        //For each file, if it's a csv then process.
        if ((has_suffix(files[fileNum], ".tsv") || has_suffix(files[fileNum], ".TSV")) && files[fileNum].substr(0, 9).compare("_RESULTS_") != 0 && files[fileNum].substr(0, 12).compare("_STATISTICS_") != 0) {
            processed = true;
            current_file_name = files[fileNum];
            cout << "Processing " << files[fileNum] << "..." << endl << endl;
            
            //Get file data and put in data_table
            parseTSV(currentPath + "/" + files[fileNum]);
            
            //find row numbers for each category below
            COL_STUDENT_NAME = -1;
            COL_EMAIL = -1;
            COL_GRADE = -1;
            COL_CHOICE_START = -1;
            NUM_CHOICES = -1;
            COL_WEEK_A = -1;
            COL_WEEK_B = -1;
            
            for (int col_num = 0; col_num < data_table.at(0).size(); col_num++) {
                string col = to_uppercase(data_table[0][col_num]);
                
                col.erase(remove(col.begin(), col.end(), '"'), col.end());
                if (col.find("TIME") != -1) {
                    //This is the timestamp column
                    continue;
                }
                else if (col.find("NAME") != -1) {
                    //This is the student name column
                    COL_STUDENT_NAME = col_num;
                }
                else if (col.find("MAIL") != -1) {
                    //This is the email column
                    COL_EMAIL = col_num;
                }
                else if (col.find("GRADE") != -1) {
                    //This is the grade column
                    COL_GRADE = col_num;
                }
                else if ((col.find("CHOICE") != -1 || col.find("CLUB") != -1) && COL_CHOICE_START == -1) {
                    //This is the first club column
                    COL_CHOICE_START = col_num;
                    NUM_CHOICES = 1;
                }
                else if (col.find("A") != -1 && col.find("WEEK") != -1) {
                    COL_WEEK_A = col_num;
                }
                else if (col.find("B") != -1 && col.find("WEEK") != -1) {
                    COL_WEEK_B = col_num;
                }
                else if (col.find("CHOICE") != -1 || col.find("CLUB") != -1) {
                    NUM_CHOICES++;
                }
                else {
                    cout << "[WARNING] Found unknown column heading.\n" << endl;
                }
            }
            if (COL_STUDENT_NAME == -1 && COL_EMAIL == -1) {
                cout << "[ERROR] Student or email column missing.  Please try again. (Error Code #192)\n" << endl;
                return 192;
            }
            if (COL_CHOICE_START == -1) {
                cout << "[ERROR] No choice columns found.  Please try again. (Error Code #193)\n" << endl;
                return 193;
            }
            if (NUM_CHOICES <= 1) {
                cout << "[ERROR] Not enough choices per person found.  Please try again. (Error Code #194)\n" << endl;
                return 194;
            }
            if (COL_WEEK_A == -1 || COL_WEEK_B == -1) {
                cout << "[ERROR] A and B Week club columns not found.  Please try again. (Error Code #196)\n" << endl;
                return 196;
            }
            
            //Add students to array
            cout << "Finding students..." << endl;
            vector<string> students;
            for (int row = 1; row < data_table.size(); row++) {
                if (data_table[row][COL_STUDENT_NAME].compare("") == 0) break;
                students.push_back(data_table[row][COL_STUDENT_NAME]);
            }
            cout << "Found all students." << endl << endl;
            
            
            srand(std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1));
            process_a(students);
            process_b(students);
            write_results_to_file(students, "_RESULTS_");
            write_statistics_to_file(students, "_STATISTICS_");
            
            cout << "Finished processing " << files[fileNum] << "." << endl << "--------------------" << endl << endl;
        }
    }
    
    
    if (! processed) {
        cout << "[ERROR] No .TSV file found.  Please try again.\n (Error Code #190)\n";
        return 190;
    }
    return 0;
}



/************************************************************/
/*                                                          */
/*                                                          */
/*       ALL FUNCTIONS ARE BELOW. ALPHABETICAL ORDER.       */
/*                                                          */
/*                                                          */
/************************************************************/



bool has_suffix(string str, string suffix)
{
    return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

int list_dir(const char *path) {
    struct dirent *entry;
    DIR *dir = opendir(path);
    
    if (dir == NULL) {
        return -1;
    }
    int i = 0;
    while ((entry = readdir(dir)) != NULL && i < MAX_FILES) {
        files[i++] = entry->d_name;
    }
    closedir(dir);
    return i;
}

void parseTSV(string filePath)
{
    data_table.clear();
    ifstream data(filePath);
    string line;
    while(getline(data,line))
    {
        stringstream lineStream(line);
        string cell;
        vector<string> parsedRow;
        while(getline(lineStream,cell,'\t'))
        {
            cell = remove_unnecessary_chars(cell);
            if (cell.length() <= 1) parsedRow.push_back("");//useless values.  No point keeping them.
            else parsedRow.push_back(cell);
        }
        
        data_table.push_back(parsedRow);
    }
}

void process_a(vector<string> students) {
    cout << "Processing A week choice data..." << endl;
    
    //Grab all clubs
    vector<string> clubs;
    int studyHall = -1;
    for (int row = 1; row < data_table.size(); row++) {
        
        //Check to make sure data exists
        if (data_table[row].size() <= COL_WEEK_A) break;
        
        string c_club = data_table[row][COL_WEEK_A];
        if (c_club.length() <= 1) break;//Starting here the clubs are gone, possibly because there are more clubs in B week than A week.
        
        bool isInVector = false;
        for (int i = 0; i < clubs.size(); i++) {
            if (clubs[i].compare(c_club) == 0) {
                isInVector = true;
                break;
            }
        }
        if (! isInVector) {
            clubs.push_back(c_club);
            if (to_uppercase(c_club).compare("STUDY HALL") == 0 || to_uppercase(c_club).compare("STUDYHALL") == 0) {
                studyHall = row - 1;
            }
        }
    }
    
    //cout << "I hate seg faults1." << endl;
    ListDigraph flowGraph;
    
    ListDigraph::Node start = flowGraph.addNode(), end = flowGraph.addNode();
    
    vector<ListDigraph::Node> club_nodes, student_nodes;
    vector<ListDigraph::Arc> student_arcs, club_arcs;
    vector<vector<ListDigraph::Arc>> connection_arcs;
    vector<vector<int>> connection_arcs_d;
    
    //cout << "I hate seg faults2." << endl;
    //Add student nodes and arcs
    for (int i = 0; i < students.size(); i++) {
        student_nodes.push_back(flowGraph.addNode());
        student_arcs.push_back(flowGraph.addArc(start, student_nodes.back()));
        //cout << "Adding arc from start to " << students[i] << endl;
    }
    
    //cout << "I hate seg faults3." << endl;
    //Add club nodes and arcs
    for (int i = 0; i < clubs.size(); i++) {
        club_nodes.push_back(flowGraph.addNode());
        club_arcs.push_back(flowGraph.addArc(club_nodes.back(), end));
        //cout << "Adding arc from " << clubs[i] << " to end" << endl;
    }
    
    //cout << "I hate seg faults4." << endl;
    //Add connection arcs
    for (int s = 0; s < students.size(); s++) {
        vector<int> h1;
        vector<ListDigraph::Arc> h2;
        connection_arcs_d.push_back(h1);
        connection_arcs.push_back(h2);
        string name = students[s];
        //ic
        vector<string> choices;
        //cout << "P: " << name << endl;
        //add in connection arcs
        for (int i = 0; i < NUM_CHOICES; i++) {
            bool c_ex = false;
            for (int c = 0; c < clubs.size(); c++) {
                //cout << i + COL_CHOICE_START << " " << row << " ";
                //cout << "Club:" << data_table[row][i + COL_CHOICE_START] << endl;
                if (data_table[s + 1][i + COL_CHOICE_START].compare(clubs[c]) == 0) {
                    bool exists = false;
                    
                    c_ex = true;
                    for (int j = 0; j < choices.size(); j++) {
                        if (choices[j].compare(clubs[c]) == 0) exists = true;
                    }
                    //cout << "E: " << exists << endl;
                    if (! exists) {
                        if (! (to_uppercase(clubs[c]).compare("STUDY HALL") == 0 || to_uppercase(clubs[c]).compare("STUDYHALL") == 0)) {
                            connection_arcs[s].push_back(flowGraph.addArc(student_nodes[s], club_nodes[c]));
                            connection_arcs_d[s].push_back(c);
                            choices.push_back(clubs[c]);
                        }
                        break;
                        //cout << "Adding arc from " << students[s] << " to " << clubs[c] << endl;
                    }
                }
            }
        }
        
        connection_arcs[s].push_back(flowGraph.addArc(student_nodes[s], club_nodes[studyHall]));
        connection_arcs_d[s].push_back(studyHall);
    }
    //Add upper and cost maps
    ListDigraph::ArcMap<int> upper_map(flowGraph);
    ListDigraph::ArcMap<double> cost_map(flowGraph);
    
    for (int i = 0; i < students.size(); i++) {
        upper_map[student_arcs[i]] = 1;
        cost_map[student_arcs[i]] = 0;
    }
    
    for (int s = 0; s < students.size(); s++) {
        
        string name = students[s];
        vector<int> choices;
        bool choices_has_sh = false;
        
        //find connection arcs
        for (int i = 0; i < NUM_CHOICES; i++) {
            for (int c = 0; c < clubs.size(); c++) {
                if (data_table[s + 1][i + COL_CHOICE_START].compare(clubs[c]) == 0) {
                    bool exists = false;
                    for (int j = 0; j < choices.size(); j++) {
                        if (choices[j] == c) exists = true;
                    }
                    if (! exists) {
                        choices.push_back(c);
                        if (to_uppercase(clubs[c]).compare("STUDY HALL") == 0 || to_uppercase(clubs[c]).compare("STUDYHALL") == 0) choices_has_sh = true;
                    }
                }
            }
        }
        if (! choices_has_sh) choices.push_back(studyHall);
        
        
        //find cost of connection arcs
        
        double cost = 0.0001;
        
        int grade = 0;
        if (to_uppercase(data_table[s + 1][COL_GRADE]).compare("FRESHMAN") == 0 || to_uppercase(data_table[s + 1][COL_GRADE]).compare("FRESHMEN") == 0) {
            grade = 9;
        }
        else if (to_uppercase(data_table[s + 1][COL_GRADE]).compare("SOPHOMORE") == 0) {
            grade = 10;
        }
        else if (to_uppercase(data_table[s + 1][COL_GRADE]).compare("JUNIOR") == 0) {
            grade = 11;
        }
        else if (to_uppercase(data_table[s + 1][COL_GRADE]).compare("SENIOR") == 0) {
            grade = 12;
        }
        
        for (int c = 0; c < choices.size(); c++) {
            double rnd = ((double) rand() / (RAND_MAX));
            double offset = 0;
            
            if (grade == 12) {
                rnd *= 0.02;
                offset = 0.01;
            }
            else if (grade == 11) {
                rnd *= 0.03;
                offset = 0.015;
            }
            else if (grade == 10) {
                rnd *= 0.04;
                offset = 0.02;
            }
            else if (grade == 9) {
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
    
    for (int i = 0; i < clubs.size(); i++) {
        //Default club size is 20 unless Allison emails a second time saying that she changed her mind
        //No max student count for study hall.
        if (to_uppercase(clubs[i]).compare("STUDY HALL") == 0 || to_uppercase(clubs[i]).compare("STUDYHALL") == 0) upper_map[club_arcs[i]] = students.size() * 10;
        else upper_map[club_arcs[i]] = CLUB_CAPACITY;
        cost_map[club_arcs[i]] = 0;
    }
    
    //use lemon to process flow
    NetworkSimplex<ListDigraph, int, double> flow(flowGraph);
    flow.upperMap(upper_map);
    flow.costMap(cost_map);
    flow.stSupply(start, end, students.size());
    NetworkSimplex<ListDigraph, int, double>::ProblemType pt = flow.run();
    
    if (pt == NetworkSimplex<ListDigraph>::INFEASIBLE || pt == NetworkSimplex<ListDigraph>::UNBOUNDED) cout << "[ERROR] Flow could not be calculated." << endl;
    
    //Create flow data from clubs
    a_week_results = vector<string>();
    for (int i = 0; i < students.size(); i++) {
        for (int c = 0; c < connection_arcs[i].size(); c++) {
            int f = flow.flow(connection_arcs[i][c]);
            if (f == 1) {
                a_week_results.push_back(clubs[connection_arcs_d[i][c]]);
                break;
            }
        }
    }
    
    cout << "Finished processing A week choice data." << endl;
}

/******************************/

void process_b(vector<string> students) {
    cout << "Processing B week choice data..." << endl;
    
    //Grab all clubs
    vector<string> clubs;
    int studyHall = -1;
    for (int row = 1; row < data_table.size(); row++) {
        
        //Check to make sure data exists
        if (data_table[row].size() <= COL_WEEK_B) break;
        
        string c_club = data_table[row][COL_WEEK_B];
        if (c_club.length() <= 1) break;//Starting here the clubs are gone, possibly because there are more clubs in B week than A week.
        
        bool isInVector = false;
        for (int i = 0; i < clubs.size(); i++) {
            if (clubs[i].compare(c_club) == 0) {
                isInVector = true;
                break;
            }
        }
        if (! isInVector) {
            clubs.push_back(c_club);
            if (to_uppercase(c_club).compare("STUDY HALL") == 0 || to_uppercase(c_club).compare("STUDYHALL") == 0) {
                studyHall = row - 1;
            }
        }
    }
    
    //cout << "I hate seg faults1." << endl;
    ListDigraph flowGraph;
    
    ListDigraph::Node start = flowGraph.addNode(), end = flowGraph.addNode();
    
    vector<ListDigraph::Node> club_nodes, student_nodes;
    vector<ListDigraph::Arc> student_arcs, club_arcs;
    vector<vector<ListDigraph::Arc>> connection_arcs;
    vector<vector<int>> connection_arcs_d;
    //Add student nodes and arcs
    for (int i = 0; i < students.size(); i++) {
        student_nodes.push_back(flowGraph.addNode());
        student_arcs.push_back(flowGraph.addArc(start, student_nodes.back()));
    }
    
    //Add club nodes and arcs
    for (int i = 0; i < clubs.size(); i++) {
        club_nodes.push_back(flowGraph.addNode());
        club_arcs.push_back(flowGraph.addArc(club_nodes.back(), end));
    }
    //Add connection arcs
    for (int s = 0; s < students.size(); s++) {
        vector<int> h1;
        vector<ListDigraph::Arc> h2;
        connection_arcs_d.push_back(h1);
        connection_arcs.push_back(h2);
        string name = students[s];
        
        
        vector<string> choices;
        //add in connection arcs
        for (int i = 0; i < NUM_CHOICES; i++) {
            bool c_ex = false;
            for (int c = 0; c < clubs.size(); c++) {
                if (data_table[s + 1][i + COL_CHOICE_START].compare(clubs[c]) == 0) {
                    bool exists = false;
                    
                    c_ex = true;
                    for (int j = 0; j < choices.size(); j++) {
                        if (choices[j].compare(clubs[c]) == 0) exists = true;
                    }
                    if (! exists) {
                        if (! (to_uppercase(clubs[c]).compare("STUDY HALL") == 0 || to_uppercase(clubs[c]).compare("STUDYHALL") == 0)) {
                            if (to_uppercase(clubs[c]).compare(to_uppercase(a_week_results[s])) != 0) {
                                connection_arcs[s].push_back(flowGraph.addArc(student_nodes[s], club_nodes[c]));
                                connection_arcs_d[s].push_back(c);
                                choices.push_back(clubs[c]);
                            }
                        }
                        break;
                    }
                }
            }
        }
        
        connection_arcs[s].push_back(flowGraph.addArc(student_nodes[s], club_nodes[studyHall]));
        connection_arcs_d[s].push_back(studyHall);
    }
    //Add upper and cost maps
    ListDigraph::ArcMap<int> upper_map(flowGraph);
    ListDigraph::ArcMap<double> cost_map(flowGraph);
    
    for (int i = 0; i < students.size(); i++) {
        upper_map[student_arcs[i]] = 1;
        cost_map[student_arcs[i]] = 0;
    }
    int count_1 = 0;
    for (int s = 0; s < students.size(); s++) {
        
        string name = students[s];
        vector<int> choices;
        bool choices_has_sh = false;
        
        //find connection arcs
        for (int i = 0; i < NUM_CHOICES; i++) {
            for (int c = 0; c < clubs.size(); c++) {
                if (data_table[s + 1][i + COL_CHOICE_START].compare(clubs[c]) == 0) {
                    bool exists = false;
                    for (int j = 0; j < choices.size(); j++) {
                        if (choices[j] == c) exists = true;
                    }
                    if (! exists) {
                        if (to_uppercase(clubs[c]).compare(to_uppercase(a_week_results[s])) != 0) {
                            choices.push_back(c);
                            if (to_uppercase(clubs[c]).compare("STUDY HALL") == 0 || to_uppercase(clubs[c]).compare("STUDYHALL") == 0) choices_has_sh = true;
                        }
                    }
                }
            }
        }
        if (! choices_has_sh) choices.push_back(studyHall);
        
        
        //find cost of connection arcs

        
        double cost = 0.0001;
        
        int grade = 0;
        if (to_uppercase(data_table[s + 1][COL_GRADE]).compare("FRESHMAN") == 0 || to_uppercase(data_table[s + 1][COL_GRADE]).compare("FRESHMEN") == 0) {
            grade = 9;
        }
        else if (to_uppercase(data_table[s + 1][COL_GRADE]).compare("SOPHOMORE") == 0) {
            grade = 10;
        }
        else if (to_uppercase(data_table[s + 1][COL_GRADE]).compare("JUNIOR") == 0) {
            grade = 11;
        }
        else if (to_uppercase(data_table[s + 1][COL_GRADE]).compare("SENIOR") == 0) {
            grade = 12;
        }
        for (int c = 0; c < choices.size(); c++) {
            double rnd = ((double) rand() / (RAND_MAX));
            double offset = 0;
            if (grade == 12) {
                rnd *= 0.02;
                offset = 0.01;
            }
            else if (grade == 11) {
                rnd *= 0.03;
                offset = 0.015;
            }
            else if (grade == 10) {
                rnd *= 0.04;
                offset = 0.02;
            }
            else if (grade == 9) {
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
    
    for (int i = 0; i < clubs.size(); i++) {
        //Default club size is 20 unless Allison emails a second time saying that she changed her mind
        //No max student count for study hall.
        if (to_uppercase(clubs[i]).compare("STUDY HALL") == 0 || to_uppercase(clubs[i]).compare("STUDYHALL") == 0) upper_map[club_arcs[i]] = students.size() * 10;
        else upper_map[club_arcs[i]] = CLUB_CAPACITY;
        cost_map[club_arcs[i]] = 0;
    }
    
    //use lemon to process flow
    NetworkSimplex<ListDigraph, int, double> flow(flowGraph);
    flow.upperMap(upper_map);
    flow.costMap(cost_map);
    flow.stSupply(start, end, students.size());
    NetworkSimplex<ListDigraph, int, double>::ProblemType pt = flow.run();
    
    if (pt == NetworkSimplex<ListDigraph>::INFEASIBLE || pt == NetworkSimplex<ListDigraph>::UNBOUNDED) cout << "[ERROR] Flow could not be calculated." << endl;
    
    ListDigraph::ArcMap<int> output_flow_map(flowGraph);
    flow.flowMap(output_flow_map);//output_flow_map now has the flow map so keep all the paths(arcs) and nodes in arrays to match data up again
    
    vector<int> totals;
    for (int i = 0; i < NUM_CHOICES; i++) totals.push_back(0);
    
    //Create flow data from clubs
    b_week_results = vector<string>();
    for (int i = 0; i < students.size(); i++) {
        for (int c = 0; c < connection_arcs[i].size(); c++) {
            int f = flow.flow(connection_arcs[i][c]);
            if (f == 1) {
                b_week_results.push_back(clubs[connection_arcs_d[i][c]]);
                totals[c]++;
                break;
            }
        }
    }
    
    
    cout << "Finished processing B week choice data." << endl;
}

string remove_unnecessary_chars(string str) {
    char char_array[str.length() + 1];
    int added = 0;
    for(int i = 0; i < str.length(); i++)
    {
        //0xc3 and 0xbc are hacks for that funny U in "Cru" because that thing is in UTF-8.  i hate you guys
        if (((int)str.at(i) <= 126 && (int)str.at(i) >= 32) || str.at(i) == (char)0xc3 || str.at(i) == (char)0xbc) {//Anything that isn't displayed is useless.
            char_array[added++] = str.at(i);
        }
    }
    
    char temp[added + 1];
    
    for (int i = 0; i < added; i++) {
        temp[i] = char_array[i];
    }
    temp[added] = 0;
    return string(temp);
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

void write_results_to_file(vector<string> students, string filePrefix) {
    
    cout << "Writing results to file " << filePrefix << current_file_name << "..." << endl;
    
    //output results to .tsv file
    ofstream f_out;
    f_out.open(current_path + "/" + filePrefix + current_file_name);
    f_out << "Name\tA Week\tB Week";
    for (int i = 0; i < students.size(); i++) {
        f_out << "\n" << students[i] << "\t" << a_week_results[i] << "\t" << b_week_results[i];
    }
    f_out.close();
    cout << "Finished writing results to file.\n\n\n" << endl;
}

void write_statistics_to_file(vector<string> students, string filePrefix) {
    
    cout << "Writing statistics to file " << filePrefix << current_file_name << "..." << endl;
    
    vector<int> total_count_a, total_count_b;
    
    for (int i = 0; i < NUM_CHOICES; i++) {
        total_count_a.push_back(0);
        total_count_b.push_back(0);
    }
    
    for (int i = 0; i < students.size(); i++) {
        int offset = 0;
        vector<string> choices;
        for (int c = 0; c < NUM_CHOICES; c++) {
            string ch = to_uppercase(data_table[i + 1][COL_CHOICE_START + c]);
            bool exists = false;
            for (int c1 = 0; c1 < choices.size(); c1++) {
                if (choices[c1].compare(ch) == 0) {
                    exists = true;
                    break;
                }
            }
            if (exists) {
                offset++;
                continue;
            }
            
            
            if (ch.compare(to_uppercase(a_week_results[i])) == 0) {
                total_count_a[c - offset]++;
                choices.push_back(ch);
                if (c - offset == 5) cout << "A: " << students[i] << endl;
                break;
            }
        }
        offset = 0;
        choices.clear();
        for (int c = 0; c < NUM_CHOICES; c++) {
            string ch = to_uppercase(data_table[i + 1][COL_CHOICE_START + c]);
            bool exists = false;
            for (int c1 = 0; c1 < choices.size(); c1++) {
                if (choices[c1].compare(ch) == 0) {
                    exists = true;
                    break;
                }
            }
            if (exists) {
                offset++;
                continue;
            }
            
            
            if (ch.compare(to_uppercase(b_week_results[i])) == 0) {
                total_count_b[c - offset]++;
                choices.push_back(ch);
                if (c - offset == 5) cout << "B: " << students[i] << endl;
                break;
            }
        }
    }
    
    //output results to .tsv file
    ofstream f_out;
    f_out.open(current_path + "/" + filePrefix + current_file_name);
    f_out << "Choice #\tA Week Students\tB Week Students";
    for (int i = 0; i < NUM_CHOICES; i++) {
        f_out << "\n" << i + 1 << "\t" << total_count_a[i] << "\t" << total_count_b[i];
    }
    f_out.close();
    cout << "Finished writing statistics to file.\n\n\n" << endl;
}
