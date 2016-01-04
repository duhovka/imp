/**
 *  \file Atom.cpp   \brief Simple atoms decorator.
 *
 *  Copyright 2007-2016 IMP Inventors. All rights reserved.
 *
 */

#include <IMP/atom/Representation.h>
#include <IMP/atom/Atom.h>
#include <IMP/atom/Mass.h>
#include <IMP/core/Gaussian.h>
#include <IMP/log.h>

#include <boost/unordered_map.hpp>
#include <sstream>
#include <vector>

IMPATOM_BEGIN_NAMESPACE

double get_resolution(Model* m, ParticleIndex pi) {
  double min = std::numeric_limits<double>::max();
  IMP_FOREACH(Hierarchy l, get_leaves(Hierarchy(m, pi))) {
    double cur = core::XYZR(l).get_radius();
    IMP_USAGE_CHECK(cur > 0, "Particle " << l << " has an invalid radius");
    min = std::min(min, cur);
  }
  return 1.0 / min;
}
const double ALL_RESOLUTIONS = -std::numeric_limits<double>::max();

IntsKey Representation::get_types_key() {
  static IntsKey k("representation_types");
  return k;
}

FloatKey Representation::get_resolution_key(unsigned int index) {
  static boost::unordered_map<unsigned int, FloatKey> keys;
  if (keys.find(index) == keys.end()) {
    std::ostringstream oss;
    oss << "representation_resolution_" << index;
    keys[index] = FloatKey(oss.str());
  }
  return keys.find(index)->second;
}

FloatKey Representation::get_base_resolution_key() {
  static FloatKey key("base_resolution");
  return key;
}

ParticleIndexesKey Representation::get_representations_key() {
  static ParticleIndexesKey key("representations");
  return key;
}

void Representation::do_setup_particle(Model* m,
                                       ParticleIndex pi,
                                       double resolution) {
  if (resolution < 0) {
    resolution = get_resolution(m, pi);
  }
  m->add_attribute(get_base_resolution_key(), pi, resolution);
}

namespace {
double get_resolution_distance(double a, double b) {
  if (a < b) std::swap(a, b);
  return a / b - 1;
}
}

Hierarchy Representation::get_representation(double resolution,
                                             RepresentationType type) {
  IMP_USAGE_CHECK(type == BALLS || type == DENSITIES, "No matching types found");
  double closest_resolution = get_model()->get_attribute(
     get_base_resolution_key(), get_particle_index());

  int closest_index = -1;

  // manually check for the resolution 0 case
  if (type==BALLS && resolution==0.0 && closest_resolution==0.0) {
  }
  // if multiple representations, check for matching type and try to improve on base resolution
  else if (get_model()->get_has_attribute(get_types_key(), get_particle_index())) {
    Ints types =
        get_model()->get_attribute(get_types_key(), get_particle_index());
    IMP_LOG_VERBOSE("Found " << types.size() << " resolution levels"
                             << std::endl);
    for (unsigned int i = 0; i < types.size(); ++i) {

      if (types[i]==type) {
        double cur_resolution = get_model()->get_attribute(get_resolution_key(i),
                                                           get_particle_index());

        if (closest_index==-1 ||
            get_resolution_distance(resolution, cur_resolution) <
            get_resolution_distance(resolution, closest_resolution)) {
          closest_index = i;
          closest_resolution = cur_resolution;
        }
      }
    }
  }

  if (closest_index==-1){
    if (type==BALLS) { //requested balls, couldn't find better than base res
      IMP_LOG_VERBOSE("Returning highest resolution children" << std::endl);
      return *this;
    }
    else {             // requested other than balls but couldn't find ANY match
      IMP_LOG_VERBOSE("The requested representation type was not found" << std::endl);
      return Hierarchy();
    }
  }
  else {
    IMP_LOG_VERBOSE("Returning children with resolution " << closest_resolution
                    << std::endl);
    return Hierarchy(get_model(), get_model()->get_attribute(
                                      get_representations_key(),
                                      get_particle_index())[closest_index]);
  }
}

Hierarchies Representation::get_representations(RepresentationType type) {
  Hierarchies ret;
  if (get_model()->get_has_attribute(get_types_key(), get_particle_index())) {
    Ints types =
        get_model()->get_attribute(get_types_key(), get_particle_index());
    for (unsigned int i = 0; i < types.size(); ++i) {
      if (types[i] == type) {
        ret.push_back(Hierarchy(
            get_model(), get_model()->get_attribute(get_representations_key(),
                                                    get_particle_index())[i]));
      }
    }
  }
  if (type == BALLS) ret += Hierarchies(1, *this);
  return ret;
}

void Representation::add_representation(ParticleIndexAdaptor rep,
                                        RepresentationType type,
                                        double resolution) {
  if (resolution < 0) {
    resolution = get_resolution(get_model(), rep);
  }
  int ct = 0;
  Hierarchies lvs = core::get_leaves(Hierarchy(get_model(),rep));
  for (unsigned i=0;i<lvs.size();i++) {
    ct += int(!core::Gaussian::get_is_setup(lvs[i]));
  }
  IMP_USAGE_CHECK( type!=DENSITIES || ct==0,
                   "DENSITY representations must be Gaussian");
  // fake the parent
  if (get_model()->get_has_attribute(Hierarchy::get_traits().get_parent_key(),
                                     get_particle_index())) {
    get_model()->add_attribute(Hierarchy::get_traits().get_parent_key(), rep,
                               get_parent().get_particle_index());
  }
  if (get_model()->get_has_attribute(get_types_key(), get_particle_index())) {
    // if this particle already has this representation type setup, just add this resolution
    int index = get_model()
                    ->get_attribute(get_types_key(), get_particle_index())
                    .size();
    get_model()
        ->access_attribute(get_types_key(), get_particle_index())
        .push_back(type);
    get_model()
        ->access_attribute(get_representations_key(), get_particle_index())
        .push_back(rep);
    get_model()->add_attribute(get_resolution_key(index), get_particle_index(),
                               resolution);
  } else {
    // otherwise initiate a new list of resolutions for this type
    int index = 0;
    get_model()->add_attribute(get_types_key(), get_particle_index(),
                               Ints(1, type));
    get_model()->add_attribute(get_representations_key(), get_particle_index(),
                               ParticleIndexes(1, rep));
    get_model()->add_attribute(get_resolution_key(index), get_particle_index(),
                               resolution);
  }
}

Floats Representation::get_resolutions(RepresentationType type) const {
  Floats ret;
  if (type == BALLS) {
    ret.push_back(get_model()->get_attribute(get_base_resolution_key(),
                                             get_particle_index()));
  }
  if (get_model()->get_has_attribute(get_types_key(), get_particle_index())) {
    Ints types =
        get_model()->get_attribute(get_types_key(), get_particle_index());
    for (unsigned int i = 0; i < types.size(); ++i) {
      if (types[i] == type) {
        ret.push_back(get_model()->get_attribute(get_resolution_key(i),
                                                 get_particle_index()));
      }
    }
  }
  return ret;
}

void Representation::show(std::ostream& out) const { out << get_resolutions(); }

IMPATOM_END_NAMESPACE
