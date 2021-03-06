#include "kernel/discretization/element.h"
#include "kernel/discretization/node.h"

#include <functional>
namespace kernel
{

namespace numerics
{

template<int dim>
Element<dim>::Element()
    :
      MeshEntity(KernelConsts::invalid_size_type(), KernelConsts::invalid_size_type()),
      dofs_(),
      property_map_()
{
    std::function<boost::any()> func = [this]()->boost::any{return this->get_pid();};
    property_map_.insert({"pid", func});
}

template<int dim>
Element<dim>::Element(uint_t id, uint_t pid)
    :
      MeshEntity(id, pid),
      dofs_(),
      property_map_()
{

    std::function<boost::any()> func = [this]()->boost::any{return this->get_pid();};
    property_map_.insert({"pid", func});
}

template<int dim>
Element<dim>::~Element()
{}

template<int dim>
void
Element<dim>::invalidate_dofs(const std::string_view name){
    dofs_.invalidate_dofs(name);
}


template<int dim>
void
Element<dim>::insert_dof(DoF&& dof){
    dofs_.insert_dof(std::move(dof));
}

template<int dim>
void
Element<dim>::get_dofs(std::string_view name, std::vector<DoF>& dofs)const{

    dofs.clear();
    dofs.push_back(dofs_.get_dof(name));
}


template<int dim>
void
Element<dim>::resize_nodes(){
    nodes_.resize(n_nodes(), nullptr);
}

template<int dim>
typename Element<dim>::neighbor_ptr_t
Element<dim>::neighbor_ptr(uint_t n){
    return neginbors_[n];
}

template<int dim>
void
Element<dim>::resize_neighbors(){
    neginbors_.resize(n_neighbors(), nullptr);
}


template<int dim>
void
Element<dim>::set_neighbor(uint_t n, typename Element<dim>::neighbor_ptr_t neigh){

    if(n >= n_neighbors()){
        throw std::logic_error("Invalid node index: " +
                               std::to_string(n) +
                               " not in [0," +
                               std::to_string(n_neighbors()) +
                               ")");
    }

    neginbors_[n] = neigh;
}

template<int dim>
void
Element<dim>::set_node(uint_t i, typename Element<dim>::node_ptr_t node){

    if(!node){

        throw std::logic_error("Cannot add null node pointer");
    }

    if(i >= n_nodes()){
        throw std::logic_error("Invalid node index: " +
                               std::to_string(i) +
                               " not in [0," +
                               std::to_string(n_nodes()) +
                               ")");
    }

    nodes_[i] = node;

}

template<int dim>
void
Element<dim>::append_node(typename Element<dim>::node_ptr_t node){

    if(!node){
        throw std::logic_error("Cannot add null node pointer");
    }

    if( this->nodes_.size() == n_nodes()){
        throw std::logic_error("Nodes list is full cannot append new node ");
    }

    this->nodes_.push_back(node);

}

template<int dim>
typename Element<dim>::node_ptr_t
Element<dim>::get_node(uint_t n){

    if(n >= n_nodes()){
        throw std::logic_error("Invalid node index: " +
                               std::to_string(n) +
                               " not in [0," +
                               std::to_string(n_nodes()) +
                               ")");
    }

    return  this->nodes_[n];

}

template<int dim>
typename Element<dim>::cneighbor_ref_t
Element<dim>::get_neighbor(uint_t n)const{

    if(n >= n_neighbors()){
        throw std::logic_error("Invalid neighbor index: " +
                               std::to_string(n) +
                               " not in [0," +
                               std::to_string(n_neighbors()) +
                               ")");
    }

    if(neginbors_.empty()){
        throw std::logic_error("Neighbor elements have not been populated");
    }

    if(neginbors_[n] == nullptr){
        throw std::logic_error("The requested neighbor is NULL");
    }

    return *neginbors_[n];
}



template<int dim>
const typename Element<dim>::neighbor_ptr_t
Element<dim>::neighbor_ptr(uint_t n)const{
    return neginbors_[n];
}

template<int dim>
uint_t
Element<dim>::which_neighbor_am_i(const Element<dim>& element)const{

    if(neginbors_.empty()){
        throw std::logic_error("Neighbor elements have not been populated");
    }

    for(uint_t n=0; n<neginbors_.size(); ++n){
        if(neginbors_[n] && neginbors_[n]->get_id() == element.get_id()){
            return n;
        }
    }

    return KernelConsts::invalid_size_type();
}

template<int dim>
GeomPoint<dim>
Element<dim>::centroid()const{

    auto vertices = get_vertices();
    GeomPoint<dim> centroid(0.0);

    for(auto* vertex:vertices){
        centroid += *vertex;
    }

    return centroid /= n_vertices();
}

template<int dim>
std::vector<typename Element<dim>::node_ptr_t>
Element<dim>::get_vertices()const{

    if(nodes_.empty()){
        throw std::logic_error("Nodes list is not initialized");
    }


    std::vector<typename Element<dim>::node_ptr_t> vertices;
    vertices.reserve(n_vertices());

    for(uint_t n=0; n < n_nodes(); ++n){
        if(nodes_[n] && nodes_[n]->is_vertex()){
            vertices.push_back(nodes_[n]);
        }
    }

    return vertices;

}

template<int dim>
boost::any
Element<dim>::get_property(const std::string& name)const{

    auto itr = property_map_.find(name);

    if(itr != property_map_.end()){
        return (itr->second)();
    }

    throw std::logic_error("Property: " + name + " was not found");
}


template class Element<1>;
template class Element<2>;
template class Element<3>;

}

}
