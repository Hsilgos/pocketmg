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
		FactoryCollect mCreators;

	protected:
		ConcreteFactory doGet(const IdType &aId)
		{
			typename FactoryCollect::const_iterator it = mCreators.find(aId);
			if( it != mCreators.end() )
				return it->second;

			std::stringstream tErrorInfo;

			tErrorInfo << "Invalid concrete factory " << aId;

			throw std::invalid_argument(tErrorInfo.str());
		}

		void doGetKeylist(std::vector<IdType> &aList)
		{
			aList.reserve(mCreators.size());
			utils::collectKeys(mCreators, std::back_inserter(aList));
		}
	public:
		// Returns concrete factory
		ConcreteFactory get(const IdType &aId)
		{
			return doGet(aId);
		}

		std::vector<IdType> keyList()
		{
			std::vector<IdType> tList;
			doGetKeylist(tList);
			return tList;
		}

		void regFactory(const IdType &aId, ConcreteFactory aFactory)
		{
			mCreators[aId] = aFactory;
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
		static ConcreteFactory get(const IdType &aId)
		{
			return ThisFactory::getInstance().doGet(aId);
		}

		static std::vector<IdType> keyList()
		{
			std::vector<IdType> tList;
			ThisFactory::getInstance().doGetKeylist(tList);
			return tList;
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

