# Example 16: Compare Lasso, Ridge and ElasticNet Regularizers

## Contents
* [Overview](#overview) 
* [Common Regularizers](#common_regularizers)
	* [Lasso Regularization](#lasso_regularization)
	* [Ridge Regularization](#ridge_regularization)
	* [ElasticNet Regularization](#elasticnet_regularization)
* [Include files](#include_files)
* [The main function](#m_func)
* [Results](#results)
* [Source Code](#source_code)


## <a name="overview"></a> Overview

A comparison of the regularizers can be found here:
<a href="https://www.datacamp.com/community/tutorials/tutorial-ridge-lasso-elastic-net">Regularization: Ridge, Lasso and Elastic Net</a>

## <a name="common_regularizers"></a> Common Regularizers

### <a name="lasso_regularization"></a> Lasso Regularization

### <a name="ridge_regularization"></a> Ridge Regularization

### <a name="elasticnet_regularization"></a> ElasticNet Regularization

ElasticNet regularization (<a href="https://en.wikipedia.org/wiki/Elastic_net_regularization">Elastic net regularization</a>)


## <a name="include_files"></a> Include files

```
#include "cubic_engine/base/cubic_engine_types.h"
#include "cubic_engine/ml/supervised_learning/regressor.h"
#include "cubic_engine/optimization/serial_batch_gradient_descent.h"
#include "cubic_engine/optimization/utils/gd_control.h"
#include "cubic_engine/maths/functions/lasso_function.h"
#include "cubic_engine/maths/functions/ridge_function.h"
#include "cubic_engine/maths/functions/elastic_net_function.h"


#include "kernel/maths/functions/real_vector_polynomial.h"
#include "kernel/maths/errorfunctions/mse_function.h"
#include "kernel/utilities/data_set_loaders.h"

#include <iostream>
```

## <a name="m_func"></a> The main function

```
int main(){

    using cengine::uint_t;
    using cengine::real_t;
    using cengine::DynMat;
    using cengine::DynVec;
    using cengine::GDControl;
    using cengine::Gd;
    using cengine::LinearRegression;
    using cengine::LassoFunction;
    using cengine::RidgeFunction;
    using cengine::ElasticNetFunction;
    using kernel::RealVectorPolynomialFunction;
    using kernel::MSEFunction;

    try{

        auto dataset = kernel::load_x_y_sinuisoid_data_set();

        typedef RealVectorPolynomialFunction hypothesis_t;
        typedef LassoFunction<hypothesis_t, DynMat<real_t>, DynVec<real_t>> lasso_t;
        typedef RidgeFunction<hypothesis_t, DynMat<real_t>, DynVec<real_t>> ridge_t;
        typedef ElasticNetFunction<hypothesis_t, DynMat<real_t>, DynVec<real_t>> elastic_net_t;

        const real_t LASSO_COEF = 0.0001;
        const real_t RIDGE_COEF = 0.001;

        {
            std::cout<<"Using Lasso Regularizer"<<std::endl;
            std::cout<<"\n";

            // the regressor to use. use a hypothesis of the form
            // f = w_0 + w_1*x_1
            // set initial weights to 0
            LinearRegression regressor({0.0, 0.0});

            /// lasso regulaizer
            lasso_t lasso(regressor.get_model(), LASSO_COEF, 1,
                          regressor.get_model().n_coeffs());

            GDControl control(10000, kernel::KernelConsts::tolerance(),
                                       GDControl::DEFAULT_LEARNING_RATE);
            control.show_iterations = false;

            typedef MSEFunction<hypothesis_t, DynMat<real_t>, DynVec<real_t>, lasso_t> error_t;
            Gd<error_t> gd(control);

            auto result = regressor.train(dataset.first, dataset.second, gd, lasso);
            std::cout<<result<<std::endl;
            std::cout<<regressor<<std::endl;

        }

        {
            std::cout<<"Using Ridge Regularizer"<<std::endl;
            std::cout<<"\n";

            // the classifier to use. use a hypothesis of the form
            // f = w_0 + w_1*x_1
            // set initial weights to 0
            LinearRegression regressor({0.0, 0.0});

            ridge_t ridge(regressor.get_model(), RIDGE_COEF, 1,
                          regressor.get_model().n_coeffs());

            GDControl control(10000, kernel::KernelConsts::tolerance(),
                              GDControl::DEFAULT_LEARNING_RATE);
            control.show_iterations = false;

            // the error function to to use for measuring the error
            typedef MSEFunction<hypothesis_t, DynMat<real_t>, DynVec<real_t>, ridge_t> error_t;
            Gd<error_t> gd(control);

            auto result = regressor.train(dataset.first, dataset.second, gd, ridge);
            std::cout<<result<<std::endl;
            std::cout<<regressor<<std::endl;

        }

        {
            std::cout<<"Using ElasticNet Regularizer"<<std::endl;
            std::cout<<"\n";

            // the classifier to use. use a hypothesis of the form
            // f = w_0 + w_1*x_1
            // set initial weights to 0
            LinearRegression regressor({0.0, 0.0});

             elastic_net_t elastic_net(regressor.get_model(), LASSO_COEF, RIDGE_COEF,
                                       1, regressor.get_model().n_coeffs());

            GDControl control(10000, kernel::KernelConsts::tolerance(),
                                       GDControl::DEFAULT_LEARNING_RATE);
            control.show_iterations = false;

            // the error function to to use for measuring the error
            typedef MSEFunction<hypothesis_t, DynMat<real_t>, DynVec<real_t>, elastic_net_t> error_t;
            Gd<error_t> gd(control);

            auto result = regressor.train(dataset.first, dataset.second, gd, elastic_net);
            std::cout<<result<<std::endl;
            std::cout<<regressor<<std::endl;

        }
    }
    catch(std::exception& e){

        std::cerr<<e.what()<<std::endl;
    }
    catch(...){

        std::cerr<<"Unknown exception occured"<<std::endl;
    }

    return 0;
}
```

## <a name="results"></a> Results

```
Using Lasso Regularizer

# iterations:..5092
# processors:..1
# threads:.....1
Residual:......9.99727e-09
Tolerance:.....1e-08
Convergence:...Yes
Total time:....0.137556
Learning rate:..0.01

1.17059,-2.16211

Using Ridge Regularizer

# iterations:..3850
# processors:..1
# threads:.....1
Residual:......9.87317e-09
Tolerance:.....1e-08
Convergence:...Yes
Total time:....0.0711511
Learning rate:..0.01

1.15803,-2.1376

Using ElasticNet Regularizer

# iterations:..3831
# processors:..1
# threads:.....1
Residual:......9.95495e-09
Tolerance:.....1e-08
Convergence:...Yes
Total time:....0.0772399
Learning rate:..0.01

1.15767,-2.13691
```

## <a name="source_code"></a> Source Code

<a href="../exe.cpp">exe.cpp</a>
