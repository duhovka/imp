/**
 *  \file PairContainer.h    \brief A container for particle_pairs.
 *
 *  This file is generated by a script (core/tools/make-container).
 *  Do not edit directly.
 *
 *  Copyright 2007-9 Sali Lab. All rights reserved.
 */

#ifndef IMPCORE_PAIR_CONTAINER_H
#define IMPCORE_PAIR_CONTAINER_H

#include "config.h"
#include "internal/IndexingIterator.h"
#include "internal/container_helpers.h"
#include "Particle.h"
#include "VersionInfo.h"
#include "base_types.h"
#include "Pointer.h"
#include "VersionInfo.h"

IMP_BEGIN_NAMESPACE

//! A shared container for particle_pairs
/** Stores a searchable shared collection of particle_pairs.
    \ingroup restraints
 */
class IMPEXPORT PairContainer : public RefCounted,
                                     public Object
{
  struct Accessor {
    typedef Accessor This;
    typedef ParticlePair result_type;
    typedef unsigned int argument_type;
    result_type operator()(argument_type i) const {
      return o_->get_particle_pair(i);
    }
    Accessor(PairContainer *pc): o_(pc){}
    Accessor(){}
    IMP_COMPARISONS_1(o_);
  private:
    bool is_default() const { return false;}
    // This should be ref counted, but swig memory management is broken
    PairContainer* o_;
  };

public:
  PairContainer();

  //!
  /** \note This function may be linear. Be aware of the complexity
      bounds of your particular container.
   */
  virtual bool get_contains_particle_pair(ParticlePair p) const =0;
  //! return the number of particle_pairs in the container
  /** \note this isn't always constant time
   */
  virtual unsigned int get_number_of_particle_pairs() const =0;

  //! get one particle_pair
  virtual ParticlePair get_particle_pair(unsigned int i) const=0;

#ifdef IMP_DOXYGEN
  //! An iterator through the contents of the container
  class ParticlePairIterator;
#else
  typedef internal::IndexingIterator<Accessor> ParticlePairIterator;
#endif
  //! begin iterating through the particle_pairs
  ParticlePairIterator particle_pairs_begin() const {
    // Since I can't make the count mutable in Object
    return
      ParticlePairIterator(Accessor(const_cast<PairContainer*>(this)),
                        0);
  }
  //! iterate through the particle_pairs
  ParticlePairIterator particle_pairs_end() const {
    return
      ParticlePairIterator(Accessor(const_cast<PairContainer*>(this)),
                        get_number_of_particle_pairs());
    }

  IMP_REF_COUNTED_DESTRUCTOR(PairContainer)
};

IMP_OUTPUT_OPERATOR(PairContainer);

//! A collection of containers
typedef std::vector<PairContainer*> PairContainers;

IMP_END_NAMESPACE

#endif  /* IMPCORE_PAIR_CONTAINER_H */
