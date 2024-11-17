/*
 Course Section: Fall 2023 CS 201
 Homework #: 1
 Instructions to compile the program: g++ -std=c++17 functest.cpp
 Instructions to execute the program: ./a.out
*/

#include <iostream>
#include <array>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>

// print data given start and end values
template<class RandomIt>
void print(RandomIt start, RandomIt end) {
	while (start != end) {
		std::cout << *start << " ";
		++start;
	}
	std::cout << std::endl;
}

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

int main(int argc, char *argv[]) {
	int a0[] = {56, 23, 11, 64, 43};
	std::array<int, 5> a1 = {5, 4, 3, 2, 1};
	std::array<std::string, 5> a2 = {"lion", "dog", "cat", "fox", "pig"};
	std::vector<double> v = {4.2, 3.1, 5.6, 2.8, 1.9};

    // heapsort
    std::cout << "heapsort:" << std::endl;

    heapsort(&a0[0], &a0[5]);
	print(&a0[0], &a0[5]);
    
    heapsort(a0, a0+5);
	print(a0, a0+5);

	heapsort(a1.begin(), a1.end());
	print(a1.begin(), a1.end());

	heapsort(a2.begin(), a2.end());
	print(a2.begin(), a2.end());

	std::reverse(a2.begin(), a2.end());
	print(a2.begin(), a2.end());

	heapsort(a2.begin(), a2.end());
	print(a2.begin(), a2.end());

	heapsort(v.begin(), v.end());
	print(v.begin(), v.end());

    // quicksort
    std::cout << std::endl << "quicksort:" << std::endl;
    
    quicksort(&a0[0], &a0[5]);
	print(&a0[0], &a0[5]);
    
    quicksort(a0, a0+5);
	print(a0, a0+5);

	quicksort(a1.begin(), a1.end());
	print(a1.begin(), a1.end());

	quicksort(a2.begin(), a2.end());
	print(a2.begin(), a2.end());

	std::reverse(a2.begin(), a2.end());
	print(a2.begin(), a2.end());

	quicksort(a2.begin(), a2.end());
	print(a2.begin(), a2.end());

	quicksort(v.begin(), v.end());
	print(v.begin(), v.end());

	return 0;
}
