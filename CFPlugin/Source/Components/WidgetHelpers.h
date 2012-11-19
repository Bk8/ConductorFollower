#pragma once

#include <boost/utility.hpp>

template<typename MapType>
class WidgetInitializer : public boost::noncopyable
{
public:
	WidgetInitializer(MapType & map) : map_(map) {}
	WidgetInitializer(WidgetInitializer const & other) : map_(other.map_) {}

	template<typename PairType>
	void operator()(PairType & pair) const
	{
		boost::fusion::at_key<typename PairType::first_type>(map_).Initialize(pair.second);
	}

private:
	mutable MapType & map_;
};

template<typename MapType>
class WidgetUpdater : public boost::noncopyable
{
public:
	WidgetUpdater(MapType & map) : map_(map) {}
	WidgetUpdater(WidgetUpdater const & other) : map_(other.map_) {}

	template<typename PairType>
	void operator()(PairType & pair) const
	{
		boost::fusion::at_key<typename PairType::first_type>(map_).Update(pair.second);
	}

private:
	mutable MapType & map_;
};

template<typename TContainer, cf::Status::Level Level>
class WidgetCollector : public boost::noncopyable
{
public:
	WidgetCollector(TContainer & container) : container_(container) {}
	WidgetCollector(WidgetCollector const & other) : container_(other.container_) {}

	template<typename PairType>
	void operator()(PairType & pair) const
	{
		if (pair.second.level() > Level) { return; }
		container_.push_back(static_cast<Component *>(&pair.second));
	}

private:
	mutable TContainer & container_;
};

template<cf::Status::Level Level, typename TContainer>
WidgetCollector<TContainer, Level>
make_widget_collector(TContainer & container)
{
	return WidgetCollector<TContainer, Level>(container);
}
