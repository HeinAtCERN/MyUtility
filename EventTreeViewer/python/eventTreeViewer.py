

import pypdt
from PyQt4 import QtGui
from DataFormats.FWLite import Events,Handle
app = QtGui.QApplication([])


def gp_eq(a, b):
    return (
        a.pdgId() == b.pdgId()
        and a.status() == b.status()
        and a.px() == b.px()
        and a.py() == b.py()
        and a.pz() == b.pz()
    )


class EventTreeViewer(QtGui.QTreeWidget):
    def __init__(self, parent=None):
        super(EventTreeViewer, self).__init__(parent)
        self.setColumnCount(1)
        self.setHeaderLabels(["pdg", "status", "e", "px", "py", "pz"])
        self.insertTopLevelItems(0, [QtGui.QTreeWidgetItem()])
        self.collection = "genParticles"

    def setEventTree(self, event, expand_key_func=None):
        self.clear()
        handle = Handle("vector<reco::GenParticle>")
        event.getByLabel(self.collection, handle)

        def fill_tree(p, parent_item):
            item = QtGui.QTreeWidgetItem(
                parent_item,
                [
                    "%d" % p.pdgId(),
                    "%d" % p.status(),
                    "%f" % p.energy(),
                    "%f" % p.px(),
                    "%f" % p.py(),
                    "%f" % p.pz(),
                ]
            )
            if not parent_item:
                self.addTopLevelItem(item)
            expanded = expand_key_func and expand_key_func(p)
            for d in xrange(p.numberOfDaughters()):
                d = p.daughter(d)
                expanded = fill_tree(d, item) or expanded
            if parent_item and expanded:
                parent_item.setExpanded(True)
            return expanded

        for gp in (p for p in handle.product() if not p.mother()):
            fill_tree(gp, None)



def event_iterator(filename, handles=None):
    """handles is a list of tuples: (varname, type, InputTag)"""
    events = Events(filename)
    if not handles: handles = []
    for evt in events.__iter__():
        for name,typ,inputtag in handles:
            handle = Handle(typ)
            evt.getByLabel(inputtag, handle)
            setattr(evt, "hndl_" + name, handle)
        yield evt


def open_viewer(filename, expand_key_func=None, collection_name=None):
    evtit = event_iterator(filename)
    w = EventTreeViewer()
    if collection_name: w.collection = collection_name
    def skipper():
        w.setEventTree(evtit.next(), expand_key_func)
    skipper()
    w.show()
    return skipper


fname_hein = "/afs/desy.de/user/t/tholenhe/xxl-af-cms/samples/Zbb_batch1/ZbbhadronicAODSIM1.root"


def expand_photons_25(particle):
    return particle.pdgId() == 22 and particle.et() > 25.


def expand_z_boson(particle):
    return particle.pdgId() == 23


def expand_final_b(gp):
    return pypdt.hasBottom(abs(gp.pdgId())) and not any(
                    pypdt.hasBottom(abs(gp.daughter(i).pdgId()))
                    for i in xrange(gp.numberOfDaughters())
            )