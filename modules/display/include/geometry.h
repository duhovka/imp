/**
 *  \file geometry.h
 *  \brief XXXXXXXXXXXXXX
 *
 *  Copyright 2007-9 Sali Lab. All rights reserved.
 */

#ifndef IMPDISPLAY_GEOMETRY_H
#define IMPDISPLAY_GEOMETRY_H

#include "config.h"
#include "Color.h"
#include "macros.h"
#include "internal/version_info.h"
#include <IMP/PairContainer.h>
#include <IMP/SingletonContainer.h>
#include <IMP/RefCounted.h>
#include <IMP/algebra/Vector3D.h>

IMPDISPLAY_BEGIN_NAMESPACE

//! Compute a geometric description from a particle
/** Extract geometry from a particle. This is an abstract
    base class.

    A given particle is turned into a geometric object with a
    given dimension and size. An object of dimension 0 is a sphere
    (or a point if the radius is 0), and object of dimension 1
    is a cylinder or segment, and an object with higher dimension
    is a polygon (and must have thickness 0). A dimension of -1
    means do nothing.
 */
class IMPDISPLAYEXPORT Geometry: public Object
{
  Color default_color_;
  std::string name_;
public:
  Geometry();

  virtual unsigned int get_dimension() const=0;

  virtual algebra::Vector3D get_vertex(unsigned int i) const=0;

  virtual unsigned int get_number_of_vertices() const=0;

  virtual Float get_size() const {
    return 0;
  }

  std::string get_name() const {
    return name_;
  }

  virtual Color get_color() const {
    return default_color_;
  }

  void set_color(Color c) {
    default_color_= c;
  }

  void set_name(std::string c) {
    name_= c;
  }

  virtual void show(std::ostream &out= std::cout) const=0;

  virtual VersionInfo get_version_info() const =0;

  IMP_REF_COUNTED_DESTRUCTOR(Geometry);
};

IMP_OUTPUT_OPERATOR(Geometry);

typedef VectorOfRefCounted<Geometry*> Geometries;

//! Produce some geometry from a particle
class IMPDISPLAYEXPORT CompoundGeometry: public Object {
  std::string name_;
  Color color_;
 public:
  CompoundGeometry();

  virtual void show(std::ostream &out= std::cout) const{};

  virtual VersionInfo get_version_info() const {return internal::version_info;}

  virtual Geometries get_geometry() const =0;

  const std::string &get_name() const {
    return name_;
  }

  void set_color(const Color& c) {
    color_=c;
  }

  Color get_color() const {
    return color_;
  }

  void set_name(std::string name) {
    name_=name;
  }

  IMP_REF_COUNTED_DESTRUCTOR(CompoundGeometry);
};
IMP_OUTPUT_OPERATOR(CompoundGeometry);

typedef VectorOfRefCounted<CompoundGeometry*> CompoundGeometries;


struct NullGeometry: public Geometry {
  unsigned int get_dimension() const {return 0;}

  algebra::Vector3D get_vertex(unsigned int i) const {
    throw InvalidStateException("No vertices");
  }

  unsigned int get_number_of_vertices() const {
    return 0;
  }

  void show(std::ostream &out) const {
    out << "Null geometry";
  }
  VersionInfo get_version_info() const {
    return internal::version_info;
  }
};

IMPDISPLAY_END_NAMESPACE

#endif  /* IMPDISPLAY_GEOMETRY_H */
