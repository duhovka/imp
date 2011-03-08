import IMP
import IMP.core
import IMP.algebra
import IMP.domino
import IMP.atom
import IMP.membrane
import math

def create_representation(seq,tmh,topo):

    m=IMP.Model()
    tbr= IMP.core.TableRefiner()
    all=IMP.atom.Hierarchy.setup_particle(IMP.Particle(m))

    def generate_balls(ii,seq,tmh,sign):
        nres=len(seq)
        atoms=[]
        for i in range(nres):
            x=2.3*math.cos(math.radians(100.0)*float(i))
            y=2.3*math.sin(math.radians(100.0)*float(i))
            z=sign*1.51*(float(i)-float((nres-1))/2.0)
            # set up residue
            p=IMP.Particle(m)
            r=IMP.atom.Residue.setup_particle(p, IMP.atom.get_residue_type(seq[i]), i+tmh[0])
            rt=r.get_residue_type()
            vol=IMP.atom.get_volume_from_residue_type(rt)
            rg=IMP.algebra.get_ball_radius_from_volume_3d(vol)
            #rd=IMP.core.XYZR.setup_particle(p, IMP.algebra.Sphere3D(IMP.algebra.Vector3D(x,y,z),2.273))
            rd=IMP.core.XYZR.setup_particle(p, IMP.algebra.Sphere3D(IMP.algebra.Vector3D(x,y,z),rg))
            # set up atom
            p1=IMP.Particle(m)
            a=IMP.atom.Atom.setup_particle(p1, IMP.atom.AT_CA)
            ad=IMP.core.XYZR.setup_particle(p1, IMP.algebra.Sphere3D(IMP.algebra.Vector3D(x,y,z),rg))
            r.add_child(a)
            all.add_child(r)
            atoms.append(ad)
            if ( i == 0 ):      bb=z
            if ( i == nres-1 ): ee=z
        rb=IMP.Particle(m)
        rb=IMP.atom.create_rigid_body(atoms,"TM"+str(ii))
        tbr.add_particle(rb,atoms)
        d_rbs=IMP.membrane.HelixDecorator.setup_particle(rb,bb,ee)

    for i in range(len(seq)):
        generate_balls(i,seq[i],tmh[i],topo[i])

    return m,all,tbr

def create_restraints(m, chain, tbr, TMH):

    def add_excluded_volume():
        lsc= IMP.container.ListSingletonContainer(m)
        for h in TMH:
            s=IMP.atom.Selection(chain, residue_indexes=[(h[0],h[1]+1)])
            lsc.add_particles(s.get_selected_particles())
        evr=IMP.core.ExcludedVolumeRestraint(lsc,1)
        m.add_restraint(evr)
        m.set_maximum_score(evr, 17.0)

    def add_distance_restraint(s0, s1, x0, k):
        hub= IMP.core.HarmonicUpperBound(x0,k)
        df= IMP.core.DistancePairScore(hub)
        dr= IMP.core.PairRestraint(df, IMP.ParticlePair(s0, s1))
        m.add_restraint(dr)
        m.set_maximum_score(dr, .01)
        return dr

    def add_packing_restraint():
## if the helices are interacting, apply a filter on the crossing angle
## first define the allowed intervals, by specifying the center
## of the distributions (Walters and DeGrado PNAS (2007) 103:13658)
        om0=(-156.5, 146.4, -37.9, 13.8, 178.0, 25.5, -161.1, 44.8, 127.4, -60.2, -129.2, 2.4, 161.0)
#  the sigmas
        sig_om0=(10.1, 13.6, 7.50, 16.6, 20.8, 11.2, 10.3, 8.8, 12.3, 14.8, 12.9, 16.2, 17.6)
#  distance cutoff
        dd0=(8.61, 8.57, 7.93, 9.77, 9.14, 8.55, 9.30, 7.96, 9.40, 8.61, 8.97, 8.55, 8.75)
#  and distance sigmas
        sig_dd0=(0.89, 0.99, 0.88, 1.18, 1.47, 1.05, 1.57, 1.13, 1.0, 1.04, 1.65, 0.78, 1.33)
