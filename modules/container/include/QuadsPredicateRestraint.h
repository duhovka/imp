/**
 *  \file QuadsPredicateRestraint.h
 *  \brief Apply a QuadScore to each Quad in a list.
 *
 *  This file is generated by a script (core/tools/make-container).
 *  Do not edit directly.
 *
 *  Copyright 2007-2012 IMP Inventors. All rights reserved.
 *
 */

#ifndef IMPCONTAINER_QUADS_PREDICATE_RESTRAINT_H
#define IMPCONTAINER_QUADS_PREDICATE_RESTRAINT_H

#include "container_config.h"

#include <IMP/core/internal/CoreQuadsRestraint.h>
#include "ListQuadContainer.h"
#include <IMP/compatibility/map.h>
#include <IMP/QuadPredicate.h>
#include "generic.h"

#include <iostream>

IMPCONTAINER_BEGIN_NAMESPACE

//! Applies a QuadScore to each Quad in a list based on a predicate
/** This restraint uses a passed predicate to choose which score to apply
    to each tuple in the input container. The selections are cached, making it
    substantially faster than using a core::TypedPairScore. */
class IMPCONTAINEREXPORT QuadsPredicateRestraint :
public Restraint
{
  OwnerPointer<QuadPredicate> predicate_;
  OwnerPointer<QuadContainer> input_;
  typedef compatibility::map<unsigned int, Pointer<ListQuadContainer> >
      Map;
  Map containers_;
  Pointer<ListQuadContainer> unknown_container_;
  Restraints restraints_;
  mutable bool updated_;
  bool error_on_unknown_;
  void update_lists_if_necessary() const;
  void assign_pair(const ParticleIndexQuad& index) const;
public:
  QuadsPredicateRestraint(QuadPredicate *pred,
                      QuadContainer *input,
                      std::string name="QuadsPredicateRestraint %1%");

  /** This version uses the container::create_restraint() function and so
      is more efficient than the non-template version.*/
  template <class Score>
      void set_score(int predicate_value, Score *score) {
    IMP_USAGE_CHECK(get_is_part_of_model(),
                    "You must add this restraint to the model"
                    << " first, sorry, this can be fixed.");
    IMP_NEW(ListQuadContainer, c, (get_model()));
    restraints_.push_back(container::create_restraint(score, c.get()));
    restraints_.back()->set_model(get_model());
    containers_[predicate_value]=c;
  }
template <class Score>
      void set_unknown_score( Score *score) {
  // make sure it gets cleaned up if it is a temporary
  Pointer<Score> pscore(score);
    IMP_USAGE_CHECK(get_is_part_of_model(),
                    "You must add this restraint to the model"
                    << " first, sorry, this can be fixed.");
    IMP_NEW(ListQuadContainer, c, (get_model()));
    restraints_.push_back(container::create_restraint(score, c.get()));
    restraints_.back()->set_model(get_model());
    unknown_container_=c;
  }
  /** By default, it is an error if the predicate returns a value that is
      not known. If this is false, then they are silently skipped.
  */
  void set_is_complete(bool tf) {
    error_on_unknown_=tf;
  }

  IMP_RESTRAINT(QuadsPredicateRestraint);
private:
  Restraints do_create_current_decomposition() const;
};

IMPCONTAINER_END_NAMESPACE

#endif  /* IMPCONTAINER_QUADS_PREDICATE_RESTRAINT_H */
