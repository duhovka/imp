/**
 *  \file RestraintGraph.cpp
 *  \brief creates a MRF from a set of particles and restraints
 *
 *  Copyright 2007-2010 IMP Inventors. All rights reserved.
 *
 */

#include <IMP/domino2/internal/inference_utility.h>
#include <IMP/domino2/subset_states.h>
#include <algorithm>
#include <boost/graph/copy.hpp>
#include <boost/pending/indirect_cmp.hpp>


IMPDOMINO2_BEGIN_INTERNAL_NAMESPACE


namespace {

  std::pair<Subset, NodeData>
  get_best_conformations_internal(const SubsetGraph &jt,
                                  unsigned int root,
                                  unsigned int parent,
                                  const Subset& all,
                                  const SubsetFilterTables &filters,
                                  const SubsetStatesTable *states) {
    SubsetMap subset_map= boost::get(boost::vertex_name, jt);
    Subset s= boost::get(subset_map, root);
    IMP_LOG(VERBOSE, "Looking at subset " << s << std::endl);
    IncreaseIndent ii;
    NodeData nd= get_node_data(s, states);
    IMP_LOG(VERBOSE, "Subset data is\n" << nd << std::endl);
    for (std::pair<NeighborIterator, NeighborIterator> be
           = boost::adjacent_vertices(root, jt);
         be.first != be.second; ++be.first) {
      if (*be.first == parent) continue;
      // compute intersection set and index map in one direction
      // for each pattern of that in me, compute subset score
      // subtract the min of mine (assume scores positive)
      // for merged score, subtract off edge value
      std::pair<Subset, NodeData> cpd
        = get_best_conformations_internal(jt, *be.first, root, all,
                                          filters,
                                          states);
      EdgeData ed= get_edge_data(s, cpd.first, filters);
      nd= get_union(s, cpd.first, nd, cpd.second, ed);
      s= ed.union_subset;
      IMP_LOG(VERBOSE, "After merge, set is " << s
              << " and data is\n" << nd << std::endl);
    }
    return std::make_pair(s, nd);
  }
}

SubsetStates get_best_conformations(const SubsetGraph &jt,
                                      int root,
                                      const Subset& all_particles,
                                      const SubsetFilterTables &filters,
                                      const SubsetStatesTable *states) {
  return get_best_conformations_internal(jt, root, root,
                                         all_particles,
                                         filters,
                                         states).second.subset_states;
}


IMPDOMINO2_END_INTERNAL_NAMESPACE
