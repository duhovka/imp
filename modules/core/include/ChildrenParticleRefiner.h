/**
 *  \file ChildrenParticleRefiner.h
 *  \brief Return the hierarchy children of a particle.
 *
 *  Copyright 2007-8 Sali Lab. All rights reserved.
 */

#ifndef IMPCORE_CHILDREN_PARTICLE_REFINER_H
#define IMPCORE_CHILDREN_PARTICLE_REFINER_H

#include "config.h"
#include "internal/core_version_info.h"
#include "HierarchyDecorator.h"

#include <IMP/ParticleRefiner.h>

IMPCORE_BEGIN_NAMESPACE

class HierarchyTraits;

//! Return the hierarchy children of a particle.
/** \ingroup hierarchy
    A simple example using it
    \verbinclude simple_examples/cover_particles.py
*/
class IMPCOREEXPORT ChildrenParticleRefiner : public ParticleRefiner
{

  HierarchyTraits traits_;
public:
  //! create a refiner for a particular type of hierarchy
  ChildrenParticleRefiner(HierarchyTraits tr
                          = internal::get_default_hierarchy_traits());

  virtual ~ChildrenParticleRefiner() {}

  IMP_PARTICLE_REFINER(internal::core_version_info);
};

IMPCORE_END_NAMESPACE

#endif  /* IMPCORE_CHILDREN_PARTICLE_REFINER_H */
