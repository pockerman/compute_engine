#ifndef ELEMENT_H
#define ELEMENT_H

#include "kernel/numerics/dof_object.h"
#include "kernel/base/kernel_consts.h"

namespace kernel
{
namespace numerics
{

template<int dim> class Node;

namespace detail
{

class element_base: public DoFObject
{
public:

    /// \brief Destructor
    virtual ~element_base();

    /// \brief Returns the id of the element
    uint_t get_id()const{return id_;}

    /// \brief Set the id of the element
    void set_id(uint_t id){id_ = id;}

    /// \brief Returns the id of the element
    uint_t get_pid()const{return pid_;}

    /// \brief Set the id of the element
    void set_pid(uint_t id){pid_ = id;}

    bool has_valid_id()const{return true;}


protected:

    /// \brief Constructor
    element_base(uint_t id=KernelConsts::invalid_size_type(), uint_t pid=KernelConsts::invalid_size_type());

    /// \brief The id of the element
    uint_t id_;

    /// \brief The processor id the element belongs
    uint_t pid_;

};
}



/// \brief Wraps the notion of an element
template<int dim>
class Element: public detail::element_base
{

public:

    typedef std::shared_ptr<Element<dim>> neighbor_ptr_t;
    typedef std::shared_ptr<Node<dim>> node_ptr_t;

    /// \brief How many vertices the element has
    virtual uint_t n_vertices()const=0;

    /// \brief How many nodes the element has
    virtual uint_t n_nodes()const=0;

    /// \brief Set the i-th node
    virtual void set_node(uint_t i, node_ptr_t node)=0;

    /// \brief Reserve space for nodes
    virtual void reserve_nodes(uint n)=0;

    /// \brief Returns the i-th node
    virtual node_ptr_t get_node(uint_t n)=0;

    /// \brief How many edges the element has
    virtual uint_t n_edges()const=0;

    /// \brief How many faces the element has
    virtual uint_t n_faces()const=0;

    /// \brief Set the i-th neighbor
    virtual void set_neighbor(uint n, neighbor_ptr_t neigh)=0;

    /// \brief Reserve space for neighbors
    virtual void reserve_neighbors(uint n)=0;

    /// \brief Access the n-th neighbor
    virtual neighbor_ptr_t get_neighbor(uint_t n)=0;

protected:

    /// \brief Constructor
    Element()=default;

    /// \brief Constructor
    Element(uint_t id, uint_t pid);

    /// \brief The neighbors of the element
    std::vector<neighbor_ptr_t> neginbors_;

    /// \brief The nodes of the element
    std::vector<node_ptr_t> nodes_;

};

}

}

#endif // ELEMENT_H
