"""
eventTreeViewer.py

Usage:
edmGenParticleViewer

# in ipython shell:
next_event = open_viewer(
    'eventfile.root',               # filename
    'prunedGenParticles',           # collection name
    lambda gp: gp.pdgId() == 25     # optional: expand function (see below)
)
next_event()                        # this skips to the next event

The third argument to open_viewer(...) is optional. If given it needs to be a
function that accepts a genParticle instance and return True or False. If True
is returned for a given particle, the tree view will expand all particles up to
the given particle. These examples are defined in this scope:

def expand_photons_25(particle):
    return particle.pdgId() == 22 and particle.et() > 25.

def expand_z_boson(particle):
    return particle.pdgId() == 23

def expand_final_b(gp):
    import pypdt
    return (
        pypdt.hasBottom(abs(gp.pdgId())) and
        not any(
            pypdt.hasBottom(abs(gp.daughter(i).pdgId()))
            for i in xrange(gp.numberOfDaughters())
        )
    )
"""

from PyQt4 import QtGui
from DataFormats.FWLite import Events, Handle
app = QtGui.QApplication([])


class EventTreeViewer(QtGui.QTreeWidget):
    def __init__(self, parent=None):
        super(EventTreeViewer, self).__init__(parent)
        self.setColumnCount(1)
        self.setHeaderLabels(
            ["pdg", "status", "e", "px", "py", "pz", "vx", "vy", "vz"]
        )
        self.insertTopLevelItems(0, [QtGui.QTreeWidgetItem()])
        self.header().resizeSection(0, 400)
        self.setSelectionMode(3)
        self.itemClicked.connect(self.multiselectItems)
        self.collection = None
        self.particle_map = {}    # repr(genPraticle) -> list of tree items
        self.item_map = {}        # tree items -> list of tree items

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
                    "%f" % p.vx(),
                    "%f" % p.vy(),
                    "%f" % p.vz(),
                ]
            )
            self.add_item_to_particle_map(p, item)
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
            break  # from one proton is enough

        self.make_item_map()

    def add_item_to_particle_map(self, p, i):
        repr_p = repr(p)
        if repr_p in self.particle_map:
            self.particle_map[repr_p].append(i)
        else:
            self.particle_map[repr_p] = [i]

    def make_item_map(self):
        self.item_map.clear()
        for item_list in self.particle_map.itervalues():
            for item in item_list:
                self.item_map[item] = item_list
        self.particle_map.clear()

    def multiselectItems(self, item, column):
        items = self.item_map[item]
        for i in items:
            i.setSelected(True)


def event_iterator(filename, handles=None):
    """handles is a list of tuples: (varname, type, InputTag)"""
    events = Events(filename)
    if not handles:
        handles = []
    for evt in events.__iter__():
        for name, typ, inputtag in handles:
            handle = Handle(typ)
            evt.getByLabel(inputtag, handle)
            setattr(evt, "hndl_" + name, handle)
        yield evt


def open_viewer(filename, collection_name, expand_key_func=None):
    evtit = event_iterator(filename)
    w = EventTreeViewer()
    if collection_name:
        w.collection = collection_name

    def skipper():
        w.setEventTree(evtit.next(), expand_key_func)
    skipper()
    w.show()
    w.showMaximized()
    return skipper


def expand_photons_25(particle):
    return particle.pdgId() == 22 and particle.et() > 25.


def expand_z_boson(particle):
    return particle.pdgId() == 23


def expand_final_b(gp):
    import pypdt
    return (
        pypdt.hasBottom(abs(gp.pdgId())) and
        not any(
            pypdt.hasBottom(abs(gp.daughter(i).pdgId()))
            for i in xrange(gp.numberOfDaughters())
        )
    )


if __name__ == "__main__":
    print __doc__