#  the allowed number of sigma
        nsig=3
#  and the number of clusters
        ncl=13
# create allowed intervals (omega in radians)
# and control periodicity
        om_b=[]; om_e=[]; dd_b=[]; dd_e=[]
        for i in range(ncl):
            omb=math.radians(om0[i]-nsig*sig_om0[i])
            ome=math.radians(om0[i]+nsig*sig_om0[i])
            ddb=dd0[i]-nsig*sig_dd0[i]
            dde=dd0[i]+nsig*sig_dd0[i]
            om_b.append(max(omb,-math.pi))
            om_e.append(min(ome,math.pi))
            dd_b.append(ddb)
            dd_e.append(dde)
            if ( omb < -math.pi ):
                om_b.append(2*math.pi+omb)
                om_e.append(math.pi)
                dd_b.append(ddb)
                dd_e.append(dde)
            if ( ome > math.pi ):
                om_b.append(-math.pi)
                om_e.append(-2*math.pi+ome)
                dd_b.append(ddb)
                dd_e.append(dde)
        lrb= IMP.container.ListSingletonContainer(m)
        for h in TMH:
            s0=IMP.atom.Selection(chain, atom_type = IMP.atom.AT_CA, residue_index = h[0])
            rb=IMP.core.RigidMember(s0.get_selected_particles()[0]).get_rigid_body()
            lrb.add_particle(rb)
        nrb= IMP.container.ClosePairContainer(lrb, 25.0)
        ps=  IMP.membrane.RigidBodyPackingScore(tbr, om_b, om_e, dd_b, dd_e)
        prs= IMP.container.PairsRestraint(ps, nrb)
        m.add_restraint(prs)
        m.set_maximum_score(prs, .01)

## DOPE/GQ scoring
    def add_DOPE():
        IMP.atom.add_dope_score_data(chain)
        dsc=IMP.container.ListSingletonContainer(m)
        for h in TMH:
            s=IMP.atom.Selection(chain, atom_type = IMP.atom.AT_CA, residue_indexes=[(h[0],h[1]+1)])
            dsc.add_particles(s.get_selected_particles())
        dpc=IMP.container.ClosePairContainer(dsc, 15.0, 0.0)
        f=IMP.membrane.SameHelixPairFilter()
        dpc.add_pair_filter(f)
        dps=IMP.atom.DopePairScore(15.0, IMP.atom.get_data_path("dope_new.lib"))
        dope=IMP.container.PairsRestraint(dps, dpc)
        m.add_restraint(dope)
        m.set_maximum_score(dope, .01)

    def add_interacting_restraint():
        rbs=[]
        for h in TMH:
            s0=IMP.atom.Selection(chain, atom_type = IMP.atom.AT_CA, residue_index = h[0])
            rbs.append(IMP.core.RigidMember(s0.get_selected_particles()[0]).get_rigid_body())
        lpc= IMP.container.ListPairContainer(m)
        lpc.add_particle_pair([rbs[0],rbs[3]])
        hub= IMP.core.HarmonicUpperBound(4.0,1)
        sd=  IMP.core.SphereDistancePairScore(hub)
        kc=  IMP.core.KClosePairsPairScore(sd,tbr,1)
        ir=  IMP.container.PairsRestraint(kc, lpc)
        m.add_restraint(ir)
        m.set_maximum_score(ir, .01)
        return ir

# assembling all the restraints
    add_excluded_volume()
    rset=IMP.RestraintSet()
    for i in range(len(TMH)-1):
        s0=IMP.atom.Selection(chain, atom_type = IMP.atom.AT_CA, residue_index = TMH[i][1])
        s1=IMP.atom.Selection(chain, atom_type = IMP.atom.AT_CA, residue_index = TMH[i+1][0])
        p0=s0.get_selected_particles()[0]
        p1=s1.get_selected_particles()[0]
        rb0=IMP.core.RigidMember(p0).get_rigid_body()
        rb1=IMP.core.RigidMember(p1).get_rigid_body()
        length=1.6*(TMH[i+1][0]-TMH[i][1]+1)+7.4
        dr=add_distance_restraint(p0,p1,length,1000)
        rdr=add_distance_restraint(rb0,rb1,27.0,1000)
        rset.add_restraint(dr)
    add_packing_restraint()
    add_DOPE()
    #ir=add_interacting_restraint()
    #rset.add_restraint(ir)
    return rset

