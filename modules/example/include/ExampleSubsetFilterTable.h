/**
 *  \file IMP/example/ExampleSubsetFilterTable.h
 *  \brief A Score on the distance between a pair of particles.
 *
 *  Copyright 2007-2012 IMP Inventors. All rights reserved.
 */

#ifndef IMPEXAMPLE_EXAMPLE_SUBSET_FILTER_TABLE_H
#define IMPEXAMPLE_EXAMPLE_SUBSET_FILTER_TABLE_H

#include <IMP/example/example_config.h>
#include <IMP/domino/subset_filters.h>
#include <IMP/domino/domino_macros.h>

IMPEXAMPLE_BEGIN_NAMESPACE

/** Filter a set of particles based on the maximum difference
    in state indexes between them (a completely silly criteria).
    The thing to note is how the filter table determines when
    it should be applied and how it sets up the filters.
    \include ExampleSubsetFilterTable.h
    \include ExampleSubsetFilterTable.cpp
*/
class IMPEXAMPLEEXPORT ExampleSubsetFilterTable:
  public domino::SubsetFilterTable
{
  int max_diff_;
  const Particles ps_;
  Ints get_indexes(const domino::Subset &s,
                   const domino::Subsets& prior_subsets) const;
 public:
  ExampleSubsetFilterTable(unsigned int max_diff, const ParticlesTemp &pt);

  IMP_SUBSET_FILTER_TABLE(ExampleSubsetFilterTable);
};


IMPEXAMPLE_END_NAMESPACE

#endif  /* IMPEXAMPLE_EXAMPLE_SUBSET_FILTER_TABLE_H */
