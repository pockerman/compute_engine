#include "kernel/models/vector_updater.h"
#include "kernel/executors/thread_pool.h"
#include "kernel/base/types.h"
#include "kernel/data_structs/partitioned_object.h"
#include "kernel/data_structs/range_1d.h"
#include "kernel/partitioners/array_partitioner.h"
#include "kernel/models/scaled_ops.h"
#include "kernel/utilities/result_holder.h"

#include <vector>
#include <gtest/gtest.h>

namespace{

}

TEST(TestVectorUpdater, TestVectorUpdaterInvalidVectorSizes) {

    /***
       * Test Scenario:   The application attempts to get the vector update of three vectors of unequal size
       * Expected Output:	ParFrameLib throws a std::runtime_error
     **/

    /// TODO: Is there a better way to do this?
    try{

        using Vector = kernel::PartitionedType<kernel::DynVec<kernel::real_t>>;
        Vector v1(10);
        Vector v2(20);
        Vector v3(20);
        kernel::real_t a = 1.0;
        kernel::real_t b = 1.0;
        kernel::ResultHolder<Vector> x(std::move(v1));
        kernel::ThreadPool pool(1);

        kernel::VectorUpdater<Vector, kernel::ScaledSum<kernel::real_t>, kernel::real_t> product(x, v2, v3, a, b);
        product.execute(pool);
    }
    catch(std::runtime_error& e){
        std::string except_msg(e.what());
        ASSERT_EQ(except_msg, "Invalid vector sizes");
    }
}


TEST(TestVectorUpdater, TestVectorUpdaterReturnedResult) {

    /***
       * Test Scenario:   The application attempts to get the dot product of two vectors of the same size
       * Expected Output:	ParFrameLib should compute the dot product of two vectors and return the result
     **/

    /// TODO: Is there a better way to do this?
    try{

        using Vector = kernel::PartitionedType<kernel::DynVec<kernel::real_t>>;
        Vector v1(10, 0.0);
        Vector v2(10, 1.0);
        Vector v3(10, 1.0 );
        kernel::real_t a = 1.0;
        kernel::real_t b = 1.0;
        kernel::ThreadPool pool(1);
        std::vector<kernel::range1d<kernel::uint_t>> partitions;
        kernel::partition_range(0, v1.size(), partitions, pool.get_n_threads() );

        v1.set_partitions(partitions);
        v2.set_partitions(partitions);
        v3.set_partitions(partitions);
        kernel::ResultHolder<Vector> x(std::move(v1));
        kernel::VectorUpdater<Vector, kernel::ScaledSum<kernel::real_t>, kernel::real_t> product(x, v2, v3, a, b);
        product.execute(pool);

        auto& rslt = product.get();
        EXPECT_EQ(rslt.is_result_valid(), true);
        EXPECT_EQ(rslt.get().first[0], 2.0);

    }
    catch(...){
        ASSERT_EQ(1, 2)<<"Test TestVectorUpdater::TestVectorUpdaterReturnedResult failed with unknown exception";
    }
}



