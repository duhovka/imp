/**
 *  \file IMP/em/EnvelopePenetrationRestraint.h
 *  \brief Score how well a protein is inside its density
 *
 *  Copyright 2007-2016 IMP Inventors. All rights reserved.
 *
 */

#ifndef IMPEM_ENVELOPE_PENETRATION_RESTRAINT_H
#define IMPEM_ENVELOPE_PENETRATION_RESTRAINT_H

#include <IMP/em/em_config.h>
#include "DensityMap.h"
#include <IMP/atom/Hierarchy.h>
#include <IMP/atom/Atom.h>
#include <IMP/atom/Mass.h>
#include <IMP/core/XYZR.h>
#include <IMP/Model.h>
#include <IMP/Restraint.h>
#include <IMP/Refiner.h>

IMPEM_BEGIN_NAMESPACE

//! Calculate score based on fit to EM map
/** \ingroup exp_restraint

 */
class IMPEMEXPORT EnvelopePenetrationRestraint : public Restraint {
 public:
  //! Constructor
  /**
    \param[in] ps The particles participating in the fitting score
    \param[in] em_map  The density map used in the fitting score
    \param[in] threshold

    \note Particles that are rigid-bodies are interpolated and
     not resampled.
          This significantly reduces the running time but is less accurate.
          If the user prefers to get more accurate results, provide
          its members as input particles and not the rigid body.
    \todo we currently assume rigid bodies are also molecular hierarchies.
   */
  EnvelopePenetrationRestraint(Particles ps, DensityMap *em_map,
                               Float threshold);

  virtual double unprotected_evaluate(IMP::DerivativeAccumulator *accum)
      const IMP_OVERRIDE;
  virtual IMP::ModelObjectsTemp do_get_inputs() const IMP_OVERRIDE;
  IMP_OBJECT_METHODS(EnvelopePenetrationRestraint);

#ifndef SWIG
  IMP_LIST(private, Particle, particle, Particle *, Particles);
#endif
 private:
  IMP::PointerMember<DensityMap> target_dens_map_;
  algebra::BoundingBoxD<3> target_bounding_box_;
  // reference to the IMP environment
  IMP::core::XYZs xyz_;
  Particles ps_;
  Float threshold_;
};

IMPEM_END_NAMESPACE

#endif /* IMPEM_ENVELOPE_PENETRATION_RESTRAINT_H */
