try:
    import pypdt
except ImportError:
    pypdt = None


def daughters_of_z_boson(gen_particles, invert=False):
    z_bosons = list(gp for gp in gen_particles if abs(gp.pdgId()) == 23)
    return get_all_daughters(gen_particles, z_bosons, invert)


def get_all_daughters(gen_particle_collection, mothers, invert=False):
    tag = dict(
        (id(m.daughter(d)), True)               # start with direct daughters
        for m in mothers
        for d in xrange(m.numberOfDaughters())
    )
    for gp in gen_particle_collection:
        id_gp = id(gp)
        if id_gp in tag:
            continue
        elif any(                               # is any mom tagged?
            tag.get(id(gp.mother(i)), False)
            for i in xrange(gp.numberOfMothers())
        ):
            tag[id_gp] = True
    return list(
        p
        for p in gen_particle_collection
        if ((id(p) in tag) != invert)
    )


_b_ids = {}
def _has_b_cont(id):
    val = _b_ids.get(id, -1)
    if val == -1:
        val = pypdt.hasBottom(id)
        if val:
            _b_ids[id] = 1
        else:
            _b_ids[id] = 0
    return val


def final_b_hadrons(gen_particles):
    res = []
    for gp in gen_particles:
        if _has_b_cont(abs(gp.pdgId())) and not any(
                _has_b_cont(abs(gp.daughter(i).pdgId()))
                for i in xrange(gp.numberOfDaughters())
        ):
            res.append(gp)
    return res


def final_d_hadrons(gen_particles):
    res = []
    for gp in gen_particles:
        if pypdt.hasCharm(abs(gp.pdgId())) and not any(
                pypdt.hasCharm(abs(gp.daughter(i).pdgId()))
                for i in xrange(gp.numberOfDaughters())
        ):
            res.append(gp)
    return res


import pyximport
pyximport.install()
from libMyUtilityPythonUtil import final_b_hadrons
from libMyUtilityPythonUtil import get_all_daughters as get_all_fast

def get_all_daughters(gen_p, mothers, inv=False):
    return get_all_fast(mothers)


