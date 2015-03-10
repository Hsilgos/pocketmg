#pragma once

#include "utils/singleton.h"
#include "utils/algorithms_ex.h"

//#include <boost/function.hpp>
#include <map>
#include <vector>
#include <sstream>
#include <stdexcept>

namespace tool
{
	template <class FunSignature>
	class function
	{
	};
}

namespace pattern
{
	template<
		class Signature, 
		class IdType>
	class Factory
	{
	public:
		typedef boost::function<Signature> ConcreteFactory;
	private:
		typedef Factory<Signature, IdType> ThisFactory;

		typedef std::map<IdType, ConcreteFactory> FactoryCollect;
		FactoryCollect creators_;

	protected:
		ConcreteFactory doGet(const IdType &id)
		{
			typename FactoryCollect::const_iterator it = creators_.find(id);
			if( it != creators_.end() )
				return it->second;

			std::stringstream error_info;

			error_info << "Invalid concrete factory " << id;

			throw std::invalid_argument(error_info.str());
		}

		void doGetKeylist(std::vector<IdType> &list)
		{
			list.reserve(creators_.size());
			utils::collectKeys(creators_, std::back_inserter(list));
		}
	public:
		// Returns concrete factory
		ConcreteFactory get(const IdType &id)
		{
			return doGet(id);
		}

		std::vector<IdType> keyList()
		{
			std::vector<IdType> list;
			doGetKeylist(list);
			return list;
		}

		void regFactory(const IdType &id, ConcreteFactory factory)
		{
			creators_[id] = factory;
		}
	};

	template<
		class Signature, 
		class IdType>
	class SingletonFactory: 
		public utils::SingletonStatic< SingletonFactory<Signature, IdType> >,
		public Factory<Signature, IdType>
	{
		typedef Factory<Signature, IdType> BaseFactory;
		typedef typename BaseFactory::ConcreteFactory ConcreteFactory;
		typedef SingletonFactory<Signature, IdType> ThisFactory;
	public:
		static ConcreteFactory get(const IdType &id)
		{
			return ThisFactory::getInstance().doGet(id);
		}

		static std::vector<IdType> keyList()
		{
			std::vector<IdType> list;
			ThisFactory::getInstance().doGetKeylist(list);
			return list;
		}
	};

#define AUTO_REGISTER_FACTORY( fact_type, type_id ,concrete_factory ) \
		namespace																												\
		{																														\
			struct BOOST_JOIN(FactoryRegistrar, __LINE__)																		\
			{																													\
				BOOST_JOIN(FactoryRegistrar, __LINE__)()																		\
				{																												\
					fact_type::getInstance().regFactory(type_id, concrete_factory);												\
				}																												\
			};																													\
			static BOOST_JOIN(FactoryRegistrar, __LINE__) BOOST_JOIN(__global_factory_registrar__, __LINE__ );					\
		}																														

}

