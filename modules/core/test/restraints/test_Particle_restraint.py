import unittest
import IMP
import IMP.test
import IMP.utils
import IMP.core
import math


# This file is generated by the make-container script

class ParticleContainerTest(IMP.test.TestCase):
    """Tests for ParticleContainer related objects"""

    def create_particle(self,m):
        p= IMP.Particle()
        m.add_particle(p)
        p.add_attribute(IMP.FloatKey("thekey"), 1)
        return p

    def create_particle_pair(self,m):
        p0= IMP.Particle()
        m.add_particle(p0)
        p1= IMP.Particle()
        m.add_particle(p1)
        d0= IMP.core.XYZDecorator.create(p0)
        d1= IMP.core.XYZDecorator.create(p1)
        d0.set_coordinates(IMP.Vector3D(0,0,1))
        d1.set_coordinates(IMP.Vector3D(0,0,0))
        return IMP.ParticlePair(p0,p1)

    def create_particle_score(self):
        uf= IMP.core.Linear(0,1)
        return IMP.core.AttributeSingletonScore(uf,IMP.FloatKey("thekey"))

    def create_particle_pair_score(self):
        uf= IMP.core.Linear(0,1)
        return IMP.core.DistancePairScore(uf)


    def test_container(self):
        """Test the ParticlesRestraint"""
        m= IMP.Model()
        c= IMP.core.ListParticleContainer()
        for i in range(0,10):
            c.add_particle(self.create_particle(m))
            r= IMP.core.ParticlesRestraint(self.create_particle_score(), c)
        m.add_restraint(r)
        self.assertEqual(m.evaluate(False), 10)
        c.add_particle(self.create_particle(m))
        self.assertEqual(m.evaluate(False), 11)


    def test_container(self):
        """Test backwards compatibility on ParticlesRestraint"""
        m= IMP.Model()
        r= IMP.core.ParticlesRestraint(self.create_particle_score())
        m.add_restraint(r)

        for i in range(0,10):
            r.add_particle(self.create_particle(m))
        self.assertEqual(m.evaluate(False), 10)
        r.add_particle(self.create_particle(m))
        self.assertEqual(m.evaluate(False), 11)

    def test_filter(self):
        """Testing FilteredListParticleContainer"""
        IMP.set_log_level(IMP.VERBOSE)
        m= IMP.Model()
        c= IMP.core.FilteredListParticleContainer()
        f= IMP.core.ListParticleContainer()
        c.add_particle_container(f)
        for i in range(0,10):
            f.add_particle(self.create_particle(m))
        print "done 1"
        for i in range(0,10):
            c.add_particle(self.create_particle(m))
        print "done 2"
        for p in f.get_particles():
            c.add_particle(p)
        self.assertEqual(c.get_number_of_particles(), 10)
        print "all done"

if __name__ == '__main__':
    unittest.main()
