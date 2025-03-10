#ifndef LLALBM_INLET_OUTLET_INITIALIZER_HPP
#define LLALBM_INLET_OUTLET_INITIALIZER_HPP


// =========== STL INCLUDES ===========
#include <vector>
#include <array>
#include <cassert>
#include <functional>
#include <execution>
#include <openacc.h>
// ======================================

// =========== EIGEN INCLUDES ===========
#include <Eigen/Core>
#include <Eigen/Dense>
#include <unsupported/Eigen/CXX11/Tensor>
// ======================================

// =========== LLALBM INCLUDES ===========
#include "../../utils/loggers/Logger.hpp"
#include "../../utils/aliases.hpp" 
#include "../PolicyTypes.hpp"
// =======================================


namespace llalbm::core::initializers 
{
    /**
     * @brief Generic initializer for inlets and outlets values of velocity
     * 
     * @tparam dim Number of physical dimensions
     */
    template<std::size_t dim>
    class VelocityInitializer : public InitializationPolicyTag, public SequentialTag
    {
    public: 
        static std::vector<BoundaryPoint<dim>> inlet_nodes;
        static std::vector<BoundaryPoint<dim>> outlet_nodes;

        static std::array<Eigen::Index, dim> lattice_dimensions;
        
        static std::array<std::function<double (double, BoundaryPoint<dim>)>, dim> inlet_update_function;
        static std::array<std::function<double (double, BoundaryPoint<dim>)>, dim> outlet_update_function;
    
        /**
         * @brief Construct a new Lid Driven Initializer object
         * @note The constructor doesn't do anything special as this object is
         * thought as being completely static
         */
        VelocityInitializer() = default;

        /**
         * @brief Static method that links the elements to the lattice
         * to those of the initializer, via the passage of references.
         * 
         * @param inlet_nodes_ Vector containing the coordinates of inlet nodes
         * @param outlet_nodes_ Vector containing the coordinates of outlet nodes
         */
        static void attach_nodes(
            std::vector<BoundaryPoint<dim>>& inlet_nodes_,
            std::vector<BoundaryPoint<dim>>& outlet_nodes_
        )
        {
            inlet_nodes = inlet_nodes_;
            outlet_nodes = outlet_nodes_; 
        }

        /**
         * @brief Static method that links the update functions to the initializer,
         * via the passage of references.
         * 
         * @param inlet_update_function_ Array of functions that update the inlet nodes
         * @param outlet_update_function_ Array of functions that update the outlet nodes
         */
        static void attach_update_functions(
            std::array<std::function<double (double, BoundaryPoint<dim>)>, dim>& inlet_update_function_,
            std::array<std::function<double (double, BoundaryPoint<dim>)>, dim>& outlet_update_function_
        )
        {
            inlet_update_function = inlet_update_function_;
            outlet_update_function = outlet_update_function_;
        }

        /**
         * @brief Static method that updates the inlet and outlet nodes
         * 
         * @param time_step Current time step
         */
        static void update_nodes(const double& time_step, Eigen::Tensor<double, dim+1>& velocity_tensor, Eigen::Tensor<double, dim> density_tensor)
        {
            std::size_t inlet_size = inlet_nodes.size();
            std::size_t outlet_size = outlet_nodes.size();

            // At the given time step, update the inlet nodes by
            // calling the update function for each dimension
            Eigen::array<Eigen::Index, dim+1> coordinates;

            for (std::size_t i = 0; i < inlet_size; ++i)
            {
                for (std::size_t k = 0; k < dim; ++k)
                {
                    coordinates[k] = inlet_nodes[i].coords[k];
                }

                for (std::size_t j = 0; j < dim; ++j)
                {
                    coordinates[dim] = j;
                    double val = inlet_update_function[j](time_step, inlet_nodes[i]);
                    velocity_tensor(coordinates) = val;
                }
            }

            // And also update the outlet nodes
            for (std::size_t i = 0; i < outlet_size; ++i)
            {
                for (std::size_t k = 0; k < dim; ++k)
                {
                    coordinates[k] = outlet_nodes[i].coords[k];
                }

                for (std::size_t j = 0; j < dim; ++j)
                {
                    coordinates[dim] = j;
                    double val = outlet_update_function[j](time_step, outlet_nodes[i]);
                    velocity_tensor(coordinates) = val;
                }
            }
        }