# creating the discrete states for domino
def  create_discrete_states(m,chain,TMH):
#   store initial rotation to have the right topology
    rot0=IMP.algebra.get_rotation_about_axis(IMP.algebra.Vector3D(0,1,0), math.pi/2.0)
    trs0=[]; trs1=[]; trs2=[]; trs3=[]; trs4=[]; trs5=[]; trs6=[]
    for i in range(0,1):
        rotz=IMP.algebra.get_rotation_about_axis(IMP.algebra.Vector3D(0,0,1), i*math.pi/6)
        for t in range(0,1):
            tilt=IMP.algebra.get_rotation_about_axis(IMP.algebra.Vector3D(0,1,0), t*math.pi/18)
            rot1=IMP.algebra.compose(tilt,rotz)
            for s in range(0,1):
                if ( t == 0 ) and ( s != 0 ) : break
                swing=IMP.algebra.get_rotation_about_axis(IMP.algebra.Vector3D(0,0,1), s*math.pi/6)
                rot2=IMP.algebra.compose(swing,rot1)
                trs0.append(IMP.algebra.ReferenceFrame3D(IMP.algebra.Transformation3D(IMP.algebra.compose(rot2,rot0),
                                        IMP.algebra.Vector3D(0,0,0))))
                for dx in range(0,20,1):
                    if ( dx >= 7 ):
                        trs1.append(IMP.algebra.ReferenceFrame3D(IMP.algebra.Transformation3D(IMP.algebra.compose(rot2,rot0),
                                    IMP.algebra.Vector3D(dx,0,0))))
                    for dz in range(0,1):
                        for dy in range(0,20,1):
                            trs2.append(IMP.algebra.ReferenceFrame3D(IMP.algebra.Transformation3D(IMP.algebra.compose(rot2,rot0),
                                        IMP.algebra.Vector3D(dx,dy,dz))))
                            trs3.append(IMP.algebra.ReferenceFrame3D(IMP.algebra.Transformation3D(IMP.algebra.compose(rot2,rot0),
                                        IMP.algebra.Vector3D(dx,dy,dz))))
                            trs4.append(IMP.algebra.ReferenceFrame3D(IMP.algebra.Transformation3D(IMP.algebra.compose(rot2,rot0),
                                        IMP.algebra.Vector3D(dx,dy,dz))))
                            trs5.append(IMP.algebra.ReferenceFrame3D(IMP.algebra.Transformation3D(IMP.algebra.compose(rot2,rot0),
                                        IMP.algebra.Vector3D(dx,dy,dz))))
                            trs6.append(IMP.algebra.ReferenceFrame3D(IMP.algebra.Transformation3D(IMP.algebra.compose(rot2,rot0),
                                        IMP.algebra.Vector3D(dx,dy,dz))))

    pst= IMP.domino.ParticleStatesTable()
    pstate=[]
    pstate.append(IMP.domino.RigidBodyStates(trs0))
    pstate.append(IMP.domino.RigidBodyStates(trs1))
    pstate.append(IMP.domino.RigidBodyStates(trs2))
    pstate.append(IMP.domino.RigidBodyStates(trs3))
    pstate.append(IMP.domino.RigidBodyStates(trs4))
    pstate.append(IMP.domino.RigidBodyStates(trs5))
    pstate.append(IMP.domino.RigidBodyStates(trs6))
    for i,h in enumerate(TMH):
        s0=IMP.atom.Selection(chain, atom_type = IMP.atom.AT_CA, residue_index = h[0])
        rb=IMP.core.RigidMember(s0.get_selected_particles()[0]).get_rigid_body()
        pst.set_particle_states(rb, pstate[i])
    return pst

