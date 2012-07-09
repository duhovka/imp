/**
 *  \file PredicatePairsRestraint.h
 *  \brief Apply a PairScore to each Pair in a list.
 *
 *  This file is generated by a script (core/tools/make-container).
 *  Do not edit directly.
 *
 *  Copyright 2007-2012 IMP Inventors. All rights reserved.
 *
 */

#ifndef IMPCONTAINER_PREDICATE_PAIRS_RESTRAINT_H
#define IMPCONTAINER_PREDICATE_PAIRS_RESTRAINT_H

#include "container_config.h"

#include <IMP/internal/InternalDynamicListPairContainer.h>
#include <IMP/compatibility/map.h>
#include <IMP/PairPredicate.h>
#include <IMP/restraint_macros.h>
#include "generic.h"

#include <iostream>

IMPCONTAINER_BEGIN_NAMESPACE

//! Applies a PairScore to each Pair in a list based on a predicate
/** This restraint uses a passed predicate to choose which score to apply
    to each tuple in the input container. The selections are cached, making it
    substantially faster than using a core::TypedPairScore.

    \note The ordering of particles within a tuple may vary depending on the
    input container used. You may need to call set_score() with several
    different predicate values for different orderings.
*/
class IMPCONTAINEREXPORT PredicatePairsRestraint :
public Restraint
{
  OwnerPointer<PairPredicate> predicate_;
  OwnerPointer<PairContainer> input_;
  typedef IMP::internal::InternalDynamicListPairContainer List;
  typedef compatibility::map<unsigned int, Pointer<List> >
      Map;
  Map containers_;
  Pointer<List> unknown_container_;
  Restraints restraints_;
  mutable bool updated_;
  bool error_on_unknown_;
  void update_lists_if_necessary() const;
  bool assign_pair(const ParticleIndexPair& index) const;
public:
  PredicatePairsRestraint(PairPredicate *pred,
                      PairContainerAdaptor input,
                      std::string name="PredicatePairsRestraint %1%");

  /** Apply the passed score to all pairs whose predicate values match
      the passed value.

      This version uses the container::create_restraint() function and so
      is more efficient than the non-template version.*/
  template <class Score>
  void set_score(int predicate_value, Score *score) {
    IMP_USAGE_CHECK(get_is_part_of_model(),
                    "You must add this restraint to the model"
                    << " first, sorry, this can be fixed.");
    IMP_NEW(List, c, (input_,
                      score->get_name()+" input"));
    restraints_.push_back(container::create_restraint(score, c.get()));
    restraints_.back()->set_model(get_model());
    restraints_.back()->set_was_used(true);
    containers_[predicate_value]=c;
  }

  /** Apply this score to any pair whose predicate value does not match
      one passed to set_score().*/
  template <class Score>
  void set_unknown_score( Score *score) {
  // make sure it gets cleaned up if it is a temporary
    Pointer<Score> pscore(score);
    IMP_USAGE_CHECK(get_is_part_of_model(),
                    "You must add this restraint to the model"
                    << " first, sorry, this can be fixed.");
    IMP_NEW(List, c, (input_,
                      score->get_name()+" input"));
    restraints_.push_back(container::create_restraint(score, c.get()));
    restraints_.back()->set_model(get_model());
    unknown_container_=c;
  }
#ifndef IMP_DOXYGEN
  void set_score(int predicate_value, PairScore *score) {
    set_score<PairScore>(predicate_value, score);
  }
  void set_unknown_score(PairScore *score) {
    set_unknown_score<PairScore>(score);
  }
#endif
  /** By default, it is an error if the predicate returns a value that is
      not known. If this is false, then they are silently skipped.
  */
  void set_is_complete(bool tf) {
    error_on_unknown_=tf;
  }

  ParticlePairsTemp get_particle_pairs(int predicate_value) const {
    return containers_.find(predicate_value)->second
      ->get_particle_pairs();
  }

  IMP_RESTRAINT(PredicatePairsRestraint);
private:
  Restraints do_create_current_decomposition() const;
};

IMPCONTAINER_END_NAMESPACE

#endif  /* IMPCONTAINER_PREDICATE_PAIRS_RESTRAINT_H */
