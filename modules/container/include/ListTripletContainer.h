/**
 *  \file ListTripletContainer.h    \brief Store a list of ParticleTripletsTemp
 *
 *  This file is generated by a script (core/tools/make-container).
 *  Do not edit directly.
 *
 *  Copyright 2007-2012 IMP Inventors. All rights reserved.
 */

#ifndef IMPCONTAINER_LIST_TRIPLET_CONTAINER_H
#define IMPCONTAINER_LIST_TRIPLET_CONTAINER_H

#include "container_config.h"
#include <IMP/internal/InternalListTripletContainer.h>

IMPCONTAINER_BEGIN_NAMESPACE

//! Store a list of ParticleTripletsTemp
/** \note The indexes can change when particles are inserted
    as the list is maintained in sorted order.
 */
class IMPCONTAINEREXPORT ListTripletContainer:
#if defined(IMP_DOXYGEN) || defined(SWIG)
public TripletContainer
#else
public IMP::internal::InternalListTripletContainer
#endif
{
  typedef IMP::internal::InternalListTripletContainer P;
public:
  ListTripletContainer(const ParticleTripletsTemp &ps);

  //! construct and pass an initial set of Triplets
  ListTripletContainer(const ParticleTripletsTemp &ps,
                         std::string name);

  ListTripletContainer(Model *m,
                         std::string name= "ListTripletContainer %1%");
  ListTripletContainer(Model *m, const char *name);

#if defined(IMP_DOXYGEN) || defined(SWIG)
 /** @name Methods to control the contained objects

     This container stores a list of Triplet objects. To manipulate
     the list use these methods.
  */
  /**@{*/
  void add_particle_triplet(const ParticleTriplet& vt);
  void add_particle_triplets(const ParticleTripletsTemp &c);
  void set_particle_triplets(ParticleTripletsTemp c);
  void clear_particle_triplets();
  /**@}*/
  IMP_NO_DOXYGEN(bool get_is_up_to_date() const);
  IMP_TRIPLET_CONTAINER(ListTripletContainer);
#else
  IMP_OBJECT(ListTripletContainer);
#endif
};

IMP_OBJECTS(ListTripletContainer,ListTripletContainers);

IMPCONTAINER_END_NAMESPACE

#endif  /* IMPCONTAINER_LIST_TRIPLET_CONTAINER_H */
