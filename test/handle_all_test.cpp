// Copyright (c) 2018 Emil Dotchevski
// Copyright (c) 2018 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/handle.hpp>
#include "boost/core/lightweight_test.hpp"

namespace leaf = boost::leaf;

template <int> struct info { int value; };

enum class error_code
{
	ok,
	error1,
	error2,
	error3
};
namespace boost { namespace leaf {
	template <> struct is_error_type<error_code>: std::true_type { };
} }

struct error_codes_ { error_code value; };

template <class R>
leaf::result<R> f( error_code ec )
{
	if( ec==error_code::ok )
		return R(42);
	else
		return leaf::new_error(ec,error_codes_{ec},info<1>{1},info<2>{2},info<3>{3});
}

int main()
{
	// void, handle_all (success)
	{
		int c=0;
		leaf::handle_all(
			[&c]() -> leaf::result<void>
			{
				LEAF_AUTO(answer, f<int>(error_code::ok));
				c = answer;
				return { };
			},
			[&c]
			{
				BOOST_TEST(c==0);
				c = 1;
			} );
		BOOST_TEST(c==42);
	}

	// void, handle_all (failure)
	{
		int c=0;
		leaf::handle_all(
			[&c]() -> leaf::result<void>
			{
				LEAF_AUTO(answer, f<int>(error_code::error1));
				c = answer;
				return { };
			},
			[&c]( error_code ec, info<1> const & x, info<2> y )
			{
				BOOST_TEST(ec==error_code::error1);
				BOOST_TEST(x.value==1);
				BOOST_TEST(y.value==2);
				BOOST_TEST(c==0);
				c = 1;
			},
			[&c]()
			{
				BOOST_TEST(c==0);
				c = 2;
			} );
		BOOST_TEST(c==1);
	}

	// void, handle_all (failure), match enum (single enum value)
	{
		int c=0;
		leaf::handle_all(
			[&c]() -> leaf::result<void>
			{
				LEAF_AUTO(answer, f<int>(error_code::error1));
				c = answer;
				return { };
			},
			[&c]( leaf::match<error_code,error_code::error2> )
			{
				BOOST_TEST(c==0);
				c = 1;
			},
			[&c]( leaf::match<error_code,error_code::error1> ec, info<1> const & x, info<2> y )
			{
				BOOST_TEST(ec.value==error_code::error1);
				BOOST_TEST(x.value==1);
				BOOST_TEST(y.value==2);
				BOOST_TEST(c==0);
				c = 2;
			},
			[&c]()
			{
				BOOST_TEST(c==0);
				c = 3;
			} );
		BOOST_TEST(c==2);
	}

	// void, handle_all (failure), match enum (multiple enum values)
	{
		int c=0;
		leaf::handle_all(
			[&c]() -> leaf::result<void>
			{
				LEAF_AUTO(answer, f<int>(error_code::error1));
				c = answer;
				return { };
			},
			[&c]( leaf::match<error_code,error_code::error2> )
			{
				BOOST_TEST(c==0);
				c = 1;
			},
			[&c]( leaf::match<error_code,error_code::error2,error_code::error1> ec, info<1> const & x, info<2> y )
			{
				BOOST_TEST(ec.value==error_code::error1);
				BOOST_TEST(x.value==1);
				BOOST_TEST(y.value==2);
				BOOST_TEST(c==0);
				c = 2;
			},
			[&c]()
			{
				BOOST_TEST(c==0);
				c = 3;
			} );
		BOOST_TEST(c==2);
	}

	// void, handle_all (failure), match value (single value)
	{
		int c=0;
		leaf::handle_all(
			[&c]() -> leaf::result<void>
			{
				LEAF_AUTO(answer, f<int>(error_code::error1));
				c = answer;
				return { };
			},
			[&c]( leaf::match<error_codes_,error_code::error2> )
			{
				BOOST_TEST(c==0);
				c = 1;
			},
			[&c]( leaf::match<error_codes_,error_code::error1> ec, info<1> const & x, info<2> y )
			{
				BOOST_TEST(ec.value==error_code::error1);
				BOOST_TEST(x.value==1);
				BOOST_TEST(y.value==2);
				BOOST_TEST(c==0);
				c = 2;
			},
			[&c]()
			{
				BOOST_TEST(c==0);
				c = 3;
			} );
		BOOST_TEST(c==2);
	}

	// void, handle_all (failure), match value (multiple values)
	{
		int c=0;
		leaf::handle_all(
			[&c]() -> leaf::result<void>
			{
				LEAF_AUTO(answer, f<int>(error_code::error1));
				c = answer;
				return { };
			},
			[&c]( leaf::match<error_codes_,error_code::error2> )
			{
				BOOST_TEST(c==0);
				c = 1;
			},
			[&c]( leaf::match<error_codes_,error_code::error2,error_code::error1> ec, info<1> const & x, info<2> y )
			{
				BOOST_TEST(ec.value==error_code::error1);
				BOOST_TEST(x.value==1);
				BOOST_TEST(y.value==2);
				BOOST_TEST(c==0);
				c = 2;
			},
			[&c]()
			{
				BOOST_TEST(c==0);
				c = 3;
			} );
		BOOST_TEST(c==2);
	}

	//////////////////////////////////////

	// int, handle_all (success)
	{
		int r = leaf::handle_all(
			[ ]() -> leaf::result<int>
			{
				LEAF_AUTO(answer, f<int>(error_code::ok));
				return answer;
			},
			[ ]
			{
				return 1;
			} );
		BOOST_TEST(r==42);
	}

	// int, handle_all (failure)
	{
		int r = leaf::handle_all(
			[ ]() -> leaf::result<int>
			{
				LEAF_AUTO(answer, f<int>(error_code::error1));
				return answer;
			},
			[ ]( error_code ec, info<1> const & x, info<2> y )
			{
				BOOST_TEST(ec==error_code::error1);
				BOOST_TEST(x.value==1);
				BOOST_TEST(y.value==2);
				return 1;
			},
			[ ]
			{
				return 2;
			} );
		BOOST_TEST(r==1);
	}

	// int, handle_all (failure), match enum (single enum value)
	{
		int r = leaf::handle_all(
			[ ]() -> leaf::result<int>
			{
				LEAF_AUTO(answer, f<int>(error_code::error1));
				return answer;
			},
			[ ]( leaf::match<error_code,error_code::error2> )
			{
				return 1;
			},
			[ ]( leaf::match<error_code,error_code::error1> ec, info<1> const & x, info<2> y )
			{
				BOOST_TEST(ec.value==error_code::error1);
				BOOST_TEST(x.value==1);
				BOOST_TEST(y.value==2);
				return 2;
			},
			[ ]
			{
				return 3;
			} );
		BOOST_TEST(r==2);
	}

	// int, handle_all (failure), match enum (multiple enum values)
	{
		int r = leaf::handle_all(
			[ ]() -> leaf::result<int>
			{
				LEAF_AUTO(answer, f<int>(error_code::error1));
				return answer;
			},
			[ ]( leaf::match<error_code,error_code::error2> )
			{
				return 1;
			},
			[ ]( leaf::match<error_code,error_code::error2,error_code::error1> ec, info<1> const & x, info<2> y )
			{
				BOOST_TEST(ec.value==error_code::error1);
				BOOST_TEST(x.value==1);
				BOOST_TEST(y.value==2);
				return 2;
			},
			[ ]
			{
				return 3;
			} );
		BOOST_TEST(r==2);
	}

	// int, handle_all (failure), match value (single value)
	{
		int r = leaf::handle_all(
			[ ]() -> leaf::result<int>
			{
				LEAF_AUTO(answer, f<int>(error_code::error1));
				return answer;
			},
			[ ]( leaf::match<error_codes_,error_code::error2> )
			{
				return 1;
			},
			[ ]( leaf::match<error_codes_,error_code::error1> ec, info<1> const & x, info<2> y )
			{
				BOOST_TEST(ec.value==error_code::error1);
				BOOST_TEST(x.value==1);
				BOOST_TEST(y.value==2);
				return 2;
			},
			[ ]
			{
				return 3;
			} );
		BOOST_TEST(r==2);
	}

	// int, handle_all (failure), match value (multiple values)
	{
		int r = leaf::handle_all(
			[ ]() -> leaf::result<int>
			{
				LEAF_AUTO(answer, f<int>(error_code::error1));
				return answer;
			},
			[ ]( leaf::match<error_codes_,error_code::error2> )
			{
				return 1;
			},
			[ ]( leaf::match<error_codes_,error_code::error2,error_code::error1> ec, info<1> const & x, info<2> y )
			{
				BOOST_TEST(ec.value==error_code::error1);
				BOOST_TEST(x.value==1);
				BOOST_TEST(y.value==2);
				return 2;
			},
			[ ]
			{
				return 3;
			} );
		BOOST_TEST(r==2);
	}

	return boost::report_errors();
}
