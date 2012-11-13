/**
 *  \file IMP/container/MinimumQuadScore.h
 *  \brief Define QuadScore.
 *
 *  This file is generated by a script (tools/make-container).
 *  Do not edit directly.
 *
 *  Copyright 2007-2012 IMP Inventors. All rights reserved.
 */

#ifndef IMPCONTAINER_MINIMUM_QUAD_SCORE_H
#define IMPCONTAINER_MINIMUM_QUAD_SCORE_H

#include "container_config.h"
#include <IMP/QuadScore.h>
#include <IMP/quad_macros.h>

IMPCONTAINER_BEGIN_NAMESPACE

//! Evaluate the min or max n particle_quad scores of the passed set
/** Each of the set of QuadScores is evaluated and the sum of the
    minimum n is returned.
*/
class IMPCONTAINEREXPORT MinimumQuadScore : public QuadScore
{
  QuadScores scores_;
  unsigned int n_;
public:
  MinimumQuadScore(const QuadScoresTemp &scores,
                       unsigned int n=1,
                       std::string name="QuadScore %1%");
  IMP_INDEX_QUAD_SCORE(MinimumQuadScore);

  Restraints create_current_decomposition(Model *m,
                                          const ParticleIndexQuad& vt)
      const IMP_OVERRIDE;
};

IMP_OBJECTS(MinimumQuadScore,MinimumQuadScores);

IMPCONTAINER_END_NAMESPACE

#endif  /* IMPCONTAINER_MINIMUM_QUAD_SCORE_H */
