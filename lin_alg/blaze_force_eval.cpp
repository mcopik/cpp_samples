#include <numeric>
#include <chrono>
#include <blaze/Blaze.h>

// inf norm of matrix
//template<typename Matrix>
double norm(const blaze::DynamicMatrix<double, blaze::rowMajor> & m)
{
	//typedef typename Matrix::ElementType FloatType;
	typedef double FloatType;	
	FloatType res = 0.0;
	//No generic iterator + padding in matrix -> no STL algorithms
	for(size_t i = 0; i < m.rows(); ++i) {
		FloatType sum = std::accumulate(m.begin(i), m.end(i), 0.0,
			[](FloatType sum, FloatType elem) {
				return sum + std::abs(elem);
			});
		res = std::max(res, sum);
	}

	return res;
}

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
