/*
 Course Section: Fall 2023 CS 201
 Homework #: 2
 Instructions to Compile: g++ homework2.cpp
 Instructions to Execute: ./a.out <input file> OR ./a.exe <input file>
*/

#include <string>
#include <iostream>
#include <fstream>
#include <chrono>
#include <map>
#include <regex>

using namespace std;

// compare int in descending order, followed by string in ascending order
bool comparison(pair<string, int> p1, pair<string, int> p2) {
    if (p1.second == p2.second) {
        return p1.first < p2.first;
    }
    return p1.second > p2.second;
}

int main(int argc, char **argv) {
    // start timer
    chrono::time_point<chrono::system_clock> start, end;
    start = chrono::system_clock::now();

    // verify user input
    if (argc < 2) {
        cout << "Please specify an input file" << endl;
        return 1;
    }
    
    // verify input file
    string filename = argv[1];
    ifstream file(filename);
    if (!file) {
        cout << "Unable to open " << filename << endl;
        return 2;
    }

    // read in words from file
    // based on blackboard example
    map<string, int> dataMap;
    string text;
    int wordCount = 0;
    const regex delim("\\s+");
    while (file >> text) {
        sregex_token_iterator begin = sregex_token_iterator(text.begin(), text.end(), delim, -1);
        sregex_token_iterator end = sregex_token_iterator();
        for (std::sregex_token_iterator word = begin; word != end; word++) {
            // add words to map
            wordCount++;
            if (dataMap[*word]) {
                dataMap[*word] += 1;
            } else {
                dataMap[*word] = 1;
            }
        }
    }
    file.close();

    // convert map to vector
    vector< pair<string, int> > dataVec;
    for (map<string, int>::const_iterator it = dataMap.begin(); it != dataMap.end(); it++) {
        dataVec.push_back(make_pair(it->first, it->second));
    }

    // sort vector and output to file
    sort(dataVec.begin(), dataVec.end(), comparison);
    ofstream outFile("output.txt");
    for (int i = 0; i < dataVec.size(); i++) {
        outFile << dataVec.at(i).first << ": " << dataVec.at(i).second << endl;
    }
    outFile.close();

    // calculate program duration
    end = chrono::system_clock::now();
    chrono::duration<double> programDuration = end - start;
    cout << "Total number of words: " << wordCount << endl;
    cout << "Time taken: " << programDuration.count() << "s" << endl;
    return 0;
}