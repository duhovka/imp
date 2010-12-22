/**
 * \file atom/CHARMMParameters.h \brief access to Charmm force field parameters
 *
 * Copyright 2007-2010 IMP Inventors. All rights reserved.
 *
 */
#ifndef IMPATOM_CHARMM_PARAMETERS_H
#define IMPATOM_CHARMM_PARAMETERS_H

#include "internal/charmm_helpers.h"
#include "ForceFieldParameters.h"
#include "charmm_topology.h"
#include "atom_macros.h"

#include <string>
#include <fstream>

// swig is being dumb
IMP_BEGIN_NAMESPACE
class VersionInfo;
IMP_END_NAMESPACE

IMPATOM_BEGIN_NAMESPACE

//! The parameters for a CHARMM bond or angle.
struct CHARMMBondParameters {
  double force_constant;
  double ideal;
  IMP_SHOWABLE_INLINE(CHARMMBondParameters,
                      {out << "force constant: " << force_constant
                       << "; ideal value: " << ideal;});
};
IMP_OUTPUT_OPERATOR(CHARMMBondParameters);
IMP_VALUES(CHARMMBondParameters, CHARMMBondParametersList);

//! The parameters for a CHARMM dihedral or improper.
struct CHARMMDihedralParameters {
  double force_constant;
  int multiplicity;
  double ideal;
  IMP_SHOWABLE_INLINE(CHARMMDihedralParameters,
                      {out << "force constant: " << force_constant
                       << "; multiplicity: " << multiplicity
                       << "; ideal value: " << ideal;});
};
IMP_OUTPUT_OPERATOR(CHARMMDihedralParameters);
IMP_VALUES(CHARMMDihedralParameters, CHARMMDihedralParametersList);

//! CHARMM force field parameters.
/** This class reads in topology and parameter files in CHARMM format and
    stores the information.

    It does not actually evaluate the force field itself - there are other
    classes that use the parameters provided by this class to do that. For
    example, the LennardJonesPairScore or CoulombPairScore evaluate the
    nonbond terms of the CHARMM force field, while BondSingletonScore,
    AngleSingletonScore, DihedralSingletonScore and ImproperSingletonScore
    cover the bond terms.

    Typically, the create_topology() method is used to create a new
    CHARMMTopology object for a given Hierarchy; that object can then be
    used to assign atomic radii, bonds, etc.
 */
class IMPATOMEXPORT CHARMMParameters : public ForceFieldParameters {
  std::map<std::string, Pointer<CHARMMIdealResidueTopology> >
      residue_topologies_;
  std::map<std::string, Pointer<CHARMMPatch> > patches_;
  std::map<internal::CHARMMBondNames, CHARMMBondParameters> bond_parameters_;
  std::map<internal::CHARMMAngleNames, CHARMMBondParameters> angle_parameters_;

  typedef std::vector<std::pair<internal::CHARMMDihedralNames,
                                CHARMMDihedralParameters> > DihedralParameters;
  DihedralParameters dihedral_parameters_;
  DihedralParameters improper_parameters_;

  DihedralParameters::const_iterator
        find_dihedral(DihedralParameters::const_iterator begin,
                      DihedralParameters::const_iterator end,
                      const internal::CHARMMDihedralNames &dihedral,
                      bool allow_wildcards) const;

public:

  //! Construction with CHARMM topology (and optionally parameters) file.
  /** For addition of atom types, the topology file alone is enough;
      for adding bonds and radii, both files are needed.
   */
  CHARMMParameters(const String& topology_file_name,
                   const String& par_file_name = std::string());

  /** \name Residue topology

      The class stores the topology of each residue type as defined in the
      topology file, as a set of CHARMMIdealResidueTopology objects.
   */
  /**@{*/
  void add_residue_topology(CHARMMIdealResidueTopology *res) {
    res->set_was_used(true);
    residue_topologies_.insert(std::make_pair(res->get_type(), res));
  }
#if 0
  // return of non const-ref values is not allowed
  CHARMMIdealResidueTopology &get_residue_topology(ResidueType type) {
    std::map<std::string, CHARMMIdealResidueTopology>::iterator it
              = residue_topologies_.find(type.get_string());
    if (it != residue_topologies_.end()) {
      return it->second;
    } else {
      IMP_THROW("Residue " << type << " does not exist", ValueException);
    }
  }
#endif

