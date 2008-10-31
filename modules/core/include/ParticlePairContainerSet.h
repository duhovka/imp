/**
 *  \file ParticlePairContainerSet.h
 *  \brief Store a set of ParticlePairContainers
 *
 *  This file is generated by a script (core/tools/make-container).
 *  Do not edit directly.
 *
 *  Copyright 2007-8 Sali Lab. All rights reserved.
 */

#ifndef IMPCORE_PARTICLE_PAIR_CONTAINER_SET_H
#define IMPCORE_PARTICLE_PAIR_CONTAINER_SET_H

#include "core_exports.h"
#include "ParticlePairContainer.h"
#include "internal/core_version_info.h"

IMPCORE_BEGIN_NAMESPACE

//! Stores a set of ParticlePairContainers
/**
 */
class IMPCOREEXPORT ParticlePairContainerSet
  : public ParticlePairContainer
{
public:
  ParticlePairContainerSet();

  virtual ~ParticlePairContainerSet();

  IMP_PARTICLE_PAIR_CONTAINER(internal::core_version_info);

  IMP_CONTAINER(ParticlePairContainer, particle_pair_container,
                ParticlePairContainerIndex);
};


IMPCORE_END_NAMESPACE

#endif  /* IMPCORE_PARTICLE_PAIR_CONTAINER_SET_H */