        static void print_data()
        {
            // Print the inlet nodes
            std::cout << "Inlet nodes: " << std::endl;
            for (std::size_t i = 0; i < inlet_nodes.size(); ++i)
            {
                std::cout << "Node " << i << ": ";
                for (std::size_t j = 0; j < dim; ++j)
                {
                    std::cout << inlet_nodes[i].coords[j] << " ";
                }
                std::cout << std::endl;
            }
            // Print the outlet nodes
            std::cout << "Outlet nodes: " << std::endl;
            for (std::size_t i = 0; i < outlet_nodes.size(); ++i)
            {
                std::cout << "Node " << i << ": ";
                for (std::size_t j = 0; j < dim; ++j)
                {
                    std::cout << outlet_nodes[i].coords[j] << " ";
                }
                std::cout << std::endl;
            }
        }
    };

    template<std::size_t dim>
    std::vector<BoundaryPoint<dim>> VelocityInitializer<dim>::inlet_nodes;
    template<std::size_t dim>
    std::vector<BoundaryPoint<dim>> VelocityInitializer<dim>::outlet_nodes;
    template<std::size_t dim>
    std::array<std::function<double (double, BoundaryPoint<dim>)>, dim> VelocityInitializer<dim>::inlet_update_function;
    template<std::size_t dim>
    std::array<std::function<double (double, BoundaryPoint<dim>)>, dim> VelocityInitializer<dim>::outlet_update_function;



template<std::size_t dim>
    class OMPVelocityInitializer : public InitializationPolicyTag, public OMPTag
    {
    public: 
        static std::vector<BoundaryPoint<dim>> inlet_nodes;
        static std::vector<BoundaryPoint<dim>> outlet_nodes;

        static std::array<Eigen::Index, dim> lattice_dimensions;
        
        static std::array<std::function<double (double, BoundaryPoint<dim>)>, dim> inlet_update_function;
        static std::array<std::function<double (double, BoundaryPoint<dim>)>, dim> outlet_update_function;
    
        /**
         * @brief Construct a new Lid Driven Initializer object
         * @note The constructor doesn't do anything special as this object is
         * thought as being completely static
         */
        OMPVelocityInitializer() = default;

        /**
         * @brief Static method that links the elements to the lattice
         * to those of the initializer, via the passage of references.
         * 
         * @param inlet_nodes_ Vector containing the coordinates of inlet nodes
         * @param outlet_nodes_ Vector containing the coordinates of outlet nodes
         */
        static void attach_nodes(
            std::vector<BoundaryPoint<dim>>& inlet_nodes_,
            std::vector<BoundaryPoint<dim>>& outlet_nodes_
        )
        {
            inlet_nodes = inlet_nodes_;
            outlet_nodes = outlet_nodes_; 
        }

        /**
         * @brief Static method that links the update functions to the initializer,
         * via the passage of references.
         * 
         * @param inlet_update_function_ Array of functions that update the inlet nodes
         * @param outlet_update_function_ Array of functions that update the outlet nodes
         */
        static void attach_update_functions(
            std::array<std::function<double (double, BoundaryPoint<dim>)>, dim>& inlet_update_function_,
            std::array<std::function<double (double, BoundaryPoint<dim>)>, dim>& outlet_update_function_
        )
        {
            inlet_update_function = inlet_update_function_;
            outlet_update_function = outlet_update_function_;
        }
        
