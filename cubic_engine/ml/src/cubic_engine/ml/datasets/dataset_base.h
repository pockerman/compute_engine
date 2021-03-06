#ifndef DATASET_BASE_H
#define DATASET_BASE_H

namespace cengine {
namespace ml {

///
/// \brief The DatasetBase class. This class does not
/// simply used for providing dataset type uniformity
///
class DatasetBase
{
public:

    ///
    /// \brief Destructor
    ///
    virtual ~DatasetBase() = default;

protected:

    ///
    /// \brief Destructor
    ///
    DatasetBase() = default;
};

}

}

#endif // DATASET_BASE_H
