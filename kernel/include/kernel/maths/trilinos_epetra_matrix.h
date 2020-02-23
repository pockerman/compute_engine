#ifndef TRILINOS_EPETRA_MATRIX_H
#define TRILINOS_EPETRA_MATRIX_H

#include "kernel/base/config.h"

#ifdef USE_TRILINOS

#include "kernel/base/types.h"

#include <Epetra_CrsMatrix.h>
#include <Epetra_CrsGraph.h>
#include <Epetra_SerialComm.h>

#include <memory>
#include <vector>

namespace kernel{
namespace numerics{

/// forward declarations
class TrilinosKrylovSolver;

class TrilinosEpetraMatrix
{

public:


    typedef real_t value_t;
    typedef std::vector<real_t> RowEntries;
    typedef std::vector<uint_t> RowIndices;

    /// Constructor; initializes the matrix to be empty, without any structure, i.e.
    /// the matrix is not usable at all. Thisonstructor is therefore only useful
    /// for matrices which are members of a
    /// class. All other matrices should be created at a point in the data flow
    /// where all necessary information is
    /// available.
    ///
    /// You have to initialize
    /// the matrix before usage with
    /// \p init(...).
    TrilinosEpetraMatrix ();

    /// \brief detailed dtor
    ~TrilinosEpetraMatrix();

    /// \brief Initialize a matrix with m global rows and n globals columns
    /// having nz non-zero entries per rows
    void init(uint_t m, uint_t n, uint_t nz);

    /// \brief Initialize by passing in the given sparsity pattern
    void init(const Epetra_CrsGraph& graph);

    /// \brief Zro the entries of the matrix
    void zero();

    /// \brief set the entries of the row to value
    void set_row_entries(uint_t r, real_t val);

    /// \brief Set the (i,j) entry of the matrix to val
    void set_entry(uint_t i,uint_t j,real_t val);

    /// \brief Returns the (i, j) entry
    real_t entry(uint_t i,  uint_t j)const;

    /// \brief set the entries in RowIndices to the values in RowEntries
    void set_row_entries(const RowIndices& indices,const RowEntries& entries)const;

    /// \brief add to the (i,j) entry of the matrix the value val
    void add_entry(uint_t i,uint_t j,real_t val);

    /// \brief Add to the entries in RowIndices the values in RowEntries of the given Epetra_CrsMatrix
    void add_row_entries(RowIndices& indices,RowEntries& entries);

    ///signal the underlying Epetra_FECrsMatrix that filling
    ///of the matrix is completed
    void fill_completed(){mat_->FillComplete();}

    /// \brief Returns true iff fill_completed has been called
    bool is_filled()const{return mat_->Filled();}

    /// \brief The number of rows the matrix has
    uint_t m () const;

    /// \brief The number of columns the matrix has
    uint_t n () const;

    /// \brief Print the matrix
    std::ostream& print(std::ostream& out)const;

protected:

    Epetra_CrsMatrix* get_matrix(){return mat_.get(); }

private:

   /// \brief The  Epetra datatype to hold matrix entries.
   /// A pointer since Epetra_CrsMatrix does not have a default ctor
   std::unique_ptr<Epetra_CrsMatrix> mat_;

   /// \brief The communicator we use.
   /// The Trilinos::Epetra_SerialComm has default ctor
   Epetra_SerialComm comm_;

   /// \brief
   std::unique_ptr<Epetra_Map> epetra_map_;

   friend class TrilinosKrylovSolver;

};

}
}
#endif
#endif // TRILINOS_EPETRA_MATRIX_H