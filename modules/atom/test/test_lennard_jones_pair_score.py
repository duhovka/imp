import io
import random
import math
import IMP
import IMP.test
import IMP.atom
import IMP.core

from test_coulomb import place_xyzs


def make_test_pair_score(min_distance=9.0, max_distance=10.0):
    m = IMP.Model()
    p0 = m.add_particle("p0")
    sph = IMP.algebra.Sphere3D(IMP.algebra.Vector3D(0, 0, 0), 1.0)
    IMP.core.XYZR.setup_particle(m, p0, sph)
    d0 = IMP.atom.LennardJones.setup_particle(m, p0, 1.0)
    p1 = m.add_particle("p1")
    IMP.core.XYZR.setup_particle(m, p1, sph)
    d1 = IMP.atom.LennardJones.setup_particle(m, p1, 1.0)
    sm = IMP.atom.ForceSwitch(min_distance, max_distance)
    c = IMP.atom.LennardJonesPairScore(sm)
    r = IMP.core.PairRestraint(m, c, (p0, p1))
    sf = IMP.core.RestraintsScoringFunction([r])
    return m, sf, d0, d1, c


class Tests(IMP.test.TestCase):

    """Test the LennardJonesPairScore"""

    def test_get_set(self):
        """Check LennardJonesPairScore get/set methods"""
        sm = IMP.atom.ForceSwitch(9.0, 10.0)
        c = IMP.atom.LennardJonesPairScore(sm)
        self.assertEqual(c.get_repulsive_weight(), 1.0)
        c.set_repulsive_weight(5.0)
        self.assertEqual(c.get_repulsive_weight(), 5.0)
        self.assertEqual(c.get_attractive_weight(), 1.0)
        c.set_attractive_weight(10.0)
        self.assertEqual(c.get_attractive_weight(), 10.0)

    def test_value(self):
        """Check score value of LennardJonesPairScore"""
        m, sf, d0, d1, c = make_test_pair_score()

        box = IMP.algebra.Vector3D(10.0, 20.0, 30.0)
        for r0 in (2.0, 1.0):
            d0.set_radius(r0)
            for r1 in (2.0, 1.0):
                d1.set_radius(r1)
                rmin = r0 + r1
                for wd0 in (0.0, 1.0, 2.0):
                    d0.set_well_depth(wd0)
                    for wd1 in (0.0, 1.0, 2.0):
                        d1.set_well_depth(wd1)
                        wd = math.sqrt(wd0 * wd1)
                        for att in (0.0, 0.5, 1.0):
                            c.set_attractive_weight(att)
                            for rep in (0.0, 0.5, 1.0):
                                c.set_repulsive_weight(rep)
                                for r in (3.0, 4.0, 5.0):
                                    place_xyzs(d0, d1, box, r)
                                    score = sf.evaluate(False)
                                    expected = wd * (rep * (rmin / r) ** 12
                                                     - 2.0 * att * (rmin / r) ** 6)
                                    self.assertAlmostEqual(score, expected,
                                                           delta=1e-2)

    def test_derivatives(self):
        """Check derivatives of LennardJonesPairScore"""
        m, sf, d0, d1, c = make_test_pair_score(4.0, 6.0)
        # Place one particle at the origin and the other at a random position
        # between 1 and 6 angstroms away (not too close since the derivatives
        # are too large there)
        d0.set_coordinates(IMP.algebra.Vector3D(0, 0, 0))
        d1.set_coordinates(IMP.algebra.get_random_vector_on(IMP.algebra.get_unit_sphere_3d())
                           * (random.random() * 5.0 + 1.0))
        self.assertXYZDerivativesInTolerance(sf, d0, 2.0, 5.0)
        self.assertXYZDerivativesInTolerance(sf, d1, 2.0, 5.0)

    def test_smoothing(self):
        """Check smoothing of LennardJonesPairScore"""
        m, sf, d0, d1, c = make_test_pair_score()
        smm, smsf, smd0, smd1, smc = make_test_pair_score(min_distance=4.0,
                                                          max_distance=5.0)
        box = IMP.algebra.Vector3D(10.0, 20.0, 30.0)

        def place_all(dist):
            place_xyzs(d0, d1, box, dist)
            smd0.set_coordinates(d0.get_coordinates())
            smd1.set_coordinates(d1.get_coordinates())

        # For dist <= min_distance, scores should be identical
        for dist in (3.0, 3.5, 4.0):
            place_all(dist)
            self.assertAlmostEqual(sf.evaluate(False), smsf.evaluate(False),
                                   delta=1e-6)

        # For dist > max_distance, smoothed score should be zero
        place_all(5.5)
        self.assertEqual(smsf.evaluate(False), 0.0)
        self.assertNotEqual(sf.evaluate(False), 0.0)

if __name__ == '__main__':
    IMP.test.main()
