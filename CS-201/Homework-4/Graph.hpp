/*
 Course Section: Fall 2023 CS 201
 Homework #: 4
 Instructions to Compile and Execute: use GraphDemo.cpp driver class
*/

#ifndef _GRAPH_HPP_
#define _GRAPH_HPP_

#include <iostream>
#include <string>
#include <sstream>
#include <list>
#include <limits>
#include <queue>

class Vertex {
public:
	bool visited;
	int distance;
	int previous;
	int finish;
	std::list<int> adj;
};

class Graph {
public:
	Graph(int V, int E, std::pair<int, int> *edges) {
		_V = V;
		_E = E;
		vertices = new Vertex[_V];
		for (int i = 0; i < _V; i++) {
                    vertices[i].visited = false;
                    vertices[i].distance = INFINITY;
                    vertices[i].previous = -1;
		}
		for (int i = 0; i < _E; i++) {
		    addEdge(edges[i].first, edges[i].second);
		}
	}

	virtual ~Graph() {
		for (int i=0; i<_V; ++i) {
		    auto adj = vertices[i].adj;
		    adj.clear(); // clear list
		}

		delete[] vertices; // delete array of vertices
	}

	int V() {
		return _V;
	}

	int E() {
		return _E;
	}

	void addEdge(int u, int v) {
		vertices[u].adj.push_back(v);
	}

	std::list<int> getEdges(int u) {
		return vertices[u].adj;
	}

	int degree(int u) {
		return vertices[u].adj.size();
	}

	void bfs(int s) {
		// set vertex values back to defaults
        for (int i = 0; i < _V; i++) {
            vertices[i].visited = false;
            vertices[i].distance = INFINITY;
            vertices[i].previous = -1;
        }
		// set up values for source vertex
		vertices[s].visited = true;
        vertices[s].distance = 0;
        vertices[s].previous = -1;
		// initialize queue with source vertex
        std::queue<int> theta;
        theta.push(s);
		// iterate through queue
        while (!theta.empty()) {
			// fetch item from queue
            int u = theta.front();
            theta.pop();
			// iterate through adjacency list
            for (const auto& l : vertices[u].adj) {
				// if not already visited, add it to queue
                if (!vertices[l].visited) {
                    vertices[l].visited = true;
                    vertices[l].distance = vertices[u].distance + 1;
                    vertices[l].previous = u;
                    theta.push(l);
                }
            }
        }
	}

	void dfs() {
		// set vertex values back to defaults
        for (int i = 0; i < _V; i++) {
            vertices[i].visited = false;
            vertices[i].previous = -1;
        }
		// clear time value
        time = 0;
		// dfs visit each vertex
        for (int i = 0; i < _V; i++) {
            if (!vertices[i].visited) dfs_visit(i);	    
		}
	}

	void dfs_visit(int u) {
		// set up vertex values
        time++;
        vertices[u].distance = time;
        vertices[u].visited = true;
		// iterate through adjacency list
        for (const auto& l : vertices[u].adj) {
            if (!vertices[l].visited) {
                vertices[l].previous = u;
                dfs_visit(l);
            }
        }
		// print vertex and color it black
		std::cout << u << " ";
        time++;
        vertices[u].finish = time;
	}

	void print_path(int s, int v) {
		if (v == s)
		   std::cout << s;
		else if (vertices[v].previous == -1)
		   std::cout << "not connected";
		else {
		   print_path(s,vertices[v].previous);
		   std::cout << "->" << v;
		}
	}

	std::string toString() {
		std::stringbuf buffer;
		std::ostream os(&buffer);
		os << "Vertices = " << _V << ", Edges = " << _E << std::endl;
		for (int i = 0; i < _V; ++i) {
		    os << i << "(" << degree(i) << "): ";
		    for (const auto& l : vertices[i].adj) 
			os << l << " ";
		    os << std::endl;
		}

		return buffer.str();
	}
private:
	int _V; // no. of vertices
	int _E; // no. of edges
    int time;
	Vertex *vertices; // array of vertices
};

#endif
