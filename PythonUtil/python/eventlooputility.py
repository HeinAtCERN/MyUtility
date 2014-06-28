import itertools
import numpy
import ROOT
ROOT.gSystem.Load('libGenVector')
ROOT.gROOT.ProcessLine('gErrorIgnoreLevel = kError;')
DeltaR = ROOT.Math.VectorUtil.DeltaR
Pi = ROOT.Math.Pi()


def deltaR_vec_to_vec(a, b):
    d_eta = a.eta()-b.eta()
    d_phi = abs(float(a.phi())-float(b.phi()))
    if d_phi > Pi:
         d_phi -= 2.*Pi 
    return (d_eta**2 + d_phi**2)**.5
deltaR_cand_to_cand = lambda a, b: DeltaR(a.p4(), b.p4())
deltaR_vec_to_cand = lambda a, b: deltaR_vec_to_vec(a, b.p4())


def mkrtvec(numpy_vec):
    """
    numpy array => ROOT.Math.XYZVector

    >>> a = ROOT.Math.XYZVector(1.,2.,3.)
    >>> b = mkrtvec(mkvec(a))
    >>> a.x() == b.x()
    True
    >>> a.y() == b.y()
    True
    >>> a.z() == b.z()
    True
    """
    return ROOT.Math.XYZVector(
        *numpy_vec[:3]
    )


def mkvec(root_vec):
    """
    ROOT.Math.XYZVector => numpy array

    >>> a = numpy.array([1., 2., 3.])
    >>> b = mkvec(mkrtvec(a))
    >>> a[0] == b[0]
    True
    >>> a[1] == b[1]
    True
    >>> a[2] == b[2]
    True
    """
    return numpy.array([
        root_vec.x(),
        root_vec.y(),
        root_vec.z()
    ])


def matching(reco_objs, gen_objs, keyfunc, cutvalue):
    combos = itertools.product(reco_objs, gen_objs)             # all combos
    combos = ((ro, go, keyfunc(ro, go)) for ro, go in combos)   # add dR
    combos = list(c for c in combos if c[2] < cutvalue)         # select < 0.1
    matched_combos = []
    while combos:
        match = min(combos, key=lambda c: c[2])     # select by lowest dR
        combos = filter(                            # remove all connected
            lambda c: c[0] != match[0] and c[1] != match[1],
            combos
        )
        matched_combos.append(match)
    return matched_combos


def covariance_significance(secondary_vertex, gen_particle):

    # get the difference vector of vertices
    gsv = mkvec(gen_particle.daughter(0).vertex())  # gen secondary vertex
    rsv = mkvec(secondary_vertex.position())        # reco secondary vertex
    vec = gsv - rsv                                 # difference

    # calculate significance^2
    cov = secondary_vertex.covariance()
    assert(cov.Invert())
    np_cov = numpy.matrix(
        tuple(tuple(cov(i, j)
        for i in range(3))
        for j in range(3))
    )
    cov.Invert()
    res = numpy.dot(numpy.dot(vec, np_cov), vec) # sig^2 = vec * inv_cov * vec
    res = res[0, 0]
    if res is numpy.nan or res is numpy.inf or res < 0.:
        res = 9999999.
    else:
        res = numpy.sqrt(res)
    return res


if __name__ == "__main__":
    import doctest
    doctest.testmod()

