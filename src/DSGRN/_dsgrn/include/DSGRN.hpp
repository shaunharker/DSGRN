/// DSGRN.hpp
/// Shaun Harker
/// 2015-05-29
/// 
/// Note: include this file for header-only use

#pragma once

#define INLINE_IF_HEADER_ONLY inline 

#include "Graph/Components.hpp"
#include "Graph/Digraph.hpp"
#include "Graph/Poset.hpp"
#include "Graph/StrongComponents.hpp"
#include "Graph/LabelledMultidigraph.hpp"
#include "Dynamics/Annotation.hpp"
#include "Dynamics/MorseDecomposition.hpp"
#include "Dynamics/MorseGraph.hpp"
#include "Parameter/Network.hpp"
#include "Parameter/LogicParameter.hpp"
#include "Parameter/OrderParameter.hpp"
#include "Parameter/Parameter.hpp"
#include "Parameter/ParameterGraph.hpp"
#include "Parameter/Configuration.h"
#include "Parameter/ParameterSampler.hpp"
#include "Phase/Domain.hpp"
#include "Phase/DomainGraph.hpp"
#include "Phase/Wall.hpp"
#include "Phase/WallGraph.hpp"
#include "Pattern/MatchingGraph.hpp"
#include "Pattern/MatchingRelation.hpp"
#include "Pattern/Pattern.hpp"
#include "Pattern/PatternGraph.hpp"
#include "Pattern/PatternMatch.hpp"
#include "Pattern/SearchGraph.hpp"
#include "Query/ComputeSingleGeneQuery.hpp"
#include "Query/NFA.hpp"
#include "Query/ThompsonsConstruction.hpp"
