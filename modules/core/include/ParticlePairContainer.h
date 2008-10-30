/**
 *  \file ParticlePairContainer.h    \brief A container for particle_pairs.
 *
 *  This file is generated by a script (core/tools/make-container).
 *  Do not edit directly.
 *
 *  Copyright 2007-8 Sali Lab. All rights reserved.
 */

#ifndef IMPCORE_PARTICLE_PAIR_CONTAINER_H
#define IMPCORE_PARTICLE_PAIR_CONTAINER_H

#include "core_exports.h"
#include "internal/IndexingIterator.h"
#include <IMP/Particle.h>
#include <IMP/base_types.h>
#include <IMP/Pointer.h>

IMPCORE_BEGIN_NAMESPACE

//! A shared container for particle_pairs
/** Stores a searchable shared collection of particle_pairs.
 */
class IMPCOREEXPORT ParticlePairContainer : public RefCountedObject
{
  struct Accessor {
    typedef Accessor This;
    typedef ParticlePair result_type;
    typedef unsigned int argument_type;
    result_type operator()(argument_type i) const {
      return o_->get_particle_pair(i);
    }
    Accessor(ParticlePairContainer *pc): o_(pc){}
    Accessor(){}
    IMP_COMPARISONS_1(o_);
  private:
    bool is_default() const { return false;}
    Pointer<ParticlePairContainer> o_;
  };

public:
  ParticlePairContainer();

  virtual ~ParticlePairContainer();

  //!
  /** \note This function may be linear. Be aware of the complexity
      bounds of your particular container.
   */
  virtual bool get_contains_particle_pair(ParticlePair p) const =0;
  //!
  virtual unsigned int get_number_of_particle_pairs() const =0;
  //!
  virtual ParticlePair get_particle_pair(unsigned int i) const=0;

  virtual void show(std::ostream &out = std::cout) const;

  virtual VersionInfo get_version_info() const=0;

  //!
  typedef internal::IndexingIterator<Accessor> ParticlePairIterator;
  //!
  ParticlePairIterator particle_pairs_begin() const {
    // Since I can't make the count mutable in Object
    return
      ParticlePairIterator(Accessor(const_cast<ParticlePairContainer*>(this)),
                        0);
  }
  //!
  ParticlePairIterator particle_pairs_end() const {
    return
      ParticlePairIterator(Accessor(const_cast<ParticlePairContainer*>(this)),
                        get_number_of_particle_pairs());
    }

  /** \todo patch ref_counting.h so this is not needed.
   */
  unsigned int get_index() const {return -1;}
};

IMP_OUTPUT_OPERATOR(ParticlePairContainer);

typedef std::vector<ParticlePairContainer*> ParticlePairContainers;
typedef Index<ParticlePairContainer> ParticlePairContainerIndex;

IMPCORE_END_NAMESPACE

#define IMP_PARTICLE_PAIR_CONTAINER(version_info)                      \
  bool get_contains_particle_pair(ParticlePair p) const;                      \
  unsigned int get_number_of_particle_pairs() const;                   \
  ParticlePair get_particle_pair(unsigned int i) const;                       \
  void show(std::ostream &out= std::cout) const;                   \
  IMP::VersionInfo get_version_info() const { return version_info; }

#endif  /* IMPCORE_PARTICLE_PAIR_CONTAINER_H */
