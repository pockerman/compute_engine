#ifndef DATASETWRAPPER_HPP
#define DATASETWRAPPER_HPP

#include "kernel/base/types.h"
#include <boost/noncopyable.hpp>

#include <vector>

namespace kernel
{
	namespace data_structs
	{
		///
		/// \brief General template undefined
		///
		template<typename StorageTp>
		class DataSetWrapper;

		template<typename T>
		class DataSetWrapper<DynMat<T>>: private boost::noncopyable
		{
		

		public:
		
			///
			/// \brief The type of storage
			///
			typedef DynMat<T> storage_t;
			
			///
			/// \brief The type of the row
			///
			typedef DynVec<T> row_t;
		
			///
			/// \brief Constructor
			///
			DataSetWrapper();
			
			///
			/// \brief Destructor
			///
			~DataSetWrapper();
			
			///
			/// \brief Number of rows
			///
			uint_t n_rows()const{return data_.rows();}
			
			///
			/// \brief Number of columns
			///
			uint_t n_columns()const{return data_.columns();}
			
			///
			/// \brief Returns the row_idx-th row.
			/// Throws std::logic_error if the row_idx
			/// is out of range
			///
			row_t get_row(uint_t row_idx)const;
			
			///
			/// \brief Load the data set from the given filename
			/// using the specified loader
			///
			template<typename Loader>
			void load_from(const Loader& loader);
			
		private:
		
			///
			/// \brief The data 
			///
			storage_t data_;
			
		};
		
		template<typename T>
		template<typename Loader>
		void 
		DataSetWrapper<DynMat<T>>::load_from(const Loader& loader){
			
			loader(data_);
		}
		
	}// data_structs

}

#endif // DATASETWRAPPER_HPP