        /**
         * @brief Static method that updates the inlet and outlet nodes
         * 
         * @param time_step Current time step
         */
        static void update_nodes(const double& time_step, Eigen::Tensor<double, dim+1>& velocity_tensor, Eigen::Tensor<double, dim> density_tensor)
        {
            std::size_t inlet_size = inlet_nodes.size();
            std::size_t outlet_size = outlet_nodes.size();

            // At the given time step, update the inlet nodes by
            // calling the update function for each dimension
            Eigen::array<Eigen::Index, dim+1> coordinates;

            #pragma omp parallel for private(coordinates)
            for (std::size_t i = 0; i < inlet_size; ++i)
            {
                for (std::size_t k = 0; k < dim; ++k)
                {
                    coordinates[k] = inlet_nodes[i].coords[k];
                }

                for (std::size_t j = 0; j < dim; ++j)
                {
                    coordinates[dim] = j;
                    double val = inlet_update_function[j](time_step, inlet_nodes[i]);
                    velocity_tensor(coordinates) = val;
                }
            }

            // And also update the outlet nodes
            #pragma omp parallel for private(coordinates)
            for (std::size_t i = 0; i < outlet_size; ++i)
            {
                for (std::size_t k = 0; k < dim; ++k)
                {
                    coordinates[k] = outlet_nodes[i].coords[k];
                }

                for (std::size_t j = 0; j < dim; ++j)
                {
                    coordinates[dim] = j;
                    double val = outlet_update_function[j](time_step, outlet_nodes[i]);
                    velocity_tensor(coordinates) = val;
                }
            }
        }

        static void print_data()
        {
            // Print the inlet nodes
            std::cout << "Inlet nodes: " << std::endl;
            for (std::size_t i = 0; i < inlet_nodes.size(); ++i)
            {
                std::cout << "Node " << i << ": ";
                for (std::size_t j = 0; j < dim; ++j)
                {
                    std::cout << inlet_nodes[i].coords[j] << " ";
                }
                std::cout << std::endl;
            }
            // Print the outlet nodes
            std::cout << "Outlet nodes: " << std::endl;
            for (std::size_t i = 0; i < outlet_nodes.size(); ++i)
            {
                std::cout << "Node " << i << ": ";
                for (std::size_t j = 0; j < dim; ++j)
                {
                    std::cout << outlet_nodes[i].coords[j] << " ";
                }
                std::cout << std::endl;
            }
        }
    };

    template<std::size_t dim>
    std::vector<BoundaryPoint<dim>> OMPVelocityInitializer<dim>::inlet_nodes;
    template<std::size_t dim>
    std::vector<BoundaryPoint<dim>> OMPVelocityInitializer<dim>::outlet_nodes;
    template<std::size_t dim>
    std::array<std::function<double (double, BoundaryPoint<dim>)>, dim> OMPVelocityInitializer<dim>::inlet_update_function;
    template<std::size_t dim>
    std::array<std::function<double (double, BoundaryPoint<dim>)>, dim> OMPVelocityInitializer<dim>::outlet_update_function;

    template<std::size_t dim>
    class STDExecVelocityInitializer : public InitializationPolicyTag, public STDExecTag
    {
    public: 
        static std::vector<BoundaryPoint<dim>> inlet_nodes;
        static std::vector<BoundaryPoint<dim>> outlet_nodes;

        static std::array<Eigen::Index, dim> lattice_dimensions;
        
        static std::array<std::function<double (double, BoundaryPoint<dim>)>, dim> inlet_update_function;
        static std::array<std::function<double (double, BoundaryPoint<dim>)>, dim> outlet_update_function;
    
        /**
         * @brief Construct a new Lid Driven Initializer object
         * @note The constructor doesn't do anything special as this object is
         * thought as being completely static
         */
        STDExecVelocityInitializer() = default;

        /**
         * @brief Static method that links the elements to the lattice
         * to those of the initializer, via the passage of references.
         * 
         * @param inlet_nodes_ Vector containing the coordinates of inlet nodes
         * @param outlet_nodes_ Vector containing the coordinates of outlet nodes
         */
        static void attach_nodes(
            std::vector<BoundaryPoint<dim>>& inlet_nodes_,
            std::vector<BoundaryPoint<dim>>& outlet_nodes_
        )
        {
            inlet_nodes = inlet_nodes_;
            outlet_nodes = outlet_nodes_; 
        }

        /**
         * @brief Static method that links the update functions to the initializer,
         * via the passage of references.
         * 
         * @param inlet_update_function_ Array of functions that update the inlet nodes
         * @param outlet_update_function_ Array of functions that update the outlet nodes
         */
        static void attach_update_functions(
            std::array<std::function<double (double, BoundaryPoint<dim>)>, dim>& inlet_update_function_,
            std::array<std::function<double (double, BoundaryPoint<dim>)>, dim>& outlet_update_function_
        )
        {
            inlet_update_function = inlet_update_function_;
            outlet_update_function = outlet_update_function_;
        }

