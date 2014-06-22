import pypdt
from libcpp.map cimport map


def get_all_daughters(gen_particle_collection, mothers, invert=False):
    cdef map[int,int] tag
    cdef int id_gp 
    cdef int d
    cdef int i
    cdef size_t ivrt = 0 if invert else 1

    # start with direct daughters
    for m in mothers:
        for d in range(m.numberOfDaughters()):
            tag[id(m.daughter(d))] = 1

    for gp in gen_particle_collection:
        id_gp = id(gp)
        if tag.count(id_gp):
            continue
        for i in range(gp.numberOfMothers()):
            if tag.count(id(gp.mother(i))):
                tag[id_gp] = 1
                break

    return list(
        p
        for p in gen_particle_collection
        if tag.count(id(p)) != invert
    )


cdef map[int,int] * _b_ids = new map[int,int]()
cdef int _has_b_cont(int id):
    if _b_ids.count(id):
        return _b_ids[0][id]
    else:
        if pypdt.hasBottom(id):
            _b_ids[0][id] = 1
            return 1
        else:
            _b_ids[0][id] = 0
            return 0


def final_b_hadrons(gen_particles):
    res = []
    for gp in gen_particles:
        if _has_b_cont(abs(gp.pdgId())) and not any(
                _has_b_cont(abs(gp.daughter(i).pdgId()))
                for i in xrange(gp.numberOfDaughters())
        ):
            res.append(gp)
    return res



