import IMP
import IMP.test
import IMP.atom


class Tests(IMP.test.TestCase):

    def test_read(self):
        """Check getting a residue from an atomic protein"""
        m = IMP.kernel.Model()

        #! read PDB
        mp = IMP.atom.read_pdb(self.open_input_file("input.pdb"),
                               m, IMP.atom.NonWaterPDBSelector())
        chain = mp.get_child(0)
        r = IMP.atom.get_residue(chain, 10)
        self.assertNotEqual(r, IMP.atom.Hierarchy())
        r.show()
        rr = IMP.atom.Residue(r.get_particle())
        self.assertEqual(rr.get_index(), 10)

    def test_residue_mass(self):
        """Check getting a mass for a residue"""
        m = IMP.kernel.Model()

        #! read PDB
        mp = IMP.atom.read_pdb(self.open_input_file("input.pdb"),
                               m, IMP.atom.NonWaterPDBSelector())
        chain = mp.get_child(0)
        r = IMP.atom.get_residue(chain, 10)
        rr = IMP.atom.Residue(r.get_particle())
        mass = IMP.atom.get_mass(rr.get_residue_type())
        self.assertEqual(mass, 71.079)

    def test_chain(self):
        """Check getting a residue from a cg protein"""
        IMP.base.set_log_level(IMP.base.VERBOSE)
        m = IMP.kernel.Model()
        p = IMP.kernel.Particle(m)
        mp = IMP.atom.create_protein(m, "a protein", 10, 100)
        r = IMP.atom.get_residue(mp, 10)
        r.show()
        self.assertNotEqual(r, IMP.atom.Hierarchy())

if __name__ == '__main__':
    IMP.test.main()
