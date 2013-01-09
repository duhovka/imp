/**
 *  \file IMP/multifit/anchor_utilities.h
 *  \brief Common functions in anchor calculations
 *
 *  Copyright 2007-2012 IMP Inventors. All rights reserved.
 *
 */

#ifndef IMPMULTIFIT_ANCHOR_UTILITIES_H
#define IMPMULTIFIT_ANCHOR_UTILITIES_H

#include <IMP/multifit/multifit_config.h>
#include <IMP/atom/Hierarchy.h>
#include <IMP/statistics/VQClustering.h>
#include <IMP/statistics/DataPoints.h>
#include "DataPointsAssignment.h"
#include "anchors_reader.h"

IMPMULTIFIT_BEGIN_NAMESPACE

IMPMULTIFITEXPORT
inline AnchorsData molecule2anchors(atom::Hierarchy mh,int k) {
  IMP_NEW(IMP::statistics::ParticlesDataPoints,ddp,(core::get_leaves(mh)));
  IMP::statistics::VQClustering vq(ddp,k);
  vq.run();
  multifit::DataPointsAssignment assignment(ddp,&vq);
  multifit::AnchorsData ad(
                          assignment.get_centers(),
                          *(assignment.get_edges()));
  return ad;
}

IMPMULTIFITEXPORT
//! Generate anchors in several formats for a given density map.
void get_anchors_for_density(em::DensityMap *dmap, int number_of_means,
                             float density_threshold,
                             std::string pdb_filename,
                             std::string cmm_filename,
                             std::string seg_filename,
                             std::string txt_filename);

IMPMULTIFIT_END_NAMESPACE

#endif /* IMPMULTIFIT_ANCHOR_UTILITIES_H */
