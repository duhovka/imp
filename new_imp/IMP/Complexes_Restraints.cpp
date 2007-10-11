/*
 *  Complexes_Restraints.cpp
 *  IMP
 *
 *  Copyright 2007 Sali Lab. All rights reserved.
 *
 */

#include <cmath>

#include "Complexes_Restraints.h"
#include "Model.h"
#include "Particle.h"
#include "log.h"
#include "emscore.h"
#include "mystdexcept.h"

namespace imp
{

//######### RSR_Coordinate Restraint #########
// Optimize based on distance from an absolute position

/**
  Constructor - set up the values and indexes for this coordinate restraint.

  \param[in] model Pointer to the model.
  \param[in] p1 Pointer to particle of the restraint.
  \param[in] axis String indicating the axis of absolute reference:
      X_AXIS, Y_AXIS, Z_AXIS, XY_RADIAL, XZ_RADIAL, YZ_RADIAL, XYZ_SPHERE
  \param[in] score_func_params Parameters for creating a score function.
 */

RSR_Coordinate::RSR_Coordinate(Model& model,
                               Particle* p1,
                               const std::string axis,
                               Basic_Score_Func_Params* score_func_params)
{
  model_data_ = model.get_model_data();

  particles_.push_back(p1);
  x1_ = p1->float_index(std::string("X"));
  y1_ = p1->float_index(std::string("Y"));
  z1_ = p1->float_index(std::string("Z"));

  axis_ = axis;
  score_func_ = score_func_params->create_score_func();
}

/**
  Destructor
 */

RSR_Coordinate::~RSR_Coordinate ()
{
}

/**
  Calculate the score for this coordinate restraint based on the current
  state of the model.

 \param[in] calc_deriv If true, partial first derivatives should be calculated.
  */

Float RSR_Coordinate::evaluate(bool calc_deriv)
{
  Float score = 0.0, deriv;
  Float x, y, z;
  Float dx = 0.0;
  Float dy = 0.0;
  Float dz = 0.0;
  Float current_distance;

  // get current position of particle
  x = model_data_->get_float(x1_);
  y = model_data_->get_float(y1_);
  z = model_data_->get_float(z1_);

  // restrain the x coordinate
  if (axis_ == "X_AXIS") {
    score = (*score_func_)(x, dx);
  }

  // restrain the y coordinate
  else if (axis_ == "Y_AXIS") {
    score = (*score_func_)(y, dy);
  }

  // restrain the z coordinate
  else if (axis_ == "Z_AXIS") {
    score = (*score_func_)(z, dz);
  }

  // restrain the xy radial distance
  else if (axis_ == "XY_RADIAL") {
    current_distance = sqrt(x * x + y * y);
    score = (*score_func_)(sqrt(x * x + y * y), deriv);
    dx = x / current_distance * deriv;
    dy = y / current_distance * deriv;
  }

  // restrain the xz radial distance
  else if (axis_ == "XZ_RADIAL") {
    current_distance = sqrt(x * x + z * z);
    score = (*score_func_)(current_distance, deriv);
    dx = x / current_distance * deriv;
    dz = z / current_distance * deriv;
  }

  // restrain the yz radial distance
  else if (axis_ == "YZ_RADIAL") {
    current_distance = sqrt(y * y + z * z);
    score = (*score_func_)(current_distance, deriv);
    dy = y / current_distance * deriv;
    dz = z / current_distance * deriv;
  }

  // restrain the xyz spherical distance (Euclidian distance from origin)
  else if (axis_ == "XYZ_SPHERE") {
    current_distance = sqrt(x * x + y * y + z * z);
    score = (*score_func_)(current_distance, deriv);
    dx = x / current_distance * deriv;
    dy = y / current_distance * deriv;
    dz = z / current_distance * deriv;
  }

  // if needed, use the partial derivatives
  if (calc_deriv) {
    model_data_->add_to_deriv(x1_, dx);
    model_data_->add_to_deriv(y1_, dy);
    model_data_->add_to_deriv(z1_, dz);
  }

  IMP_LOG(VERBOSE, axis_ << " score: " << score << "  x: " << x << " y: " << y << " z: " << z << "   dx: " << dx << " dy: " << dy << " dz: " << dz << std::endl);

  return score;
}


/**
  Show the current restraint.

 \param[in] out Stream to send restraint description to.
 */

void RSR_Coordinate::show(std::ostream& out) const
{
  if (is_active()) {
    out << "coordinate restraint (active):" << std::endl;
  } else {
    out << "coordinate restraint (inactive):" << std::endl;
  }

  out << "version: " << version() << "  " << "last_modified_by: " << last_modified_by() << std::endl;
  out << "  x1:" << model_data_->get_float(x1_);
  out << "  y1:" << model_data_->get_float(y1_);
  out << "  z1:" << model_data_->get_float(z1_) << std::endl;

  out << "  dx1:" << model_data_->get_deriv(x1_);
  out << "  dy1:" << model_data_->get_deriv(y1_);
  out << "  dz1:" << model_data_->get_deriv(z1_) << std::endl;

  out << "  axis:" << axis_;
}


//######### RSR_Torus Restraint #########
// Optimize based on distance from torus interior

/**
  Constructor - set up the values and indexes for this torus restraint. Expects
 coordinates to be labeled "X", "Y", and "Z" in the particles.

  \param[in] model Pointer to the model.
  \param[in] p1 Pointer to particle of the restraint.
  \param[in] main_radius The main radius from the origin to the midline of the tube.
  \param[in] tube_radius The tube radius is min distance from the tube midline to the tube surface.
  \param[in] score_func_params Parameters for creating a score function.
 */

RSR_Torus::RSR_Torus(Model& model,
                     Particle* p1,
                     const Float main_radius,
                     const Float tube_radius,
                     Basic_Score_Func_Params* score_func_params)
{
  model_data_ = model.get_model_data();

  particles_.push_back(p1);
  x1_ = p1->float_index(std::string("X"));
  y1_ = p1->float_index(std::string("Y"));
  z1_ = p1->float_index(std::string("Z"));

  main_radius_ = main_radius;
  tube_radius_ = tube_radius;
  score_func_ = score_func_params->create_score_func();
}

/**
  Destructor
 */

RSR_Torus::~RSR_Torus ()
{
}

/**
  Calculate the score for this coordinate restraint based on the current
  state of the model.

 \param[in] calc_deriv If true, partial first derivatives should be calculated.
  */

Float RSR_Torus::evaluate(bool calc_deriv)
{
  Float tube_center_x, tube_center_y;
  Float xy_distance_from_center;
  Float distance_from_tube_center;
  Float torus_feature;
  Float score, deriv;
  Float x, y, z;
  Float dx, dy, dz;

  IMP_LOG(VERBOSE, "... evaluating torus restraint.");
  // get current position of particle
  x = model_data_->get_float(x1_);
  y = model_data_->get_float(y1_);
  z = model_data_->get_float(z1_);

  // get the x, y distance from the origin
  xy_distance_from_center = sqrt(x * x + y * y);
  if (xy_distance_from_center == 0.0) {
    // if no direction is favored,
    // ... pick some arbitrary direction to head towards the
    // ... torus interior
    xy_distance_from_center = 0.001;
    x = 0.001;
  }

  // calculate the cross-section_center (large radius away from origin with z=0)
  tube_center_x = x * main_radius_ / xy_distance_from_center;
  tube_center_y = y * main_radius_ / xy_distance_from_center;
  // by definition: tube_center_z = 0;

  // translate to cross-section center
  x -= tube_center_x;
  y -= tube_center_y;

  // calculate the distance from the cross-section_center
  distance_from_tube_center = sqrt(x * x + y * y + z * z);

  // subtract the small radius of the torus so that if the point is within the
  // ... torus the value is negative
  torus_feature = distance_from_tube_center - tube_radius_;

  // if in the torus, partials should be zero
  if (torus_feature <= 0) {
    score = 0.0;
    deriv = 0.0;
  }

  // otherwise, derivatives should reduce feature as torus is neared
  // the derivative vector stays constant independent of distance from torus
  // i.e. a unit vector
  else {
    score = (*score_func_)(torus_feature, deriv);
  }

  // if needed, use the partial derivatives
  if (calc_deriv) {
    dx = deriv * x / distance_from_tube_center;
    dy = deriv * y / distance_from_tube_center;
    dz = deriv * z / distance_from_tube_center;

    model_data_->add_to_deriv(x1_, dx);
    model_data_->add_to_deriv(y1_, dy);
    model_data_->add_to_deriv(z1_, dz);
  }

  return score;
}


/**
  Show the current restraint.

 \param[in] out Stream to send restraint description to.
 */

void RSR_Torus::show(std::ostream& out) const
{
  if (is_active()) {
    out << "torus restraint (active):" << std::endl;
  } else {
    out << "torus restraint (inactive):" << std::endl;
  }

  out << "version: " << version() << "  " << "last_modified_by: " << last_modified_by() << std::endl;
  out << "  x1:" << model_data_->get_float(x1_);
  out << "  y1:" << model_data_->get_float(y1_);
  out << "  z1:" << model_data_->get_float(z1_) << std::endl;

  out << "  dx1:" << model_data_->get_deriv(x1_);
  out << "  dy1:" << model_data_->get_deriv(y1_);
  out << "  dz1:" << model_data_->get_deriv(z1_) << std::endl;

  out << "  main radius:" << main_radius_;
  out << "  tube radius:" << tube_radius_;
}



//######### RSR_Proximity Restraint #########
// Given a list of particles, this restraint calculates the distance
// restraints between all pairs of particles, and then applies the
// one restraint with the greatest score.

/**
  Constructor - set up the values and indexes for this connectivity restraint.

  \param[in] model Pointer to the model.
  \param[in] particles Vector of indexes of particles of first body.
  \param[in] distance Maximum length allowable between any two particles.
  \param[in] score_func_params Parameters for creating a score function.
 */

RSR_Proximity::RSR_Proximity(Model& model,
                             std::vector<int>& particle_indexes,
                             const Float distance,
                             Basic_Score_Func_Params* score_func_params)
{
  set_up(model, particle_indexes);

  // get the indexes associated with the restraints
  int idx = 0;
  score_func_params->set_mean(distance);
  score_func_params->set_score_func_type("harmonic_upper_bound");
  for (int i = 0; i < num_particles_ - 1; i++) {
    for (int j = i + 1; j < num_particles_; j++) {
      // create the restraint
      
      dist_rsrs_[idx] = new RSR_Distance(model,
                                         particles_[i],
                                         particles_[j],
                                         score_func_params);
      idx++;
    }
  }

  IMP_LOG(VERBOSE,
         "Number of restraints: " << num_restraints_ << "  number of particles: " <<
         num_particles_ << std::endl);
}

/**
  Constructor - set up the values and indexes for this connectivity restraint.

  \param[in] model Pointer to the model.
  \param[in] particles Vector of pointers  to particle of the restraint.
  \param[in] attr_name Name to get radii to calculate the mean distance.
  \param[in] distance Maximum length allowable between any two particles.
  \param[in] score_func_params Parameters for creating a score function.
 */

RSR_Proximity::RSR_Proximity(Model& model,
                             std::vector<int>& particle_indexes,
                             const std::string attr_name,
                             const Float distance,
                             Basic_Score_Func_Params* score_func_params)
{
  // Use those radii to calculate the expected distance
  set_up(model, particle_indexes);

  // get the indexes associated with the restraints
  int idx = 0;
  Float actual_mean;
  score_func_params->set_score_func_type("harmonic_upper_bound");
  for (int i = 0; i < num_particles_ - 1; i++) {
    for (int j = i + 1; j < num_particles_; j++) {
      // Use those radii to calculate the expected distance between centers
      actual_mean = distance - model_data_->get_float(particles_[i]->float_index(attr_name))
                    - model_data_->get_float(particles_[j]->float_index(attr_name));

      // create the restraint
      IMP_LOG(VERBOSE, i << " " << j << " add distance: " << actual_mean);
      score_func_params->set_mean(actual_mean);
      dist_rsrs_[idx] = new RSR_Distance(model,
                                         particles_[i],
                                         particles_[j],
                                         score_func_params);
      idx++;
    }
  }
}

/**
  Set up the values and indexes for this connectivity restraint for the constructors.

  \param[in] model Pointer to the model.
  \param[in] particles Vector of indexes of particles in the restraint.
 */

void RSR_Proximity::set_up(Model& model,
                           std::vector<int>& particle_indexes)
{
  IMP_LOG(VERBOSE, "init RSR_Connectivity");

  model_data_ = model.get_model_data();

  /** number of particles in the restraint */
  num_particles_ = particle_indexes.size();

  // set up the particles, their position indexes, and their type indexes
  Particle* p1;
  for (int i = 0; i < num_particles_; i++) {
    p1 = model.get_particle(particle_indexes[i]);
    particles_.push_back(p1);
  }

  // figure out how many restraints there are
  num_restraints_ = num_particles_ * (num_particles_ - 1) / 2;

  // use number of restraints and number of types to set up some of the
  // ... main arrays
  dist_rsrs_.resize(num_restraints_);
  scores_.resize(num_restraints_);
  rsr_idx_.resize(num_restraints_);
}

/**
  Destructor
 */

RSR_Proximity::~RSR_Proximity ()
{
  for (int i = 0; i < num_restraints_; i++) {
    delete dist_rsrs_[i];
  }
}

/**
  Calculate the distance restraints for the given particles. Use the smallest
  restraints that will connect one particle of each type together (i.e. a
  minimum spanning tree with nodes corresponding to particle types and the
  edge weights corresponding to restraint violation score values).

 \param[in] calc_deriv If true, partial first derivatives should be calculated.
 \resturn score associated with this restraint for the given state of the model.
  */

Float RSR_Proximity::evaluate(bool calc_deriv)
{
  int idx;

  IMP_LOG(VERBOSE, "evaluate RSR_Proximity");

  // calculate the scores for all of the restraints
  /*
  There is a problem with using just the largest violation restraint
  in that it causes an inherent unsmoothness in the optimization process.
  I.e. when one pair becomes more distant in another, there is a complete
  switch to an orthogonal direction.

  LogMsg(VERBOSE, "calculate restraint scores");
  for (int i = 0; i < num_restraints_; i++)
   {
   scores_[i] = dist_rsrs_[i]->evaluate(false);
   }
  */

  // dumb bubble sort of indexes by scores.
  // /rsr_idx[]/ holds the sorted indexes so that all other arrays
  // associated with the particles can be left as they are.

  /*
  LogMsg(VERBOSE, "sort scores");
  for (int i = 0; i < num_restraints_; i++)
   {
   rsr_idx_[i] = i;
   }

  for (int i = 0; i < num_restraints_ - 1; i++)
   {
   // move the next highest score index to the left most position of the
   // remaining unsorted list.
   for (int j = num_restraints_ - 1; j > i; j--)
    {
    if (scores_[rsr_idx_[j - 1]] < scores_[rsr_idx_[j]])
     {
     idx = rsr_idx_[j - 1];
     rsr_idx_[j - 1] = rsr_idx_[j];
     rsr_idx_[j] = idx;
     }
    }
   }
  */

  // apply the restraints corresponding the highest score restraints
  int num_restraints_to_use = 1; // mod later if it should be more than one

  // switching back and forth seems to be less smooth
  // just apply all of the restraints every time (also faster)
  num_restraints_to_use = num_restraints_;
  Float score = 0.0;
  for (int i = 0; i < num_restraints_to_use; i++) {
    // idx = rsr_idx_[i];
    idx = i;

    score += dist_rsrs_[idx]->evaluate(calc_deriv);
    IMP_LOG(VERBOSE,
           " " << i << " Applying Restraint: score: "
           << score << std::endl);
  }

  return score;
}


/**
  Show the current restraint.

 \param[in] out Stream to send restraint description to.
 */

void RSR_Proximity::show(std::ostream& out) const
{
  if (is_active()) {
    out << "proximity restraint (active):" << std::endl;
  } else {
    out << "proximity restraint (inactive):" << std::endl;
  }

  out << "version: " << version() << "  " << "last_modified_by: " << last_modified_by() << std::endl;
  out << "  num particles:" << num_particles_;
}




//######### RSR_Pair_Connectivity Restraint #########
// Optimize based on N "best" distances for pairs of particles
// between two structures (e.g. rigid bodies)

/**
  Constructor - set up the values and indexes for this pair connectivity restraint.
  Use the given mean for the expected distance between two particles.

  \param[in] model Pointer to the model.
  \param[in] particle1_indexes Vector of indexes of particles of first body.
  \param[in] particle2_indexes Vector of indexes of particles of second body.
  \param[in] score_func_params Parameters for creating a score function.
  \param[in] num_to_apply Number of minimum restraints to apply.
  \param[in] particle_reuse Allow minimum restraints to use particle more than once.
 */

RSR_Pair_Connectivity::RSR_Pair_Connectivity(Model& model,
    std::vector<int>& particle1_indexes,
    std::vector<int>& particle2_indexes,
    Basic_Score_Func_Params* score_func_params,
    const int num_to_apply,
    const bool particle_reuse)
{
  particle_reuse_ = particle_reuse;
  num_to_apply_ = num_to_apply;
  set_up(model, particle1_indexes, particle2_indexes);


  // get the indexes associated with the restraints
  int idx = 0;
  // use iterator to move through each predefined position in the restraint/score list
  std::list<RSR_Pair_Connectivity::Restraint_Score>::iterator rs_iter;
  rs_iter = rsr_scores_.begin();
  for (int i = 0; i < num_particles1_; i++) {
    for (int j = num_particles1_; j < num_particles1_ + num_particles2_; j++) {
      // create the restraint
      if (rs_iter == rsr_scores_.end()) {
        IMP_failure("Reached end of rsr_scores too early.",
		    std::out_of_range("Reached end of rsr_scores too early"));
      } else {
        IMP_LOG(VERBOSE, "Adding possible restraint: " << i << " " << j);
        rs_iter->rsr_ = new RSR_Distance(model,
                                         particles_[i],
                                         particles_[j],
                                         score_func_params);

        rs_iter->part1_idx_ = i;
        rs_iter->part2_idx_ = j;
      }

      ++rs_iter;
    }
  }

  IMP_LOG(VERBOSE, idx <<
         "  num_restraints_: " << num_restraints_ << "  num_particles1_: " <<
         num_particles1_ << " num_particles2_:" << num_particles2_ << std::endl);
}

/**
  Constructor - set up the values and indexes for this pair connectivity restraint. Use
  the attr_name to specify the attribute you want to use for determining the expected
  distance between two particles.

  \param[in] model Pointer to the model.
  \param[in] particles Vector of pointers  to particle of the restraint.
  \param[in] type The main radius from the origin to the midline of the tube.
  \param[in] attr_name Name to get radii to calculate the mean distance.
  \param[in] score_func_params Parameters for creating a score function.
  \param[in] num_to_apply Number of minimum restraints to apply.
  \param[in] particle_reuse Allow minimum restraints to use particle more than once.
 */

RSR_Pair_Connectivity::RSR_Pair_Connectivity(Model& model,
    std::vector<int>& particle1_indexes,
    std::vector<int>& particle2_indexes,
    const std::string attr_name,
    Basic_Score_Func_Params* score_func_params,
    const int num_to_apply,
    const bool particle_reuse)
{
  particle_reuse_ = particle_reuse;
  num_to_apply_ = num_to_apply;
  set_up(model, particle1_indexes, particle2_indexes);

  // get the indexes associated with the restraints
  int idx = 0;
  Float actual_mean;

  // use iterator to move through each predefined position in the restraint/score list
  std::list<RSR_Pair_Connectivity::Restraint_Score>::iterator rs_iter;
  rs_iter = rsr_scores_.begin();
  // particles from list 1
  for (int i = 0; i < num_particles1_; i++) {
    // particles from list 2
    for (int j = num_particles1_; j < num_particles_; j++) {
      // Use those radii to calculate the expected distance
      actual_mean = model_data_->get_float(particles_[i]->float_index(attr_name))
                    + model_data_->get_float(particles_[j]->float_index(attr_name));

      score_func_params->set_mean(actual_mean);
      
      // create the restraint
      if (rs_iter == rsr_scores_.end()) {
        IMP_failure("Reached end of rsr_scores too early.",
		    std::out_of_range("Reached end of rsr_scores too early"));
      } else {
        IMP_LOG(VERBOSE, "Adding possible restraint: " << i << " " << j);
        rs_iter->rsr_ = new RSR_Distance(model,
                                         particles_[i],
                                         particles_[j],
                                         score_func_params);

        rs_iter->part1_idx_ = i;
        rs_iter->part2_idx_ = j;
      }

      ++rs_iter;
    }
  }

  IMP_LOG(VERBOSE, idx <<
         "  num_restraints_: " << num_restraints_ << "  num_particles1_: " <<
         num_particles1_ << " num_particles2_:" << num_particles2_ << std::endl);
}

/**
  Set up the values and indexes for this pair connectivity restraint for the constructors.

  \param[in] model Pointer to the model.
  \param[in] particles1 Vector of indexes of particles in first body of the restraint.
  \param[in] particles2 Vector of indexes of particles in second body of the restraint.
 */

void RSR_Pair_Connectivity::set_up(Model& model,
                                   std::vector<int>& particle1_indexes,
                                   std::vector<int>& particle2_indexes)
{
  Particle* p1;

  IMP_LOG(VERBOSE, "init RSR_Connectivity");

  model_data_ = model.get_model_data();

  /** number of particles in the restraint */
  num_particles1_ = particle1_indexes.size();
  num_particles2_ = particle2_indexes.size();
  num_particles_ = num_particles1_ + num_particles2_;

  IMP_LOG(VERBOSE, "set up particle types");
  // set up the particles, their position indexes, and their type indexes
  for (int i = 0; i < num_particles1_; i++) {
    p1 = model.get_particle(particle1_indexes[i]);
    particles_.push_back(p1);
  }

  for (int i = 0; i < num_particles2_; i++) {
    p1 = model.get_particle(particle2_indexes[i]);
    particles_.push_back(p1);
  }

  // figure out how many restraints there are
  num_restraints_ = num_particles1_ * num_particles2_;

  // use number of restraints and number of types to set up some of the
  // ... main arrays
  used_.resize(num_particles_);
  rsr_scores_.resize(num_restraints_);
}

/**
  Destructor
 */

RSR_Pair_Connectivity::~RSR_Pair_Connectivity ()
{
  std::list<RSR_Pair_Connectivity::Restraint_Score>::iterator rs_iter;

  for (rs_iter = rsr_scores_.begin(); rs_iter != rsr_scores_.end(); ++rs_iter) {
    delete(rs_iter->rsr_);
  }
}

/**
  Calculate the distance restraints for the given particles. Use the smallest
  restraints that will connect one particle of each type together (i.e. a
  minimum spanning tree with nodes corresponding to particle types and the
  edge weights corresponding to restraint violation scores).

  There is a potential when using this restraint for two large rigid bodies
  to maintain its own exclusion volume restraints. These could be calculated
  only for the particles in the neighborhoods of the particles involved in the
  activated restraints rather than between all particles in the two bodies.
  Since once restraints are activated, they tend to be activated over and over,
  the exclusion volume restraint sets should only be reset whenever on rare
  occasion the neighborhoods actually change.
  
 \param[in] calc_deriv If true, partial first derivatives should be calculated.
 \return score associated with this restraint for the given state of the model.
  */

Float RSR_Pair_Connectivity::evaluate(bool calc_deriv)
{
  std::list<RSR_Pair_Connectivity::Restraint_Score>::iterator rs_iter;

  IMP_LOG(VERBOSE, "evaluate RSR_Pair_Connectivity");

  // only use a particle at most once in set of restraints
  for (int i = 0; i < num_particles_; i++)
    used_[i] = false;

  // calculate the scores for all of the restraints
  IMP_LOG(VERBOSE, "calculate restraint scores");
  int j = 0;
  for (rs_iter = rsr_scores_.begin(); rs_iter != rsr_scores_.end(); ++rs_iter) {
    rs_iter->evaluate();
    IMP_LOG(VERBOSE, j++ << " score: " << rs_iter->score_);
  }

  // sort by the scores
  rsr_scores_.sort();

  IMP_LOG(VERBOSE, "sorted");
  j = 0;
  for (rs_iter = rsr_scores_.begin(); rs_iter != rsr_scores_.end(); ++rs_iter) {
    IMP_LOG(VERBOSE, j++ << " score: " << rs_iter->score_);
  }

  Float score = 0.0;
  rs_iter = rsr_scores_.begin();
  int num_applied = 0;
  for (int i = 0; (num_applied < num_to_apply_) && (rs_iter != rsr_scores_.end()); i++) {
    if (particle_reuse_ || (!used_[rs_iter->part1_idx_] && !used_[rs_iter->part2_idx_])) {
      used_[rs_iter->part1_idx_] = true;
      used_[rs_iter->part2_idx_] = true;
      score += rs_iter->rsr_->evaluate(calc_deriv);
      num_applied++;
    }

    ++rs_iter;
  }

  return score;
}


/**
  Show the current restraint.

 \param[in] out Stream to send restraint description to.
 */

void RSR_Pair_Connectivity::show(std::ostream& out) const
{
  if (is_active()) {
    out << "pair connectivity restraint (active):" << std::endl;
  } else {
    out << "pair connectivity restraint (inactive):" << std::endl;
  }

  out << "version: " << version() << "  " << "last_modified_by: " << last_modified_by() << std::endl;
  out << "  num particles1:" << num_particles1_ << "  num particles2:" << num_particles2_;
  out << "  num restraints:" << num_restraints_;
  out << "  num restraints to apply:" << num_to_apply_;
}



//######### RSR_Connectivity Restraint #########
// Optimize based on "best" distances for pairs of particles w
// representing one of each possible pair of particle types

/**
  Constructor - set up the values and indexes for this connectivity restraint. Use
  the given mean for the expected distance between two particles.

  \param[in] model Pointer to the model.
  \param[in] particles Vector of pointers  to particle of the restraint.
  \param[in] type The attribute used to determine if particles are equivalent.
  \param[in] score_func_params Parameters for creating a score function.
 */

RSR_Connectivity::RSR_Connectivity(Model& model,
                                   std::vector<int>& particle_indexes,
                                   const std::string type,
                                   Basic_Score_Func_Params* score_func_params)
{
  std::list<RSR_Connectivity::Restraint_Score>::iterator rs_iter;

  set_up(model, particle_indexes, type);

  // set up the restraints
  rs_iter = rsr_scores_.begin();
  for (int i = 0; i < num_particles_ - 1; i++) {
    for (int j = i + 1; j < num_particles_; j++) {
      if (particle_type_[j] != particle_type_[i]) {
        if (rs_iter == rsr_scores_.end()) {
          IMP_failure("Over ran the caculated number of restraints in RSR_Connectivity", std::out_of_range("Over ran the calculated number of restraints"));
        } else {
          rs_iter->part1_type_ = particle_type_[i];
          rs_iter->part2_type_ = particle_type_[j];

          // create the restraint
          rs_iter->rsr_ = new RSR_Distance(model,
                                           particles_[i],
                                           particles_[j],
                                           score_func_params);
          ++rs_iter;
        }
      }
    }
  }

  IMP_LOG(VERBOSE,
         "Number of types: " << num_types_ << "  max_type_: " << max_type_ <<
         "  num_restraints_: " << num_restraints_ << "  num_particles_: " <<
         num_particles_ << std::endl);
}

/**
  Constructor - set up the values and indexes for this connectivity restraint. Use
  the attr_name to specify the attribute you want to use for determining the expected
  distance between two particles.

  \param[in] model Pointer to the model.
  \param[in] particles Vector of pointers  to particle of the restraint.
  \param[in] type The attribute used to determine if particles are equivalent.
  \param[in] attr_name Name to get radii to calculate the mean distance.
  \param[in] score_func_params Parameters for creating a score function.
 */

RSR_Connectivity::RSR_Connectivity(Model& model,
                                   std::vector<int>& particle_indexes,
                                   const std::string type,
                                   const std::string attr_name,
                                   Basic_Score_Func_Params* score_func_params)
{
  std::list<RSR_Connectivity::Restraint_Score>::iterator rs_iter;

  IMP_LOG(VERBOSE, "RSR_Connectivity constructor");
  set_up(model, particle_indexes, type);

  // set up the restraints
  Float actual_mean;
  rs_iter = rsr_scores_.begin();
  for (int i = 0; i < num_particles_ - 1; i++) {
    for (int j = i + 1; j < num_particles_; j++) {
      if (particle_type_[j] != particle_type_[i]) {
        if (rs_iter == rsr_scores_.end()) {
          IMP_failure("Over ran the caculated number of restraints in RSR_Connectivity", std::out_of_range("Over ran the calculated number of restraints"));
        } else {
          rs_iter->part1_type_ = particle_type_[i];
          rs_iter->part2_type_ = particle_type_[j];

          // Use those radii to calculate the expected distance
          actual_mean = model_data_->get_float(particles_[i]->float_index(attr_name))
                        + model_data_->get_float(particles_[j]->float_index(attr_name));

          score_func_params->set_mean(actual_mean);
          
          // create the restraint
          rs_iter->rsr_ = new RSR_Distance(model,
                                           particles_[i],
                                           particles_[j],
                                           score_func_params);
          ++rs_iter;
        }
      }
    }
  }

  IMP_LOG(VERBOSE,
         "Number of types: " << num_types_ << "  max_type_: " << max_type_ <<
         "  num_restraints_: " << num_restraints_ << "  num_particles_: " <<
         num_particles_ << std::endl);
}

/**
  Set up the values and indexes for this connectivity restraint for the constructors.

  \param[in] model Pointer to the model.
  \param[in] particles Vector of pointers  to particle of the restraint.
  \param[in] type The attribute used to determine if particles are equivalent.
 */

void RSR_Connectivity::set_up(Model& model,
                              std::vector<int>& particle_indexes,
                              const std::string type)
{
  Particle* p1;

  IMP_LOG(VERBOSE, "init RSR_Connectivity");

  model_data_ = model.get_model_data();

  IMP_LOG(VERBOSE, "got model data " << model_data_);

  /** number of particles in the restraint */
  num_particles_ = particle_indexes.size();
  particle_type_.resize(num_particles_);

  IMP_LOG(VERBOSE, "set up particle types");
  // set up the particles, their position indexes, and their type indexes
  for (int i = 0; i < num_particles_; i++) {
    p1 = model.get_particle(particle_indexes[i]);
    particles_.push_back(p1);
    type_.push_back(p1->int_index(type));
  }

  IMP_LOG(VERBOSE, "Size of particles: " << particles_.size() << "  size of types:" << type_.size() << "  num_particles_: " << num_particles_ << "  particle_type_: " << particle_type_.size());
  IMP_LOG(VERBOSE, "Figure out number of types");
  // figure out how many types there are
  int next_type;
  max_type_ = 0;
  num_types_ = 0;
  for (int i = 0; i < num_particles_; i++) {
    next_type = model_data_->get_int(type_[i]);
    if (max_type_ < next_type) {
      max_type_ = next_type;
    }

    num_types_++;
    particle_type_[i] = next_type;
    // if this type already was used, decrement numbe of types
    for (int j = 0; j < i; j++) {
      if (particle_type_[j] == next_type) {
        num_types_--;
        break;
      }
    }
  }

  // types can range from 0 to max_type_ - 1
  max_type_++;
  IMP_LOG(VERBOSE, "Max types: " << max_type_);

  IMP_LOG(VERBOSE, "Figure out number of restraints");
  // figure out how many restraints there are
  // Could use num_restraints = sum((S-si)si)) where is total number of particles
  // ... and si is number of particles of type i (summed over all types)
  num_restraints_ = num_particles_ * (num_particles_ - 1) / 2;
  for (int i = 0; i < num_particles_ - 1; i++) {
    for (int j = i + 1; j < num_particles_; j++) {
      if (particle_type_[j] == particle_type_[i]) {
        num_restraints_--;
      }
    }
  }

  IMP_LOG(VERBOSE, "Num restraints: " << num_restraints_);
  // use number of restraints and number of types to set up some of the
  // ... main arrays
  tree_id_.resize(max_type_);
  rsr_scores_.resize(num_restraints_);
}

/**
  Destructor
 */

RSR_Connectivity::~RSR_Connectivity ()
{
  std::list<RSR_Connectivity::Restraint_Score>::iterator rs_iter;

  for (rs_iter = rsr_scores_.begin(); rs_iter != rsr_scores_.end(); ++rs_iter) {
    delete(rs_iter->rsr_);
  }
}

/**
  Calculate the distance restraints for the given particles. Use the smallest
  restraints that will connect one particle of each type together (i.e. a
  minimum spanning tree with nodes corresponding to particle types and the
  edge weights corresponding to restraint violation scores).

 \param[in] calc_deriv If true, partial first derivatives should be calculated.
 \return score associated with this restraint for the given state of the model.
  */

Float RSR_Connectivity::evaluate(bool calc_deriv)
{
  IMP_LOG(VERBOSE, "evaluate RSR_Connectivity");

  std::list<RSR_Connectivity::Restraint_Score>::iterator rs_iter;

  // calculate the scores for all of the restraints
  IMP_LOG(VERBOSE, "calculate restraint scores");
  for (rs_iter = rsr_scores_.begin(); rs_iter != rsr_scores_.end(); ++rs_iter) {
    rs_iter->evaluate();
  }

  // sort by the scores
  rsr_scores_.sort();

  // calculate the minmimum spanning tree
  IMP_LOG(VERBOSE, "calc spanning tree");
  int max_tree_id = 0;

  // initially there are no trees
  for (int i = 0; i < max_type_; i++) {
    tree_id_[i] = 0;
  }

  // start with the lowest score_ edges and work our way up
  Float score = 0.0;
  int num_edges = 0;
  int type1, type2;
  for (rs_iter = rsr_scores_.begin(); (rs_iter != rsr_scores_.end()) && (num_edges < num_types_ - 1); ++rs_iter) {
    type1 = rs_iter->part1_type_;
    type2 = rs_iter->part2_type_;

    IMP_LOG(VERBOSE, "Test for " << type1 << " " << type2);
    // if neither particle is in a tree, create a new tree
    if (!tree_id_[type1] && !tree_id_[type2]) {
      max_tree_id++;
      tree_id_[type1] = max_tree_id;
      tree_id_[type2] = max_tree_id;

      num_edges++;
      IMP_LOG(VERBOSE, "Evaluate for " << type1 << " " << type2);
      score += rs_iter->rsr_->evaluate(calc_deriv);
    }

    // if only one particle is already in a tree, add the other particle
    // to that tree
    else if (!tree_id_[type1] || !tree_id_[type2]) {
      if (tree_id_[type1]) {
        tree_id_[type2] = tree_id_[type1];
      } else {
        tree_id_[type1] = tree_id_[type2];
      }

      num_edges++;
      IMP_LOG(VERBOSE, "Evaluate for " << type1 << " " << type2);
      score += rs_iter->rsr_->evaluate(calc_deriv);
    }

    // both particles are already in trees
    // if they are already in the same tree, do nothing
    // otherwise, merge the two trees
    else {
      if (tree_id_[type1] != tree_id_[type2]) {
        int old_tree_num = tree_id_[type1];
        int new_tree_num = tree_id_[type2];

        for (int j = 0; j < max_type_; j++) {
          if (tree_id_[j] == old_tree_num) {
            tree_id_[j] = new_tree_num;
          }
        }

        num_edges++;
        IMP_LOG(VERBOSE, "Evaluate for " << type1 << " " << type2);
        score += rs_iter->rsr_->evaluate(calc_deriv);
      }
    }
  }

  return score;
}


/**
  Show the current restraint.

 \param[in] out Stream to send restraint description to.
 */

void RSR_Connectivity::show(std::ostream& out) const
{
  if (is_active()) {
    out << "connectivity restraint (active):" << std::endl;
  } else {
    out << "connectivity restraint (inactive):" << std::endl;
  }

  out << "version: " << version() << "  " << "last_modified_by: " << last_modified_by() << std::endl;
  out << "  num particles:" << num_particles_;
}




//######### RSR_Exclusion_Volume Restraint #########
// Apply restraints that prevent particles from getting too close together

/**
  Constructor - set up the values and indexes for this exclusion volume restraint. Use
  the attr_name to access the radii for the minimum distance between two particles.
  Assume that there is no overlap between the two particle lists. Create restraints for
  all possible pairs between the two lists.

  \param[in] model Pointer to the model.
  \param[in] particles1 Vector of indexes of particles of the first body.
  \param[in] particles2 Vector of indexes of particles of the second body.
  \param[in] attr_name The attribute used to determine the vw radius of each particle.
  \param[in] sd Standard deviation associated with the score function for the restraint.
 */

RSR_Exclusion_Volume::RSR_Exclusion_Volume(Model& model,
    // couldn't get Swig to work with std::vector<Particle*>&
    std::vector<int>& particle1_indexes,
    std::vector<int>& particle2_indexes,
    const std::string attr_name,
    Basic_Score_Func_Params* score_func_params
                                          )
{
  Particle* p1;

  model_data_ = model.get_model_data();

  num_particles1_ = particle1_indexes.size();
  num_particles2_ = particle2_indexes.size();
  num_particles_ = num_particles1_ + num_particles2_;
  for (int i = 0; i < num_particles1_; i++) {
    p1 = model.get_particle(particle1_indexes[i]);
    particles_.push_back(p1);
  }

  for (int i = 0; i < num_particles2_; i++) {
    p1 = model.get_particle(particle2_indexes[i]);
    particles_.push_back(p1);
  }

  num_restraints_ = num_particles1_ * num_particles2_;

  // get the indexes associated with the restraints
  Float actual_mean;
  IMP_LOG(VERBOSE, "Add inter-body exclusion volume restraints " << num_restraints_);

  // particle 1 indexes
  for (int i = 0; i < num_particles1_; i++) {
    // particle 2 indexes
    for (int j = num_particles1_; j < num_particles_; j++) {
      // Use those radii to calculate the expected distance
      actual_mean = model_data_->get_float(particles_[i]->float_index(attr_name))
                    + model_data_->get_float(particles_[j]->float_index(attr_name));

      score_func_params->set_mean(actual_mean);
      
      // create the restraint
      dist_rsrs_.push_back(new RSR_Distance(model,
                                            particles_[i],
                                            particles_[j],
                                            score_func_params));
    }
  }
}

/**
  Constructor - set up the values and indexes for this exclusion volume restraint. Use
  the attr_name to access the radii for the minimum distance between two particles.
  Create restraints for all possible pairs of particles in the list.

  \param[in] model Pointer to the model.
  \param[in] particles Vector of indexes of particles.
  \param[in] attr_name The attribute used to determine the vw radius of each particle.
  \param[in] sd Standard deviation associated with the score function for the restraint.
 */

RSR_Exclusion_Volume::RSR_Exclusion_Volume(Model& model,
    // couldn't get Swig to work with std::vector<Particle*>&
    std::vector<int>& particle_indexes,
    const std::string attr_name,
    Basic_Score_Func_Params* score_func_params
                                          )
{
  Particle* p1;

  model_data_ = model.get_model_data();

  num_particles_ = particle_indexes.size();
  for (int i = 0; i < num_particles_; i++) {
    p1 = model.get_particle(particle_indexes[i]);
    particles_.push_back(p1);
  }

  num_restraints_ = num_particles_ * (num_particles_ - 1) / 2;

  // get the indexes associated with the restraints
  int idx = 0;
  Float actual_mean;
  IMP_LOG(VERBOSE, "Add intra-body exclusion volume restraints " << num_restraints_);

  // particle 1 indexes
  for (int i = 0; i < num_particles_ - 1; i++) {
    // particle 2 indexes (avoid duplicates and particle with itself)
    for (int j = i+1; j < num_particles_; j++) {
      // Use those radii to calculate the expected distance
      actual_mean = model_data_->get_float(particles_[i]->float_index(attr_name))
                    + model_data_->get_float(particles_[j]->float_index(attr_name));

      score_func_params->set_mean(actual_mean);
      
      // create the restraint
      dist_rsrs_.push_back(new RSR_Distance(model,
                                            particles_[i],
                                            particles_[j],
                                            score_func_params));
      idx++;
    }
  }
}


/**
  Destructor
 */

RSR_Exclusion_Volume::~RSR_Exclusion_Volume ()
{
  std::vector<RSR_Distance*>::iterator rsr_iter;

  for (rsr_iter = dist_rsrs_.begin(); rsr_iter != dist_rsrs_.end(); ++rsr_iter) {
    delete(*rsr_iter);
  }
}

/**
  Calculate the distance restraints for the given particles. Use the smallest
  restraints that will connect one particle of each type together (i.e. a
  minimum spanning tree with nodes corresponding to particle types and the
  edge weights corresponding to restraint violation scores).

 \param[in] calc_deriv If true, partial first derivatives should be calculated.
 \return score associated with this restraint for the given state of the model.
  */

Float RSR_Exclusion_Volume::evaluate(bool calc_deriv)
{
  std::vector<RSR_Distance*>::iterator rsr_iter;

  Float score = 0.0;

  // until this is smarter, just calculate them all
  for (rsr_iter = dist_rsrs_.begin(); rsr_iter != dist_rsrs_.end(); ++rsr_iter) {
    score += (*rsr_iter)->evaluate(calc_deriv);
  }

  return score;
}


/**
  Show the current restraint.

 \param[in] out Stream to send restraint description to.
 */

void RSR_Exclusion_Volume::show(std::ostream& out) const
{
  if (is_active()) {
    out << "exclusion volume restraint (active):" << std::endl;
  } else {
    out << "exclusion volume  restraint (inactive):" << std::endl;
  }

  out << "version: " << version() << "  " << "last_modified_by: " << last_modified_by() << std::endl;
  out << "  num particles:" << num_particles_;
}


//######### RSR_EM_Coarse Restraint #########

/**
  Constructor - set up the values and indexes for this EM coarse restraint. 
  
*/

RSR_EM_Coarse::RSR_EM_Coarse(Model& model,
                             std::vector<int>& particle_indexes,
                             EM_Density* emdens,
                             int nx,
                             int ny,
                             int nz,
                             float pixelsize,
                             float resolution,
                             std::string radius_str,
                             std::string weight_str,
                             EM_Gridcoord* gridcd,
                             float scalefac
                            )
{
  Particle* p1;

  /** number of particles in the restraint */
  num_particles_ = particle_indexes.size();

  IMP_LOG(VERBOSE, "set up particles");
  // set up the particles, their position indexes, and their type indexes
  for (int i = 0; i < num_particles_; i++) {
    p1 = model.get_particle(particle_indexes[i]);
    particles_.push_back(p1);
  }

  x_ = new Float_Index[num_particles_];
  y_ = new Float_Index[num_particles_];
  z_ = new Float_Index[num_particles_];

  cdx_ = new float[num_particles_];
  cdy_ = new float[num_particles_];
  cdz_ = new float[num_particles_];

  dvx_ = new float[num_particles_];
  dvy_ = new float[num_particles_];
  dvz_ = new float[num_particles_];

  weight_ = new float[num_particles_];
  radius_ = new float[num_particles_];

  model_data_ = model.get_model_data();

  for (int i = 0; i < num_particles_; i++) {
    p1 = particles_[i];
    x_[i] = p1->float_index(std::string("X"));
    y_[i] = p1->float_index(std::string("Y"));
    z_[i] = p1->float_index(std::string("Z"));
    weight_[i] = model_data_->get_float(p1->float_index(weight_str));
    radius_[i] = model_data_->get_float(p1->float_index(radius_str));
    IMP_LOG (VERBOSE, "radius " << radius_str << radius_[i]);
  }

  emdens_ = emdens;
  nx_ = nx;
  ny_ = ny;
  nz_ = nz;
  pixelsize_ = pixelsize;
  resolution_ = resolution;
  gridcd_ = gridcd;
  scalefac_ = scalefac;
}


/**
  Destructor
 */

RSR_EM_Coarse::~RSR_EM_Coarse ()
{
  delete x_;
  delete y_;
  delete z_;

  delete cdx_;
  delete cdy_;
  delete cdz_;

  delete dvx_;
  delete dvy_;
  delete dvz_;
}

/**
  Calculate the em coarse restraint score.

 \param[in] calc_deriv If true, partial first derivatives should be calculated.
 \return score associated with this restraint for the given state of the model.
  */

Float RSR_EM_Coarse::evaluate(bool calc_deriv)
{
  int lderiv = (int) calc_deriv;
  int ierr;

  for (int i = 0; i < num_particles_; i++) {
    cdx_[i] = model_data_->get_float(x_[i]);
    cdy_[i] = model_data_->get_float(y_[i]);
    cdz_[i] = model_data_->get_float(z_[i]);

    dvx_[i] = 0.0;
    dvy_[i] = 0.0;
    dvz_[i] = 0.0;

    IMP_LOG(VERBOSE, i << " x: " << cdx_[i] << " y: " << cdy_[i] << " z: " << cdz_[i] << " weight_: " << weight_[i] << " radius: " << radius_[i]);
  }

  IMP_LOG(VERBOSE, "before emscore: " << emdens_ << " nx_" << nx_ << " ny_" << ny_ << " nz_" << nz_ << " pixelsize_" << pixelsize_ << " resolution_" << resolution_ << " " <<
         cdx_ << " " << cdy_ << " " << cdz_ << " " << dvx_ << " " << dvy_ << " " << dvz_ << " num_particles_" << num_particles_ << " " <<
         radius_ << " " << weight_ << " gridcd_" << gridcd_ << " scalefac_" << scalefac_);

  Float score;
  score = emscore(emdens_, nx_, ny_, nz_, pixelsize_, resolution_,
                  cdx_, cdy_, cdz_, dvx_, dvy_, dvz_, num_particles_,
                  radius_, weight_, gridcd_, scalefac_, lderiv, &ierr);

  IMP_LOG(VERBOSE, "after emscore: " << score << " calc_deriv" << calc_deriv);
  score = 0.0;
  if (calc_deriv) {
    for (int i = 0; i < num_particles_; i++) {
      model_data_->add_to_deriv(x_[i], dvx_[i]);
      model_data_->add_to_deriv(y_[i], dvy_[i]);
      model_data_->add_to_deriv(z_[i], dvz_[i]);
    }
  }

  return score;
}


/**
  Show the current restraint.

 \param[in] out Stream to send restraint description to.
 */

void RSR_EM_Coarse::show(std::ostream& out) const
{
  if (is_active()) {
    out << "em coarse restraint (active):" << std::endl;
  } else {
    out << "em coarse  restraint (inactive):" << std::endl;
  }

  out << "version: " << version() << "  " << "last_modified_by: " << last_modified_by() << std::endl;
  out << "  num particles:" << num_particles_;
}

}  // namespace imp
