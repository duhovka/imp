/**
 *  \file IMP/core/PairConstraint.h
 *  \brief Use a PairModifier applied to a ParticlePairsTemp to
 *  maintain an invariant
 *
 *  This file is generated by a script (core/tools/make-container).
 *  Do not edit directly.
 *
 *  Copyright 2007-2012 IMP Inventors. All rights reserved.
 */

#ifndef IMPCORE_PAIR_CONSTRAINT_H
#define IMPCORE_PAIR_CONSTRAINT_H

#include <IMP/core/core_config.h>
#include <IMP/internal/TupleConstraint.h>
#include <IMP/PairModifier.h>
#include <IMP/PairDerivativeModifier.h>
#include <IMP/score_state_macros.h>

IMPCORE_BEGIN_NAMESPACE
//! Apply a PairFunction to a Pair
/** The score state is passed up to two PairModifiers, one to
    apply before evaluation and the other after. The one after
    should take a DerivativeAccumulator as its last argument for
    PairModifier::apply() and will only be called if
    the score was computed with derivatives.

    \see container::PairsConstraint
 */
class IMPCOREEXPORT PairConstraint :
#if defined(IMP_DOXYGEN) || defined(SWIG)
public Constraint
#else
public IMP::internal::TupleConstraint<PairModifier,
                                      PairDerivativeModifier>
#endif
{
public:
  /** before and after are the modifiers to apply before and after
      evaluate.
   */
  PairConstraint(PairModifier *before,
                      PairDerivativeModifier *after,
                      const ParticlePair& vt,
                      std::string name="PairConstraint %1%"):
      IMP::internal::TupleConstraint<PairModifier,
                                      PairDerivativeModifier>
      (before, after, vt, name)
      {
  }

#if defined(IMP_DOXYGEN) || defined(SWIG)
  IMP_CONSTRAINT(PairConstraint);
#endif
};


IMPCORE_END_NAMESPACE

#endif  /* IMPCORE_PAIR_CONSTRAINT_H */
