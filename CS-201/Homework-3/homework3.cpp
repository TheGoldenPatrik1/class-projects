/*
 Course Section: Fall 2023 CS 201
 Homework #: 3
 Instructions to Compile: g++ -std=c++20 homework2.cpp
 Instructions to Execute: ./a.out <input file> <query file>
*/

#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <unordered_map>
#include <vector>
#include <chrono>

int main(int argc, char *argv[]) {
	// check for correct command-line arguments
	if (argc != 3) {
	   std::cout << "Usage: " << argv[0] << " <db file> <query file>" << std::endl;
	   std::exit(-1);
	}

    // start timers
    std::chrono::time_point<std::chrono::system_clock> timer1, timer2, timer3;
    std::chrono::duration<double> time1, time2, time3;
    timer1 = std::chrono::system_clock::now();

    // set up variables
	std::string line, name;
	std::regex delim("/");
    int recordCount = 0;

    // open input file
	std::ifstream dbfile(argv[1]);
	if (!dbfile.is_open()) {
	   std::cout << "Unable to open file: " << argv[1] << std::endl;
	   std::exit(-1);
	}
    
    // track time it takes to create and read into data structures
    timer2 = std::chrono::system_clock::now();

    // define our data structures
    std::unordered_map<std::string, std::vector<std::string> > actorMap;
    std::unordered_map<std::string, std::vector<std::string> > movieMap;

    // parse each line for tokens delimited by "/"
    // based on the code given on blackboard
	std::cout << "***Reading db file " << argv[1] << "***" << std::endl;
	while (std::getline(dbfile, line)) {
        recordCount++;
	    auto begin = std::sregex_token_iterator(line.begin(), line.end(), delim, -1);
	    auto end = std::sregex_token_iterator();
        std::string movie = *begin;
	    begin++;
	    for (std::sregex_token_iterator word = begin; word != end; word++) {
            std::string actor = *word;
            // insert movie to actor map
            if (actorMap.count(actor) == 0) actorMap[actor];
            actorMap.at(actor).push_back(movie);
            // insert actor to movie map
            if (movieMap.count(movie) == 0) movieMap[movie];
            movieMap.at(movie).push_back(actor);
	    }
	}

    // close input file
	dbfile.close();
	std::cout << "***Done reading db file " << argv[1] << "***" << std::endl;

    // calculate time taken to create and read-into data structures
    timer3 = std::chrono::system_clock::now();
    time1 = timer3 - timer2;

    // open query file
	std::ifstream queryfile(argv[2]);
	if (!queryfile.is_open()) {
	    std::cout << "Unable to open file: " << argv[2] << std::endl;
	    std::exit(-1);
	}

    // track time it takes to execute searches
    timer2 = std::chrono::system_clock::now();

    // iterate through query file
	std::cout << "***Reading query file " << argv[2] << "***" << std::endl << std::endl;
	while (std::getline(queryfile, name)) {
	    std::cout << "*** Checking for: " << name << "***" << std::endl;
        if (actorMap.count(name)) {
            // print all movies for given actor
            for (auto begin = actorMap.at(name).begin(); begin != actorMap.at(name).end(); begin++) {
                std::cout << *begin << std::endl;
            }
        } else if (movieMap.count(name)) {
            // print all actors for given movie
            for (auto begin = movieMap.at(name).begin(); begin != movieMap.at(name).end(); begin++) {
                std::cout << *begin << std::endl;
            }
        } else {
            std::cout << "Not Found" << std::endl;
        }
        std::cout << std::endl;
	}

    // close query file
	queryfile.close();
	std::cout << "***Done reading query file " << argv[2] << "***" << std::endl;

    // calculate time taken to execute searches
    timer3 = std::chrono::system_clock::now();
    time2 = timer3 - timer2;

    // calculate total program execution time
    time3 = timer3 - timer1;

    // print times
    std::cout << std::endl << "***Program results***" << std::endl;
    std::cout << "# of records:\t\t\t\t" << recordCount << std::endl;
    std::cout << "Time taken to create data structure:\t" << time1.count() << std::endl;
    std::cout << "Time taken to search:\t\t\t" << time2.count() << std::endl;
    std::cout << "Total time taken:\t\t\t" << time3.count() << std::endl;

	return 0;
}
