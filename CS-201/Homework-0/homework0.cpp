/*
 Course Section: Fall 2023 CS 201
 Homework #: 0
 Instructions to Compile: g++ homework0.cpp
 Instructions to Execute: ./a.out <input file> OR ./a.exe <input file>
*/

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>

using namespace std;

class Movie {
    private:
        string name;
        int year;
        vector<string> actors;
        // parse input string for given movie
        void parseInput(string input) {
            size_t pos = 0;
            string token;
            bool isFirst = true;
            while ((pos = input.find("/")) != string::npos) {
                token = input.substr(0, pos);
                // handle the name & year
                if (isFirst) {
                    if (token.find(",I)") != std::string::npos) {
                        name = token.substr(0, token.length() - 9);
                        year = stoi(token.substr(token.length() - 7, 4));
                    } else if (token.find(",II)") != std::string::npos) {
                        name = token.substr(0, token.length() - 10);
                        year = stoi(token.substr(token.length() - 8, 4));
                    } else {
                        name = token.substr(0, token.length() - 7);
                        year = stoi(token.substr(token.length() - 5, 4));
                    }
                    isFirst = false;
                }
                // handle the actors
                else {
                    actors.push_back(token);
                }
                input.erase(0, pos + 1);
            }
            actors.push_back(input);
        }
    public:
        Movie(string input) {
            parseInput(input);
        }
        string getName() {
            return name;
        }
        int getYear() {
            return year;
        }
        void print(ostream &os) {
            os << name << " (" << year << ")/";
            for (int i = 0; i < actors.size(); i++) {
                os << actors.at(i);
                if (i + 1 != actors.size()) os << "/";
            }
            os << endl;
        }
};

bool compareByName(Movie* movie1, Movie* movie2) {
    if (movie1->getName() == movie2->getName()) {
        return movie1->getYear() < movie2->getYear();
    }
    return movie1->getName() < movie2->getName();
}

bool compareByYear(Movie* movie1, Movie* movie2) {
    if (movie1->getYear() == movie2->getYear()) {
        return movie1->getName() < movie2->getName();
    }
    return movie1->getYear() < movie2->getYear();
}

int main(int argc, char **argv) {
    // start timer
    chrono::time_point<chrono::system_clock> start, end, breakPoint1, breakPoint2;
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

    // read in movies
    vector<Movie*> movies;
    string line;
    while (getline(file, line)) {
        Movie* movie = new Movie(line);
        movies.push_back(movie);
    }
    file.close();

    // calculate time to create collection
    breakPoint1 = chrono::system_clock::now();
    chrono::duration<double> programDuration = breakPoint1 - start;
    cout << "Time taken to create the collection: " << programDuration.count() << "s" << endl;

    // sort movies based on name & output to file
    sort(movies.begin(), movies.end(), compareByName);
    ofstream outFile1(filename.substr(0, 7) + "ByName.txt");
    for (int i = 0; i < movies.size(); i++) movies.at(i)->print(outFile1);
    outFile1.close();

    // calculate time to sort based on name
    breakPoint2 = chrono::system_clock::now();
    programDuration = breakPoint2 - breakPoint1;
    cout << "Time taken to sort based on movie name: " << programDuration.count() << "s" << endl;

    // sort movies based on year & output to file
    sort(movies.begin(), movies.end(), compareByYear);
    ofstream outFile2(filename.substr(0, 7) + "ByYear.txt");
    for (int i = 0; i < movies.size(); i++) movies.at(i)->print(outFile2);
    outFile2.close();

    // calculate program duration
    end = chrono::system_clock::now();
    programDuration = end - breakPoint2;
    cout << "Time taken to sort based on movie year: " << programDuration.count() << "s" << endl;
    programDuration = end - start;
    cout << "Total time taken: " << programDuration.count() << "s" << endl;
    return 0;
}