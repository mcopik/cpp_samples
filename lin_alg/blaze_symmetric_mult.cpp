#include <numeric>
#include <chrono>
#include <blaze/Blaze.h>

// According to docs we can save result of symmetric * regular matrix multiplication to a symmetric matrix
// https://bitbucket.org/blaze-lib/blaze/wiki/Symmetric%20Matrices#!the-elements-of-a-dense-symmetric-matrix-are-always-default-initialized
// I don't think that's the case - result of multiplication will not be symmetric most of the time

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
	blaze::StaticMatrix<double,3UL,3UL> init_A{
		{ 1.0, 2.0, 3.0 },
    	{ 2.0, 2.0, 0.0 },
		{ 3.0, 0.0, 1.0 }
	};
	blaze::SymmetricMatrix<blaze::DynamicMatrix<double, blaze::rowMajor>> A(init_A);

	blaze::DynamicMatrix<double, blaze::rowMajor> B{
    	{ 0.7482, 0.2290, 0.8258},
    	{ 0.4505, 0.9133, 0.5383},
    	{ 0.0838, 0.1524, 0.9961}
	};

	blaze::DynamicMatrix<double, blaze::rowMajor> expected{
    	{ 1.9007, 2.5128, 4.8909},
    	{ 2.3974, 2.2846, 2.7283},
    	{ 2.3283, 0.8393, 3.4736}
	};

	blaze::SymmetricMatrix< blaze::DynamicMatrix<double, blaze::rowMajor> > result( 3 );
	
	// Don't assume return type
	auto clock_begin = clock_t::now();
	// Here we can expect an exception
	//result = init_A * B;
	result += A - B;
	auto clock_end = clock_t::now();
	// Check corectness
	std::cout << "Diff norm: " << norm(result - expected) << std::endl;
	std::cout << "Time: " << std::chrono::duration_cast<std::chrono::microseconds>(clock_end - clock_begin).count();

	return 0;
}
