/// DSGRN.hpp
/// Shaun Harker
/// 2015-05-29
/// This file provides implementations for the user classes of DSGRN
/// Note: These implementations tend to be defined by macros since they
///       are rather repetitious. The methods in this file merely redirect
///       from the shared_ptr wrapper to the underlying implementation class.
///       See "Idiom.h" for the macro definitions.

#ifndef DSGRN_HPP
#define DSGRN_HPP

#include "Graph/Components.hpp"
#include "Graph/Digraph.hpp"
#include "Graph/Poset.hpp"
#include "Graph/StrongComponents.hpp"
#include "Dynamics/Annotation.hpp"
#include "Dynamics/MorseDecomposition.hpp"
#include "Dynamics/MorseGraph.hpp"
#include "Parameter/Network.hpp"
#include "Parameter/LogicParameter.hpp"
#include "Parameter/OrderParameter.hpp"
#include "Parameter/Parameter.hpp"
#include "Parameter/ParameterGraph.hpp"
#include "Phase/Domain.hpp"
#include "Phase/DomainGraph.hpp"
#include "Phase/Wall.hpp"
#include "Phase/WallGraph.hpp"

#endif
