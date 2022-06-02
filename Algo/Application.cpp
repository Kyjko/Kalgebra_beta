#include "Matrix.hpp"
#include <iostream>

#define FIB_DEMO

int main(void) {

#ifdef DEMO
	int nums[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};

	Matrix<int> A(nums, 3, 3);

	Matrix<int> B(nums, 3, 3);
	A *= B;

	std::cout << A;

	Matrix<int> C = A;
	std::cout << C << "\n" << A;

	std::cout << "----------------------------\n";

	int nums2[] = {5, 6, 3, 6, 5, 7, 8, 9, 1};
	int nums3[] = { 50, 60, 30, 60, 50, 70, 80, 90, 10 };
	Matrix<int> M1(nums2, 3, 3);
	Matrix<int> M2(nums3, 3, 3);
	Matrix<int> M3(nums2, 3, 3);
	M1 -= M2;
	M3 = A + M1;
	M3 *= M1;

	//M2 = M3 * M1;

	std::cout << M1 << M2 << M3;
#endif

#ifdef FIB_DEMO
	// calculate fibonacci numbers
	int nums[] = { 1, 1, 6, 1 };
	Matrix<int> FIB(nums, 2, 2);

	std::cout << FIB;

	int nums2[] = { 1, 2 };
	Vector<int> starting_state(nums2, 2);

	std::cout << starting_state;

	auto endstate = FIB * starting_state;
	std::cout << endstate;

	for (size_t i = 0; i < 50; ++i) {
		auto next = FIB * starting_state;
		starting_state = next;
		std::cout << starting_state;
		
	}

	std::cout << "50. fibonacci number: " << starting_state;

#endif
}