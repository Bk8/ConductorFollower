#pragma once

#include <boost/format.hpp>

#include "Data/ParseException.h"

namespace cf {
namespace Data {

template<typename Iterator>
struct error_handler_impl
{
	template <typename, typename, typename>
	struct result { typedef void type; };

	void operator()(qi::info const & what, Iterator const & whereBegin, Iterator const & whereEnd) const
	{
		auto where = std::string(whereBegin, whereEnd);
		if (where.length() > 100) {
			where.resize(100);
			where += "...";
		}

		auto msg = boost::format("Parse error, expecting %1%, right before:\n %2%")
			% what % where;
		throw ParseException(msg.str());
	}
};

} // namespace Data
} // namespace cf
