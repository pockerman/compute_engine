#include "kernel/discretization/vtk_mesh_file_writer.h"
#include "kernel/utilities/file_formats.h"
#include "kernel/utilities/predicates.h"
#include "kernel/discretization/mesh.h"
#include "kernel/discretization/element_mesh_iterator.h"
#include "kernel/discretization/node_mesh_iterator.h"
#include "kernel/discretization/element.h"
#include "kernel/discretization/node.h"
#include "kernel/discretization/mesh_predicates.h"


#include <chrono>
#include <exception>

namespace kernel{
namespace numerics{

VtkMeshFileWriter::VtkMeshFileWriter(const std::string& filename, bool open_file)
    :
   FileWriterBase(filename, FileFormats::Type::VTK, open_file )
{}


void
VtkMeshFileWriter::write_header(){

    //if the file is not open
    if(!is_open()){
        throw std::logic_error("File "+this->file_name_+" is not open");
    }

    std::chrono::time_point<std::chrono::system_clock> now;
    now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);

    this->file_<<"# vtk DataFile Version 2.0"<<std::endl;
    this->file_<<"This file was generated by kernellib at "<<std::ctime(&now_time)<<std::endl;
    this->file_<<"ASCII"<<std::endl;
    this->file_<<"DATASET UNSTRUCTURED_GRID"<<std::endl;
    this->file_<<std::endl;
    this->file_.flush();
}


void
VtkMeshFileWriter::write_mesh(const Mesh<2>& mesh){

      write_header();

      const uint_t n_elements = mesh.n_elements();
      const uint_t n_nodes    = mesh.n_nodes();
      const uint_t ele_n_nodes = 4;

      this->file_<<"POINTS "<<n_nodes<<" double\n";

      ConstNodeMeshIterator<NotNull, Mesh<2>> node_iterator(mesh);

      auto node_it_begin = node_iterator.begin();
      auto node_it_end =   node_iterator.end();

      for(; node_it_begin != node_it_end; ++node_it_begin){
         auto p = (*node_it_begin)->coordinates();
         this->file_<<p[0]<<" "<<p[1]<<" "<<0<<std::endl;
      }

      this->file_<<std::endl;
      this->file_<<"CELLS "<<n_elements<<" "<<n_elements*ele_n_nodes+n_elements<<std::endl;

      ConstElementMeshIterator<Active, Mesh<2>> filter(mesh);

      auto elem_it     = filter.begin();
      auto elem_it_end = filter.end();

      for(; elem_it!=elem_it_end; ++elem_it)
      {
         auto*  elem = *elem_it;
         this->file_<<elem->n_nodes()<<" ";

         for(uint_t n=0; n<elem->n_nodes(); n++)
          {

             this->file_<<elem->get_node(n)->get_id()<<"  ";
             if(n==elem->n_nodes()-1)
               this->file_<<"\n";
          }
      }

      this->file_<<std::endl;
      this->file_<<"CELL_TYPES"<<" "<<n_elements<<std::endl;

      elem_it = filter.begin();

      for(; elem_it != elem_it_end; ++elem_it){
        this->file_<<9<<"\n";
      }
      this->file_<<std::endl;
      this->file_.flush();
}

void
VtkMeshFileWriter::write_mesh(const Mesh<2>& mesh, const VtkMeshMeshCellOptions& options){

    write_mesh(mesh);

    // now let's write the options
    for(const auto& option : options.options){
        write_option(mesh, option);
    }
}

void
VtkMeshFileWriter::write_mesh(const Mesh<2>& mesh, const std::vector<real_t>& cell_values, const std::string& option_name){
     write_mesh(mesh);

     //get the number of active (this should be) elements
     const uint_t n_elements = mesh.n_elements();

     this->file_<<std::endl;
     this->file_<<"CELL_DATA"<<" "<<n_elements<<std::endl;
     this->file_<<"SCALARS "<<option_name<<" double "<<std::endl;
     this->file_<<"LOOKUP_TABLE default"<<std::endl;

     ConstElementMeshIterator<Active, Mesh<2>> filter(mesh);

     auto elem_it     = filter.begin();
     auto elem_it_end = filter.end();

     for(; elem_it != elem_it_end; elem_it++){

         const Element<2>* e = *elem_it;
         this->file_<<cell_values[e->get_id()]<<std::endl;
     }
     this->file_<<"\n";
     this->file_.flush();
}

void
VtkMeshFileWriter::write_option(const Mesh<2>& mesh, const std::string& name){

    //get the number of active (this should be) elements
    const uint_t n_elements = mesh.n_elements();

    this->file_<<std::endl;
    this->file_<<"CELL_DATA"<<" "<<n_elements<<std::endl;
    this->file_<<"SCALARS "<<name<<" double "<<std::endl;
    this->file_<<"LOOKUP_TABLE default"<<std::endl;

    ConstElementMeshIterator<Active, Mesh<2>> filter(mesh);

    auto elem_it     = filter.begin();
    auto elem_it_end = filter.end();

    for(; elem_it != elem_it_end; elem_it++){

        const Element<2>* e = *elem_it;
        auto val = e->get_pid(); //boost::any_cast<real_t>(e->get_property(name));
        this->file_<<val<<std::endl;
    }
    this->file_<<"\n";
    this->file_.flush();

}

}
}