# setting up domino (and filters)
def create_sampler(m, rset, pst):
    s=IMP.domino.DominoSampler(m, pst)
    ig= IMP.domino.get_interaction_graph(rset, pst)
    jt= IMP.domino.get_junction_tree(ig)
#   set filters
    filters=[]
    # do not allow particles with the same ParticleStates object
    # to have the same state index
    filters.append(IMP.domino.ExclusionSubsetFilterTable(pst))
    # filter states that score worse than the cutoffs in the Model
    filters.append(IMP.domino.RestraintScoreSubsetFilterTable(m, pst))
    states= IMP.domino.BranchAndBoundSubsetStatesTable(pst, filters)
    s.set_subset_states_table(states)
    s.set_subset_filter_tables(filters)
    return s

def display(m,chain,TMH,name):
    m.update()
    w= IMP.display.PymolWriter(name)
    for i,h in enumerate(TMH):
        jj=0
        s=IMP.atom.Selection(chain, residue_indexes=[(h[0],h[1]+1)])
        ps=s.get_selected_particles()
        for p in ps:
            jj+=1
            g= IMP.display.XYZRGeometry(p)
            g.set_name("TM"+str(i))
            c=i
            if ( jj == 1 ) : c=10
            g.set_color(IMP.display.get_display_color(c))
            w.add_geometry(g)

# Here starts the real job...
#IMP.set_log_level(IMP.VERBOSE)

# TM regions
TMH= [[3, 21], [31, 61], [67, 97], [102, 113], [121, 154], [161, 180], [191, 219]]

# number of TMH
nTMH=len(TMH)

# define TMH sequences
seq0=("M","V","G","L","T","T","L","F","W","L","G","A","I","G","M","L","V","G","T","L","A","F","A","W","A","G","R","D","A","G","S","G","E","R","R","Y","Y","V","T","L","V","G","I","S","G","I","A","A","V","A","Y","V","V","M","A","L","G","V","G","W","V","P","V","A","E","R","T","V","F","A","P","R","Y","I","D","W","I","L","T","T","P","L","I","V","Y","F","L","G","L","L","A","G","L","D","S","R","E","F","G","I","V","I","T","L","N","T","V","V","M","L","A","G","F","A","G","A","M","V","P","G","I","E","R","Y","A","L","F","G","M","G","A","V","A","F","L","G","L","V","Y","Y","L","V","G","P","M","T","E","S","A","S","Q","R","S","S","G","I","K","S","L","Y","V","R","L","R","N","L","T","V","I","L","W","A","I","Y","P","F","I","W","L","L","G","P","P","G","V","A","L","L","T","P","T","V","D","V","A","L","I","V","Y","L","D","L","V","T","K","V","G","F","G","F","I","A","L","D","A","A","A","T","L")

seq=[]
for h in TMH:
    tmp=[]
    for j in range(h[0],h[1]+1):
        tmp.append(seq0[j-1])
    seq.append(tmp)

# define the topology
topo=[-1.0,1.0,-1.0,1.0,-1.0,1.0,-1.0]

print "creating representation"
(m,chain,tbr)=create_representation(seq,topo)

print "creating score function"
rset=create_restraints(m,chain,tbr,TMH)

print "creating discrete states"
pst=create_discrete_states(m,chain,TMH)

print "creating sampler"
s=create_sampler(m, rset, pst)

print "sampling"
ass=IMP.domino.Subset(pst.get_particles())
cs=s.get_sample_states(ass)

print "found ", len(cs), "solutions"

score=[]
for v in cs:
    IMP.domino.load_particle_states(ass,v,pst)
    ss=m.evaluate(False)
    score.append(ss)

topscore = 30
topscore = min(topscore,len(cs))
print "visualizing the top ",topscore
for i in range(0,topscore):
    low=min(score)
    ii=score.index(low)
    score[ii]=10000000.
    print "** solution number:",i," is:",low
    IMP.domino.load_particle_states(ass,cs[ii],pst)
    display(m,chain,TMH,"sol_"+str(i)+".score_"+str(low)+".pym")
