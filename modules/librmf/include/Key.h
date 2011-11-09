/**
 *  \file RMF/Key.h
 *  \brief Handle read/write of Model data from/to files.
 *
 *  Copyright 2007-2011 IMP Inventors. All rights reserved.
 *
 */

#ifndef IMPLIBRMF_KEY_H
#define IMPLIBRMF_KEY_H

#include "RMF_config.h"
#include "Category.h"
#include "hdf5_types.h"
#include "HDF5DataSetD.h"
#if !defined(SWIG) && !defined(IMP_DOXYGEN)
namespace RMF {
  namespace internal {
class SharedData;

  } // namespace internal
} // namespace RMF
#endif

namespace RMF {

/** A key referencing a particular piece of data */
  template <class TypeTraitsT, int Arity>
class Key {
  friend class RootHandle;
  friend class NodeHandle;
  friend class internal::SharedData;
  int i_;
  Category ci_;
  bool pf_;
  int compare(const Key &o) const {
    if (i_ < o.i_) return -1;
    else if (i_ > o.i_) return 1;
    else if (ci_ < o.ci_) return -1;
    else if (ci_ > o.ci_) return 1;
    else return 0;
  }
  Key(Category category_id,
      int i, bool pf): i_(i), ci_(category_id),
                       pf_(pf) {}
  int get_index() const {
    return i_;
  }
  bool get_is_per_frame() const {
    return pf_;
  }
  Category get_category() const {
    return ci_;
  }
    int get_arity() const {
      return Arity;
    }
public:
  typedef TypeTraitsT TypeTraits;
  Key(): i_(-1), ci_(), pf_(false) {}
  void show(std::ostream &out) const {
    out << ci_.get_name() << (pf_?'E':'I') << i_;
  }
  IMP_RMF_COMPARISONS(Key);
  IMP_RMF_HASHABLE(Key, return i_*ci_.get_index());
};

#ifndef SWIG
  template <class Traits, int Arity>
  inline std::ostream &operator<<(std::ostream &out,
                                  const Key<Traits, Arity> &nh) {
  nh.show(out);
  return out;
}
#endif

#define IMP_RMF_DECLARE_KEY(lcname, Ucname, PassValue, ReturnValue,     \
                            PassValues, ReturnValues)                   \
  typedef Key<Ucname##Traits, 1> Ucname##Key;                           \
  typedef vector<Ucname##Key> Ucname##Keys;                        \
  typedef Key<Ucname##Traits, 2> Pair##Ucname##Key;                     \
  typedef vector<Pair##Ucname##Key> Pair##Ucname##Keys;            \
  typedef Key<Ucname##Traits, 3> Triplet##Ucname##Key;                  \
  typedef vector<Triplet##Ucname##Key> Triplet##Ucname##Keys;      \
  typedef Key<Ucname##Traits, 4> Quad##Ucname##Key;                     \
  typedef vector<Quad##Ucname##Key> Quad##Ucname##Keys



/** \name Key types
    RMF files support storing a variety of different types of data. These
    include
    - IntKey: store an arbitrary integer as a 64 bit integer
    - FloatKey: store an arbitrary floating point number as a double
    - StringKey: store an arbitrary length string
    - IndexKey: store non-negative indexes as 64 bit integers
    - NodeIDKey: store the ID of a node in the hierarchy
    - NodeIDsKey: store a list of NodeIDs
    - DataSetKey: store a reference to an HDF5 data set via the path to the
    data set
    - IntsKey: store a list of arbitrary integers
    @{
*/
IMP_RMF_FOREACH_TYPE(IMP_RMF_DECLARE_KEY);
/** @} */

} /* namespace RMF */

#endif /* IMPLIBRMF_KEY_H */
