// $Id$
//==============================================================================
//!
//! \file CoSTA_AdvectionDiffusion.C
//!
//! \date Sep 9 2021
//!
//! \author Arne Morten Kvarving / SINTEF
//!
//! \brief Exports the AdvectionDiffusion solver to the IFEM_CoSTA module.
//!
//==============================================================================

#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include "CoSTAModule.h"
#include "AdvectionDiffusionBDF.h"
#include "SIMAD.h"


template<class Dim> using SIMAdv = SIMAD<Dim,AdvectionDiffusionBDF>; //!< One-parameter type alias for SIMAD


//! \brief Specialization for SIMAdv.
template<>
struct CoSTASIMAllocator<SIMAdv> {
  //! \brief Method to allocate a given dimensionality of a SIMAdv.
  //! \param newModel Simulator to allocate
  //! \param model Pointer to SIMbase interface for simulator
  //! \param solModel Pointer to SIMsolution interface for simulatorA
  //! \param infile Input file to parse.
  template<class Dim>
  void allocate(std::unique_ptr<SIMAdv<Dim>>& newModel, SIMbase*& model,
                SIMsolution*& solModel, const std::string& infile)
  {
    integrand = std::make_unique<AdvectionDiffusionBDF>(Dim::dimension,
                                                        TimeIntegration::BE,
                                                        Integrand::STANDARD);
    newModel = std::make_unique<SIMAdv<Dim>>(*integrand, true);
    model = newModel.get();
    solModel = newModel.get();
    if (ConfigureSIM(*newModel, const_cast<char*>(infile.c_str())))
      throw std::runtime_error("Error reading input file");
  }

   std::unique_ptr<AdvectionDiffusionBDF> integrand; //!< Pointer to integrand instance
};


void export_AdvectionDiffusion(pybind11::module& m)
{
  pybind11::class_<CoSTAModule<SIMAdv>>(m, "AdvectionDiffusion")
  .def(pybind11::init<const std::string&>())
  .def("correct", &CoSTAModule<SIMAdv>::correct)
  .def("predict", &CoSTAModule<SIMAdv>::predict)
  .def("residual", &CoSTAModule<SIMAdv>::residual)
  .def_readonly("ndof", &CoSTAModule<SIMAdv>::ndof);
}