  CHARMMIdealResidueTopology *get_residue_topology(std::string name) const
  {
    std::map<std::string, Pointer<CHARMMIdealResidueTopology> >::const_iterator
        it = residue_topologies_.find(name);
    if (it != residue_topologies_.end()) {
      return it->second;
    } else {
      IMP_THROW("Residue " << name << " does not exist", ValueException);
    }
  }

  CHARMMIdealResidueTopology *get_residue_topology(ResidueType type) const
  { return get_residue_topology(type.get_string()); }

  /** \name Patches

      The class stores patches as defined in the topology file, as a set of
      CHARMMPatch objects.
   */
  /**@{*/
  void add_patch(CHARMMPatch *patch) {
    patch->set_was_used(true);
    patches_.insert(std::make_pair(patch->get_type(), patch));
  }
#if 0
  // return of non const ref values is not allowed
  CHARMMPatch &get_patch(std::string name) {
    std::map<std::string, CHARMMPatch>::iterator it = patches_.find(name);
    if (it != patches_.end()) {
      return it->second;
    } else {
      IMP_THROW("Patch " << name << " does not exist", ValueException);
    }
  }
#endif

  CHARMMPatch *get_patch(std::string name) const {
    std::map<std::string, Pointer<CHARMMPatch> >::const_iterator it
        = patches_.find(name);
    if (it != patches_.end()) {
      return it->second;
    } else {
      IMP_THROW("Patch " << name << " does not exist", ValueException);
    }
  }
  /**@}*/

  //! Create topology that corresponds to the primary sequence of the Hierarchy.
  CHARMMTopology *create_topology(Hierarchy hierarchy) const;

  //! Get bond parameters for the bond between the two given CHARMM atom types.
  /** The atom types may match in any order.
      \throws IndexException if no parameters are present.
   */
  const CHARMMBondParameters &get_bond_parameters(std::string type1,
                                                  std::string type2) const {
    internal::CHARMMBondNames types = internal::CHARMMBondNames(type1, type2);
    if (bond_parameters_.find(types) != bond_parameters_.end()) {
      return bond_parameters_.find(types)->second;
    } else {
      IMP_THROW("No CHARMM parameters found for bond "
                << type1 << "-" << type2, IndexException);
    }
  }

  //! Get parameters for the angle between the three given CHARMM atom types.
  /** The atom types may match in either forward or reverse order.
      \throws IndexException if no parameters are present.
   */
  const CHARMMBondParameters &get_angle_parameters(std::string type1,
                                                   std::string type2,
                                                   std::string type3) const {
    internal::CHARMMAngleNames types = internal::CHARMMAngleNames(type1, type2,
                                                                  type3);
    if (angle_parameters_.find(types) != angle_parameters_.end()) {
      return angle_parameters_.find(types)->second;
    } else {
      IMP_THROW("No CHARMM parameters found for angle "
                << type1 << "-" << type2 << "-" << type3, IndexException);
    }
  }

  //! Get parameters for the dihedral between the four given CHARMM atom types.
  /** The atom types may match in either forward or reverse order. When
      looking for a match in the library, wildcards are considered; an atom
      type of X in the library will match any atom type. The most specific
      match from the library is returned.

      Multiple sets of parameters can be specified for the same combination
      of atom types in the library, in which case all of them are returned.

      \throws IndexException if no parameters are present.
   */
  CHARMMDihedralParametersList get_dihedral_parameters(
             std::string type1, std::string type2, std::string type3,
             std::string type4) const {
    CHARMMDihedralParametersList param;
    internal::CHARMMDihedralNames types = internal::CHARMMDihedralNames(
                    type1, type2, type3, type4);
    // Get the first match, using wildcards
    DihedralParameters::const_iterator match =
        find_dihedral(dihedral_parameters_.begin(),
                      dihedral_parameters_.end(), types, true);
    if (match != dihedral_parameters_.end()) {
      // If it matched, look for duplicate dihedral terms (this time the
      // match must be exactly the same as the first match)
      param.push_back(match->second);
      while ((match = find_dihedral(match + 1, dihedral_parameters_.end(),
                                    match->first, false))
             != dihedral_parameters_.end()) {
        param.push_back(match->second);
      }
    }
    if (param.size() == 0) {
      IMP_THROW("No CHARMM parameters found for dihedral "
                << type1 << "-" << type2 << "-" << type3 << "-" << type4,
                IndexException);
    } else {
      return param;
    }
  }

