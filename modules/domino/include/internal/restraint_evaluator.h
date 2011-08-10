/**
 *  \file RestraintGraph.h
 *  \brief creates a MRF from a set of particles and restraints
 *
 *  Copyright 2007-2011 IMP Inventors. All rights reserved.
 *
 */

#ifndef IMPDOMINO_INTERNAL_RESTRAINT_EVALUATOR_H
#define IMPDOMINO_INTERNAL_RESTRAINT_EVALUATOR_H

#include "../Subset.h"
#include "../Assignment.h"
#include "../utility.h"
#include "../particle_states.h"
//#include "inference.h"
#include <IMP/Restraint.h>



IMPDOMINO_BEGIN_INTERNAL_NAMESPACE

template <class It>
inline void load_particle_states(It b, It e, const Assignment &ss,
                                 const ParticleStatesTable *pst) {
  IMP_USAGE_CHECK(std::distance(b,e)
                  == static_cast< typename std::iterator_traits<It>
                  ::difference_type>(ss.size()),
                  "Sizes don't match in load particle states");
  unsigned int i=0;
  for (It c=b; c != e; ++c) {
    pst->get_particle_states(*c)->load_particle_state(ss[i], *c);
    ++i;
  }
}

/*
  data for a subset:
  - list f restraints
  - mapping of restraint subset for each subset
  data for each restraint:
  - cache of scores for subset state
*/
class RestraintData {
  typedef IMP::compatibility::map<Assignment, double> Scores;
  mutable Scores scores_;
  Pointer<Restraint> r_;
  double weight_;
  double max_;
  bool cache_;
  mutable int filter_attempts_;
  mutable int filter_passes_;
  template <bool Filter>
  double compute_score(ParticleStatesTable *pst,
                       const ParticlesTemp &ps,
                       const Assignment &state) const {
    load_particle_states(ps.begin(), ps.end(), state, pst);
    RestraintsTemp rs(1, r_);
    double score= r_->get_model()->evaluate_if_good(rs, false)[0];
    if (Filter) {
      if (score >max_) {
        score=std::numeric_limits<double>::max();
      } else {
      }
    }
    /*std::cout << "Computed score for " << r_->get_name()
      << " on " << state << "= " << score << std::endl;*/
    return score;
  }
public:
  RestraintData(Restraint *r): r_(r),
                               max_(std::numeric_limits<double>::max()){
    filter_attempts_=0;
    filter_passes_=0;
    cache_=true;
  }
  void set_score(const Assignment &ss, double s) {
    IMP_USAGE_CHECK(scores_.find(ss) == scores_.end(),
                    "Cannot preload scores twice for state "
                    << ss);
    scores_[ss]=s;
  }
  void set_max(double max) { max_=max;}
  Restraint *get_restraint() const {return r_;}
  template <bool Filter>
  double get_score(ParticleStatesTable *pst,
                   const ParticlesTemp &ps,
                   const Assignment &state) const {
    Scores::const_iterator it= scores_.find(state);
    if (cache_ && it != scores_.end()) {
      /*std::cout << "Found cached score for " << r_->get_name()
        << " on " << state << "= " << it->second
        << "(" << it->first << ")" << std::endl;*/
      double score= it->second;
      IMP_IF_CHECK(USAGE_AND_INTERNAL) {
        double cscore= compute_score<Filter>(pst, ps, state);
        IMP_INTERNAL_CHECK((score >= max_ && cscore >= max_)
                           || (score < max_ && cscore < max_)
                           || (score-cscore) < .01,
                           "Scores don't match their side of max: "
                           << score << " vs " << cscore);
        if (score < max_) {
          IMP_INTERNAL_CHECK((score-cscore) < .1*(score+cscore) +.1,
                             "Scores don't match: " << score
                             << " vs " << cscore);
        }
      }
      return score;
    } else {
      double score= compute_score<Filter>(pst, ps, state);
      IMP_LOG(VERBOSE, "State " << state << " of particles "
              << Particles(ps) << " has score "
              << score << " for restraint " << r_->get_name()
              << std::endl);
      if (Filter) {
        ++filter_attempts_;
        if (score >max_) {
        } else {
          ++filter_passes_;
        }
      }
      if (cache_) {
        scores_[state]=score;
      }
      return score;
    }
  }
  std::pair<int,int> get_statistics() const {
    return std::make_pair(filter_attempts_, filter_passes_);
  }
  void set_use_caching(bool tf) {
    cache_=tf;
    if (!cache_) {
      scores_.clear();
    }
  }
};

struct ModelData;

