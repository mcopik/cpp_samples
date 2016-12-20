#include <numeric>
#include <chrono>
#include <blaze/Blaze.h>

int main()
{
	typedef std::chrono::high_resolution_clock clock_t;
	typedef blaze::DynamicMatrix<double, blaze::rowMajor> mat_t;
	const int rows = 400, cols = 300;

	mat_t A(rows, cols), B(rows, cols);

	// init	
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(0, 1);
	A = blaze::forEach(A, [&](double) { return dis(gen); });
	B = blaze::forEach(B, [&](double) { return dis(gen); });
	
	// Don't assume return type - no computation
	auto clock_begin = clock_t::now();
	auto result = A * blaze::trans(B);
	auto clock_end = clock_t::now();
	// Time
	std::cout << "Time: " << std::chrono::duration_cast<std::chrono::microseconds>(clock_end - clock_begin).count() << '\n';

	// Force evaluation - no computation
	clock_begin = clock_t::now();
	auto result2 = blaze::eval(A * blaze::trans(B));
	clock_end = clock_t::now();
	// Time
	std::cout << "Time: " << std::chrono::duration_cast<std::chrono::microseconds>(clock_end - clock_begin).count() << '\n';

	// Force evaluation with a temporary?
	clock_begin = clock_t::now();
	blaze::eval(A * blaze::trans(B));
	clock_end = clock_t::now();
	// Time
	std::cout << "Time: " << std::chrono::duration_cast<std::chrono::microseconds>(clock_end - clock_begin).count() << '\n';

	// Assume return type
	clock_begin = clock_t::now();
	mat_t result3 = A * blaze::trans(B);
	clock_end = clock_t::now();
	// Time
	std::cout << "Time: " << std::chrono::duration_cast<std::chrono::microseconds>(clock_end - clock_begin).count() << std::endl;

	return 0;
}
