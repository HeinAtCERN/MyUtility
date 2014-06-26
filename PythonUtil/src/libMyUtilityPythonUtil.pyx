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


cdef int _is_final_b(GenPartPtr cgp):
    cdef size_t i
    cdef int no_b_dau = 1
    cdef GenPartPtr dau
    if _has_b_cont(abs(cgp.pdgId())):
        for i in range(cgp.numberOfDaughters()):
            dau = <GenPartPtr> cgp.daughter(i)
            if _has_b_cont(abs(dau.pdgId())):
                no_b_dau = 0
                break
        return no_b_dau
    return 0


cdef list final_b_hadrons_vec(vector[GenParticle] * gen_particles):
    cdef GenPartPtr cgp
    cdef list res = []
    cdef size_t i

    for i in range(gen_particles.size()):
        cgp = &gen_particles.at(i)
        if _is_final_b(cgp):
            res.append(<object> tp.ObjectProxy_FromVoidPtr(
                <void *> cgp, 'reco::GenParticle'
            ))
    return res


def final_b_hadrons(gen_particles):
    cdef GenPartPtr cgp
    cdef list res = []

    if str(type(gen_particles)) == "<class 'DataFormats.FWLite.vector<reco::GenParticle>'>":
         return final_b_hadrons_vec(<vector[GenParticle] *> tp.ObjectProxy_AsVoidPtr(<PyObject *> gen_particles))

    for gp in gen_particles:
        if not type(gp) == genPartType:
             raise RuntimeError(
                 'get_all_daughters only accepts a list of GenParticle object')
        cgp = <GenPartPtr> tp.ObjectProxy_AsVoidPtr(<PyObject *> gp) 
        if _is_final_b(cgp):
            res.append(gp)
    return res