        /**
         * @brief Static method that updates the inlet and outlet nodes
         * 
         * @param time_step Current time step
         */
        static void update_nodes(const double& time_step, Eigen::Tensor<double, dim+1>& velocity_tensor, Eigen::Tensor<double, dim> density_tensor)
        {
            //std::size_t inlet_size = inlet_nodes.size();
            //std::size_t outlet_size = outlet_nodes.size();

            // At the given time step, update the inlet nodes by
            // calling the update function for each dimension
            Eigen::array<Eigen::Index, dim+1> coordinates;

            //for (std::size_t i = 0; i < inlet_size; ++i)
            std::for_each(std::execution::par,inlet_nodes.begin(),inlet_nodes.end(),[&](const auto inlet_node)
            {
                for (std::size_t k = 0; k < dim; ++k)
                {
                    coordinates[k] = inlet_node.coords[k];
                }

                for (std::size_t j = 0; j < dim; ++j)
                {
                    coordinates[dim] = j;
                    double val = inlet_update_function[j](time_step, inlet_node);
                    velocity_tensor(coordinates) = val;
                }
            });

            // And also update the outlet nodes
            //for (std::size_t i = 0; i < outlet_size; ++i)
            std::for_each(std::execution::par,outlet_nodes.begin(),outlet_nodes.end(),[&](const auto outlet_node)
            {
                for (std::size_t k = 0; k < dim; ++k)
                {
                    coordinates[k] = outlet_node.coords[k];
                }

                for (std::size_t j = 0; j < dim; ++j)
                {
                    coordinates[dim] = j;
                    double val = outlet_update_function[j](time_step, outlet_node);
                    velocity_tensor(coordinates) = val;
                }
            });
        }

        static void print_data()
        {
            // Print the inlet nodes
            std::cout << "Inlet nodes: " << std::endl;
            for (std::size_t i = 0; i < inlet_nodes.size(); ++i)
            {
                std::cout << "Node " << i << ": ";
                for (std::size_t j = 0; j < dim; ++j)
                {
                    std::cout << inlet_nodes[i].coords[j] << " ";
                }
                std::cout << std::endl;
            }
            // Print the outlet nodes
            std::cout << "Outlet nodes: " << std::endl;
            for (std::size_t i = 0; i < outlet_nodes.size(); ++i)
            {
                std::cout << "Node " << i << ": ";
                for (std::size_t j = 0; j < dim; ++j)
                {
                    std::cout << outlet_nodes[i].coords[j] << " ";
                }
                std::cout << std::endl;
            }
        }
    };

    template<std::size_t dim>
    std::vector<BoundaryPoint<dim>> STDExecVelocityInitializer<dim>::inlet_nodes;
    template<std::size_t dim>
    std::vector<BoundaryPoint<dim>> STDExecVelocityInitializer<dim>::outlet_nodes;
    template<std::size_t dim>
    std::array<std::function<double (double, BoundaryPoint<dim>)>, dim> STDExecVelocityInitializer<dim>::inlet_update_function;
    template<std::size_t dim>
    std::array<std::function<double (double, BoundaryPoint<dim>)>, dim> STDExecVelocityInitializer<dim>::outlet_update_function;



    template<std::size_t dim>
    class OpenACCVelocityInitializer : public InitializationPolicyTag, public OpenACCTag
    {
    public: 
        static std::vector<BoundaryPoint<dim>> inlet_nodes;
        static std::vector<BoundaryPoint<dim>> outlet_nodes;

        static std::array<Eigen::Index, dim> lattice_dimensions;
        
        static std::array<std::function<double (double, BoundaryPoint<dim>)>, dim> inlet_update_function;
        static std::array<std::function<double (double, BoundaryPoint<dim>)>, dim> outlet_update_function;
    
        /**
         * @brief Construct a new Lid Driven Initializer object
         * @note The constructor doesn't do anything special as this object is
         * thought as being completely static
         */
        OpenACCVelocityInitializer() = default;

