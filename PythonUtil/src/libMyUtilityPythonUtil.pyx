import pypdt
from libcpp.vector cimport vector
from libcpp.map cimport map
from libcpp.pair cimport pair
from cython.operator cimport dereference as deref
from cpython.ref cimport PyObject
import ROOT
try:
    ROOT.reco.GenParticle
except AttributeError:
    from DataFormats.FWLite import Events
genPartType = ROOT.reco.GenParticle


cdef extern from "DataFormats/HepMCCandidate/interface/GenParticle.h" namespace "reco":
    cdef cppclass GenParticle:
        GenParticle() except +
        size_t numberOfDaughters()
        GenParticle * daughter(size_t) const
        int pdgId()
ctypedef const GenParticle * GenPartPtr


cdef extern from "TPython.h":
    cdef cppclass TPython:
        TPython() except +
        void * ObjectProxy_AsVoidPtr(PyObject *)
        PyObject * ObjectProxy_FromVoidPtr(void *, const char *)
cdef TPython tp


def get_all_daughters(gen_particles):
    cdef map[GenPartPtr, int] all_daughters
    cdef vector[GenPartPtr] all_daughters_vec
    cdef vector[GenPartPtr] * tmp1 = new vector[GenPartPtr]()
    cdef vector[GenPartPtr] * tmp2 = new vector[GenPartPtr]()
    cdef size_t i
    cdef size_t j
    cdef GenPartPtr cgp1
    cdef GenPartPtr cgp2

    # start with direct daughters
    for gp in gen_particles:
        if not type(gp) == genPartType:
             raise RuntimeError(
                 'get_all_daughters only accepts a list of GenParticle object')
        cgp1 = <GenParticle *> tp.ObjectProxy_AsVoidPtr(<PyObject *> gp) 
        for i in range(cgp1.numberOfDaughters()):
            cgp2 = <GenParticle *> cgp1.daughter(i)
            if not all_daughters.count(cgp2):
                all_daughters[cgp2] = 1
                all_daughters_vec.push_back(cgp2)
                tmp1.push_back(cgp2)

    while tmp1.size() > 0:
        for cgp1 in deref(tmp1):
            for j in range(cgp1.numberOfDaughters()):
                cgp2 = <GenParticle *> cgp1.daughter(j)
                if not all_daughters.count(cgp2):
                    all_daughters[cgp2] = 1
                    all_daughters_vec.push_back(cgp2)
                    tmp2.push_back(cgp2)
        tmp1.clear()
        tmp1, tmp2 = tmp2, tmp1

    del tmp1
    del tmp2

    return list(
        <object> tp.ObjectProxy_FromVoidPtr(<void *> cgp1, 'reco::GenParticle')
        for cgp1 in all_daughters_vec
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



