/*
 Course Section: Fall 2023 CS 201
 Homework #: 1
 Instructions to compile the program: g++ -std=c++17 perftest.cpp
 Instructions to execute the program: ./a.out
*/

#include <iostream>
#include <iomanip>
#include <array>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <chrono>

// heapify a given element
template<class RandomIt>
constexpr void maxHeapify(RandomIt begin, RandomIt element, int heapSize) {
    // get element indexes
    int index = element - begin;
    int left = (2 * index);
    int right = left + 1;
    RandomIt largest = element;
    // left child is largest
    if (left < heapSize && *(begin + left) > *element) {
        largest = begin + left;
    }
    // right child is largest
    if (right < heapSize && *(begin + right) > *largest) {
        largest = begin + right;
    }
    // swap is necessary
    if (*largest != *element) {
        std::swap(*element, *largest);
        maxHeapify(begin, largest, heapSize);
    }
}

// method to build the max heap
template<class RandomIt>
constexpr void buildMaxHeap(RandomIt first, RandomIt last) {
    int heapSize = std::distance(first, last);
    for (int i = heapSize / 2; i >= 0; i--) {
        maxHeapify(first, first + i, heapSize);
    }
}

// heapsort implementation using iterators
template<class RandomIt>
constexpr void heapsort(RandomIt first, RandomIt last) {
    // exit early if one or zero elements
    int heapSize = std::distance(first, last);
    if (heapSize < 2) return;
    // build the max heap
    buildMaxHeap(first, last);
    // sort max heap
    for (int i = heapSize - 1; i >= 0; i--) {
        std::swap(*first, *(first + i));
        heapSize--;
        maxHeapify(first, first, heapSize);
    }
}

// partitioning for quicksort
template <typename RandomIt>
RandomIt partition(RandomIt first, RandomIt last) {
    // set up variables
    RandomIt pivot = first + (std::distance(first, last) / 2);
    RandomIt end = last - 1;
    RandomIt i = first - 1;
    // move pivot to end of array
    std::swap(*pivot, *end);
    // iterate through array
    for (RandomIt j = first; j < end; j++) {
        // swap elements
        if (*j <= *end) {
            i++;
            std::swap(*i, *j);
        }
    }
    i++;
    // perform final swap
    std::swap(*i, *end);
    // return value to partition array based on
    return i;
}

// quicksort implementation using iterators
template <typename RandomIt>
void quicksort(RandomIt first, RandomIt last) {
    // exit early if one or zero elements
    if (first < last) {
        // partition data
        RandomIt bound = partition(first, last);
        // recurisvely sort the partitions
        quicksort(first, bound);
        quicksort(bound + 1, last);
    }
}

void testPerformance(std::string algorithmName) {
    std::cout << "Testing performance with algorithm: " << algorithmName << std::endl;
    std::chrono::duration<double> timetaken[3][3];
	std::cout << "Problem Size\tTime Taken (seconds)" << std::endl;
	std::cout << "\t\tCase 1\t\tCase 2\t\tCase 3" << std::endl;
    std::cout << std::fixed;
    std::cout << std::setprecision(8);
	for (int size = 10; size <= 100000000; size *= 10) {
	
		int *a = new int[size];

		// repeat each case for three times 
		for (int i = 0; i < 3; i++) {
			std::generate(a, a+size, std::rand);

			/* Case 1: sorting random values */
			auto starttime = std::chrono::steady_clock::now();
			if (algorithmName == "heapsort") {
                heapsort(a, a+size);
            } else if (algorithmName == "quicksort") {
                quicksort(a, a+size);
            } else {
                std::sort(a, a+size);
            }
			auto endtime = std::chrono::steady_clock::now();
			timetaken[i][0] = endtime - starttime;

			/* Case 2: sorting data that is already sorted */
			starttime = std::chrono::steady_clock::now();
			if (algorithmName == "heapsort") {
                heapsort(a, a+size);
            } else if (algorithmName == "quicksort") {
                quicksort(a, a+size);
            } else {
                std::sort(a, a+size);
            }
			endtime = std::chrono::steady_clock::now();
			timetaken[i][1] = endtime - starttime;

			/* Case 3: sorting data that is in reverse sorted order */
			std::reverse(a, a+size);
			starttime = std::chrono::steady_clock::now();
			if (algorithmName == "heapsort") {
                heapsort(a, a+size);
            } else if (algorithmName == "quicksort") {
                quicksort(a, a+size);
            } else {
                std::sort(a, a+size);
            }
			endtime = std::chrono::steady_clock::now();
			timetaken[i][2] = endtime - starttime;
		}

		// print the average time of three runs
		std::cout << size << "\t";
        if (size < 10000000) std::cout << "\t";
		std::cout << (timetaken[0][0].count() + timetaken[1][0].count() + timetaken[2][0].count())/3.0 << "\t";
		std::cout << (timetaken[0][1].count() + timetaken[1][1].count() + timetaken[2][1].count())/3.0 << "\t";
		std::cout << (timetaken[0][2].count() + timetaken[1][2].count() + timetaken[2][2].count())/3.0 << std::endl;

		delete[] a;
	}
}
int main(int argc, char *argv[]) {
    //testPerformance("heapsort");
    testPerformance("quicksort");
    //testPerformance("std::sort");
	return 0;
}