  //! Get parameters for the improper between the four given CHARMM atom types.
  /** The atom types may match in either forward or reverse order. When
      looking for a match in the library, wildcards are considered; an atom
      type of X in the library will match any atom type. The most specific
      match from the library is returned.

      \throws IndexException if no parameters are present.
   */
  const CHARMMDihedralParameters &get_improper_parameters(
             std::string type1, std::string type2, std::string type3,
             std::string type4) const {
    internal::CHARMMDihedralNames types = internal::CHARMMDihedralNames(
                    type1, type2, type3, type4);
    // Return just the first match; wildcards are OK
    DihedralParameters::const_iterator it =
        find_dihedral(improper_parameters_.begin(),
                      improper_parameters_.end(), types, true);
    if (it != improper_parameters_.end()) {
      return it->second;
    } else {
      IMP_THROW("No CHARMM parameters found for improper "
                << type1 << "-" << type2 << "-" << type3 << "-" << type4,
                IndexException);
    }
  }

  //! Auto-generate Angle particles from the passed list of Bond particles.
  /** The angles consist of all unique pairs of bonds which share an
      endpoint. If no parameters are found for an angle, it is simply
      created without those parameters.

      The list of newly-created Angle particles can be passed to a
      StereochemistryPairFilter to exclude 1-3 interactions from the
      nonbonded list, or to an AngleSingletonScore to score each angle.

      \return a list of the newly-created Angle particles.

      \see CHARMMTopology::add_bonds().
   */
  Particles create_angles(Particles bonds) const;

  //! Auto-generate Dihedral particles from the passed list of Bond particles.
  /** The dihedrals consist of all unique triples of bonds which form
      dihedrals. If no parameters are found for a dihedral, it is simply
      created without those parameters; if multiple sets of parameters are
      found, multiple copies of the dihedral are created, each with one set
      of parameters.

      The list of newly-created Dihedral particles can be passed to a
      StereochemistryPairFilter to exclude 1-4 interactions from the
      nonbonded list, or to a DihedralSingletonScore to score each dihedral.

      \return a list of the newly-created Dihedral particles.

      \see CHARMMTopology::add_bonds().
   */
  Particles create_dihedrals(Particles bonds) const;

  IMP_FORCE_FIELD_PARAMETERS(CHARMMParameters);
private:

  virtual String get_force_field_atom_type(Atom atom) const;

  void read_parameter_file(std::ifstream& input_file);
  // read topology file
  void read_topology_file(std::ifstream& input_file);

  void add_angle(Particle *p1, Particle *p2, Particle *p3, Particles &ps) const;
  void add_dihedral(Particle *p1, Particle *p2, Particle *p3, Particle *p4,
                    Particles &ps) const;

  ResidueType parse_residue_line(const String& line);
  void parse_atom_line(const String& line, ResidueType curr_res_type,
                       CHARMMResidueTopologyBase *residue);
  void parse_bond_line(const String& line, ResidueType curr_res_type,
                       CHARMMResidueTopologyBase *residue);

  void parse_nonbonded_parameters_line(String line);
  void parse_bonds_parameters_line(String line);
  void parse_angles_parameters_line(String line);
  void parse_dihedrals_parameters_line(String line,
                                       DihedralParameters &param);
  WarningContext warn_context_;
};

IMP_OBJECTS(CHARMMParameters, CHARMMParametersList);



/** The default CHARMM parameters support normal amino acid
    and nucleic acid residues and the atoms found in them.
    To use CHARMM with heterogens or non-standard residues,
    a different CHARMM parameters file must be used.

    No hydrogen parameters are read.

    \see get_all_atom_CHARMM_parameters();
*/
IMPATOMEXPORT CHARMMParameters* get_heavy_atom_CHARMM_parameters();


/** The default CHARMM parameters support normal amino acid
    and nucleic acid residues and the atoms found in them.
    To use CHARMM with heterogens or non-standard residues,
    a different CHARMM parameters file must be used.

    \see get_heavy_atom_CHARMM_parameters()
*/
IMPATOMEXPORT CHARMMParameters* get_all_atom_CHARMM_parameters();

IMPATOM_END_NAMESPACE

#endif /* IMPATOM_CHARMM_PARAMETERS_H */
