
//  (C) Copyright Edward Diener 2011
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#include "test_has_mem_fun.hpp"

int main()
  {
  
  // Function signature has type which does not exist
  
  BOOST_TTI_HAS_MEMBER_FUNCTION_GEN(sFunction)<NVType,AType::AnIntType,boost::mpl::vector<short,long,double> > aVar;
  
  return 0;

  }