        /**
         * @brief Static method that links the elements to the lattice
         * to those of the initializer, via the passage of references.
         * 
         * @param inlet_nodes_ Vector containing the coordinates of inlet nodes
         * @param outlet_nodes_ Vector containing the coordinates of outlet nodes
         */
        static void attach_nodes(
            std::vector<BoundaryPoint<dim>>& inlet_nodes_,
            std::vector<BoundaryPoint<dim>>& outlet_nodes_
        )
        {
            inlet_nodes = inlet_nodes_;
            outlet_nodes = outlet_nodes_; 
        }

        /**
         * @brief Static method that links the update functions to the initializer,
         * via the passage of references.
         * 
         * @param inlet_update_function_ Array of functions that update the inlet nodes
         * @param outlet_update_function_ Array of functions that update the outlet nodes
         */
        static void attach_update_functions(
            std::array<std::function<double (double, BoundaryPoint<dim>)>, dim>& inlet_update_function_,
            std::array<std::function<double (double, BoundaryPoint<dim>)>, dim>& outlet_update_function_
        )
        {
            inlet_update_function = inlet_update_function_;
            outlet_update_function = outlet_update_function_;
        }

        /**
         * @brief Static method that updates the inlet and outlet nodes
         * 
         * @param time_step Current time step
         */
        static void update_nodes(const double& time_step, Eigen::Tensor<double, dim+1>& velocity_tensor, Eigen::Tensor<double, dim> density_tensor)
        {
            std::size_t inlet_size = inlet_nodes.size();
            std::size_t outlet_size = outlet_nodes.size();

            // At the given time step, update the inlet nodes by
            // calling the update function for each dimension
            Eigen::array<Eigen::Index, dim+1> coordinates;

            //#pragma acc parallel loop
            for (std::size_t i = 0; i < inlet_size; ++i)
            {
                for (std::size_t k = 0; k < dim; ++k)
                {
                    coordinates[k] = inlet_nodes[i].coords[k];
                }

                for (std::size_t j = 0; j < dim; ++j)
                {
                    coordinates[dim] = j;
                    double val = inlet_update_function[j](time_step, inlet_nodes[i]);
                    velocity_tensor(coordinates) = val;
                }
            }

            // And also update the outlet nodes
            //#pragma acc parallel loop
            for (std::size_t i = 0; i < outlet_size; ++i)
            {
                for (std::size_t k = 0; k < dim; ++k)
                {
                    coordinates[k] = outlet_nodes[i].coords[k];
                }

                for (std::size_t j = 0; j < dim; ++j)
                {
                    coordinates[dim] = j;
                    double val = outlet_update_function[j](time_step, outlet_nodes[i]);
                    velocity_tensor(coordinates) = val;
                }
            }
        }

        static void print_data()
        {
            // Print the inlet nodes
            std::cout << "Inlet nodes: " << std::endl;
            for (std::size_t i = 0; i < inlet_nodes.size(); ++i)
            {
                std::cout << "Node " << i << ": ";
                for (std::size_t j = 0; j < dim; ++j)
                {
                    std::cout << inlet_nodes[i].coords[j] << " ";
                }
                std::cout << std::endl;
            }
            // Print the outlet nodes
            std::cout << "Outlet nodes: " << std::endl;
            for (std::size_t i = 0; i < outlet_nodes.size(); ++i)
            {
                std::cout << "Node " << i << ": ";
                for (std::size_t j = 0; j < dim; ++j)
                {
                    std::cout << outlet_nodes[i].coords[j] << " ";
                }
                std::cout << std::endl;
            }
        }
    };

    template<std::size_t dim>
    std::vector<BoundaryPoint<dim>> OpenACCVelocityInitializer<dim>::inlet_nodes;
    template<std::size_t dim>
    std::vector<BoundaryPoint<dim>> OpenACCVelocityInitializer<dim>::outlet_nodes;
    template<std::size_t dim>
    std::array<std::function<double (double, BoundaryPoint<dim>)>, dim> OpenACCVelocityInitializer<dim>::inlet_update_function;
    template<std::size_t dim>
    std::array<std::function<double (double, BoundaryPoint<dim>)>, dim> OpenACCVelocityInitializer<dim>::outlet_update_function;

}; // namespace llalbm::core::initializers

#endif // LLALBM_INLET_OUTLET_INITIALIZER