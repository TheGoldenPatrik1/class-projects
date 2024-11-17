/*
 Course Section: Fall 2023 CS 201
 Homework #: 4
 Instructions to Compile: g++ -std=c++20 GraphDemo.cpp
 Instructions to Execute: ./a.out <input file>
*/

#include <iostream>
#include <fstream>
#include <string>
#include <utility>

#include "Graph.hpp"

int main(int argc, char *argv[]) {
	// check for correct command-line arguments
	if (argc != 2) {
	   std::cout << "Usage: " << argv[0] << " <filename>" << std::endl;
	   std::exit(-1);
	}

	std::string line;
	std::ifstream infile(argv[1]);
	// check if file can be opened for reading
	if (!infile.is_open()) {
	   std::cout << "Unable to open file: " << argv[1] << std::endl;
	   std::exit(-1);
	}

	getline(infile, line); // read # of vertices
	int V = stoi(line);
	getline(infile, line); // read # of edges
	int E = stoi(line);

	int i = 0;
	std::pair<int, int> *edges = new std::pair<int, int>[E];
	// read each line and create an array of edges
	while (std::getline(infile, line)) {
		size_t pos = line.find(' ', 0);
		std::string u = line.substr(0, pos);
		std::string v = line.substr(pos+1,line.length());
		edges[i++] = std::make_pair(std::stol(u),std::stol(v));
	}

	// check if number of edges read match E
	if (E != i) {
		std::cout << "No. of edges do not match, exiting" << std::endl;
		std::exit(-1);
	}

	// create a graph G = (V, E)
	Graph G(V, E, edges);
	// display G as an array of vertices
	std::cout << G.toString();

	// read source vertex for BFS
	int src;
	std::cout << std::endl << "Enter source vertex: ";
	std::cin >> src;
	std::cout << std::endl << "Results from BFS:" << std::endl;
	
	// test BFS
	G.bfs(src);
	for (int i = 0; i < V; ++i) {
		if (i != src) {
		   std::cout << src << " to " << i << ": ";
		   G.print_path(src, i);
		   std::cout << std::endl;
		}
	}
	std::cout << std::endl;

	// test DFS
	std::cout << "Results from DFS:\n";
	G.dfs();

	return 0;
}