class IMPDOMINOEXPORT SubsetData {
  const ModelData *md_;
  Ints ris_;
  std::vector<Ints> indices_;
  std::vector<std::pair<double, Ints> > set_ris_;
  std::vector< std::vector<Ints> > set_indices_;
  std::vector<Floats> set_weights_;
  Subset s_;
 public:
  SubsetData(){}
  SubsetData(const ModelData *md,
             const Ints &ris,
             const std::vector<std::pair< double, Ints> > &set_ris,
             std::vector<Ints> indices,
             const std::vector<std::vector<Ints> >& set_indices,
             const std::vector<Floats>& set_weights,
             const Subset &s): md_(md), ris_(ris),
    indices_(indices),
    set_ris_(set_ris), set_indices_(set_indices), set_weights_(set_weights),
    s_(s){}
  double get_score(const Assignment &state) const ;
  bool get_is_ok(const Assignment &state, double max) const ;
  unsigned int get_number_of_restraints() const {
    return ris_.size();
  }
  unsigned int get_number_of_total_restraints() const {
    return set_ris_.size()+ ris_.size();
  }
  Subset get_subset() const {return s_;}
};

struct IMPDOMINOEXPORT ModelData: public RefCounted {
  struct PreloadData {
    Subset s;
    Assignments sss;
    Floats scores;
  };
  typedef IMP::compatibility::map<Restraint*, PreloadData> Preload;
  Preload preload_;
  struct SubsetID {
    const Subset s_;
    const Subsets excluded_;
    SubsetID(const Subset &s,
             const Subsets &excluded): s_(s), excluded_(excluded){}
    bool operator<(const SubsetID &o) const {
      if (s_ < o.s_) return true;
      else if (s_ > o.s_) return false;
      else if (excluded_.size() < o.excluded_.size()) return true;
      else if (excluded_.size() > o.excluded_.size()) return false;
      else {
        for (unsigned int i=0; i< excluded_.size(); ++i) {
          if (excluded_[i] < o.excluded_[i]) return true;
          else if (excluded_[i] > o.excluded_[i]) return false;
        }
        return false;
      }
    }
  };

  bool initialized_, cache_;
  mutable Pointer<RestraintSet> rs_;

  std::vector<RestraintData> rdata_;
  std::vector<std::pair<double, Ints> > sets_;
  std::vector<Floats> set_weights_;
  std::vector<Subset> dependencies_;
  Pointer<ParticleStatesTable> pst_;
  mutable std::map<const SubsetID, SubsetData> sdata_;

  void validate() const;

  void initialize();

  ModelData(RestraintSet *rs,
            ParticleStatesTable* pst);
  Model *get_model() const {
    return rs_->get_model();
  }
  unsigned int get_number_of_restraints(const Subset &s,
                                        const Subsets &exclusions) const;
  const SubsetData &get_subset_data(const Subset &s,
                                    const Subsets &exclude=Subsets()) const;
  void add_score(Restraint *r, const Subset &subset,
                 const Assignment &state, double score);
  void set_use_caching(bool tf);
  IMP_REF_COUNTED_DESTRUCTOR(ModelData);
};




inline double SubsetData::get_score(const Assignment &state) const {
  double score=0;
  for (unsigned int i=0; i< ris_.size(); ++i) {
    Ints ssi(indices_[i].size());
    for (unsigned int j=0; j< ssi.size();++j) {
      ssi[j]= state[indices_[i][j]];
    }
    Assignment ss(ssi);
    ParticlesTemp ps(ss.size());
    for (unsigned int j=0; j< ss.size(); ++j) {
      ps[j]= s_[indices_[i][j]];
    }
    double ms=md_->rdata_[ris_[i]].get_score<false>(md_->pst_,
                                                    ps, ss);
    score+= ms;
  }
  return score;
}
inline bool SubsetData::get_is_ok(const Assignment &state, double max) const {
  double total=0;
  for (unsigned int i=0; i< ris_.size(); ++i) {
    Ints ssi(indices_[i].size());
    for (unsigned int j=0; j< ssi.size();++j) {
      ssi[j]= state[indices_[i][j]];
    }
    Assignment ss(ssi);
    ParticlesTemp ps(ss.size());
    for (unsigned int j=0; j< ss.size(); ++j) {
      ps[j]= s_[indices_[i][j]];
    }
    double ms=md_->rdata_[ris_[i]].get_score<true>(md_->pst_,
                                                   ps, ss);
    if (ms >= std::numeric_limits<double>::max()) {
      return false;
    }
    total+=ms;
    if (total > max) {
      return false;
    }
  }
  for (unsigned int h=0; h < set_ris_.size(); ++h) {
    double set_total=0;
    for (unsigned int i=0; i< set_ris_[h].second.size(); ++i) {
      Ints ssi(set_indices_[h][i].size());
      for (unsigned int j=0; j< ssi.size();++j) {
        ssi[j]= state[set_indices_[h][i][j]];
      }
      Assignment ss(ssi);
      ParticlesTemp ps(ss.size());
      for (unsigned int j=0; j< ss.size(); ++j) {
        ps[j]= s_[set_indices_[h][i][j]];
      }
      double ms=md_->rdata_[set_ris_[h].second[i]].get_score<true>(md_->pst_,
                                                                   ps, ss);
      set_total+=ms *set_weights_[h][i];
      if (set_total > set_ris_[h].first) {
        return false;
      }
    }
  }
  return true;
}
IMPDOMINO_END_INTERNAL_NAMESPACE

#endif  /* IMPDOMINO_INTERNAL_RESTRAINT_EVALUATOR_H */
