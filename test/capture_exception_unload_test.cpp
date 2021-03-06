// Copyright (c) 2018 Emil Dotchevski
// Copyright (c) 2018 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/capture_exception.hpp>
#include <boost/leaf/try.hpp>
#include <boost/leaf/throw.hpp>
#include <boost/leaf/preload.hpp>
#include "boost/core/lightweight_test.hpp"

namespace leaf = boost::leaf;

template <int> struct info { int value; };

template <class F>
void test( F f_ )
{
	auto f = leaf::capture_exception<info<1>, info<2>, info<3>>( [=] { return f_(); } );

	{
		int c=0;
		leaf::try_(
			[&f]
			{
				return f();
			},
			[&c]( info<1> const & x )
			{
				BOOST_TEST(x.value==1);
				BOOST_TEST(c==0);
				c = 1;
			},
			[&c]
			{
				BOOST_TEST(c==0);
				c = 2;
			} );
		BOOST_TEST(c==1);
	}

	{
		int c=0;
		leaf::try_(
			[&f]
			{
				return f();
			},
			[&c]( info<2> const & x )
			{
				BOOST_TEST(x.value==2);
				BOOST_TEST(c==0);
				c = 1;
			},
			[&c]
			{
				BOOST_TEST(c==0);
				c = 2;
			} );
		BOOST_TEST(c==2);
	}

	{
		int r = leaf::try_(
			[&f]
			{
				(void) f(); return 0;
			},
			[ ]( info<1> const & x )
			{
				BOOST_TEST(x.value==1);
				return 1;
			},
			[ ]
			{
				return 2;
			} );
		BOOST_TEST(r==1);
	}

	{
		int r = leaf::try_(
			[&f]
			{
				(void) f(); return 0;
			},
			[ ]( info<2> const & x )
			{
				BOOST_TEST(x.value==2);
				return 1;
			},
			[ ]
			{
				return 2;
			} );
		BOOST_TEST(r==2);
	}

	{
		bool r = leaf::try_(
			[&f]
			{
				(void) f(); return true;
			},
			[ ]( info<1> const & x, info<2> const & )
			{
				return true;
			},
			[ ]( info<1> const & x, info<3> const & y )
			{
				BOOST_TEST(x.value==1);
				BOOST_TEST(y.value==3);
				return false;
			},
			[ ]( info<1> const & x )
			{
				return true;
			},
			[ ]
			{
				return true;
			} );
		BOOST_TEST(!r);
	}

	{
		bool r = leaf::try_(
			[&f]
			{
				(void) f(); return false;
			},
			[ ]( info<1> const & x, info<2> const & )
			{
				return false;
			},
			[ ]( info<1> const & x, info<3> const & y )
			{
				BOOST_TEST(x.value==1);
				BOOST_TEST(y.value==3);
				return true;
			},
			[ ]( info<1> const & x )
			{
				return false;
			},
			[ ]
			{
				return false;
			} );
		BOOST_TEST(r);
	}
}

int main()
{
	test(
		[ ]
		{
			throw leaf::exception( std::exception(), info<1>{1}, info<3>{3} ); // Derives from leaf::error_id
		} );

	test(
		[ ]
		{
			auto propagate = leaf::preload( info<1>{1}, info<3>{3} );
			throw std::exception(); // Does not derive from leaf::error_id
		} );

	return boost::report_errors();
}
