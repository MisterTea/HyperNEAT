#ifndef HCUBE_BOOST_H_INCLUDED
#define HCUBE_BOOST_H_INCLUDED

#include "boost/multi_array.hpp"

#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_smallint.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/generator_iterator.hpp>

#include<boost/tokenizer.hpp>

#include <boost/algorithm/string.hpp>

#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/io.hpp>

#include <boost/variant.hpp>

#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/xtime.hpp>
#include <boost/bind.hpp>

#include <boost/shared_ptr.hpp>

#include <boost/pool/object_pool.hpp>
#include <boost/pool/singleton_pool.hpp>

#include <boost/progress.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

typedef boost::minstd_rand base_generator_type;
// Define a uniform random number distribution of integer values between
// 1 and 6 inclusive.
typedef boost::uniform_int<> int_distribution_type;
typedef boost::variate_generator<base_generator_type&, int_distribution_type> int_gen_type;

using namespace boost;

#endif // HCUBE_BOOST_H_INCLUDED
