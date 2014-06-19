// -*- C++ -*-
//
// Package:    PlainSVProducer
// Class:      PlainSVProducer
// 
/**\class PlainSVProducer PlainSVProducer.cc MyUtility/PlainSVProducer/src/PlainSVProducer.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Heiner Josef Antonius Tholen,68/128,2997,
//         Created:  Thu Jun 19 17:19:48 CEST 2014
// $Id$
//
//


// system include files
#include <memory>
#include <vector>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/VertexReco/interface/Vertex.h"
#include "RecoBTag/SecondaryVertex/interface/SecondaryVertex.h"

//
// class declaration
//

class PlainSVProducer : public edm::EDProducer {
   public:
      explicit PlainSVProducer(const edm::ParameterSet&);
      ~PlainSVProducer();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

   private:
      virtual void beginJob() ;
      virtual void produce(edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;
      
      virtual void beginRun(edm::Run&, edm::EventSetup const&);
      virtual void endRun(edm::Run&, edm::EventSetup const&);
      virtual void beginLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&);
      virtual void endLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&);

      // ----------member data ---------------------------
      edm::InputTag pv_src_;
      edm::InputTag sv_src_;
};

//
// constructors and destructor
//
PlainSVProducer::PlainSVProducer(const edm::ParameterSet& iConfig):
    pv_src_(iConfig.getParameter<edm::InputTag>("pv_src")),
    sv_src_(iConfig.getParameter<edm::InputTag>("sv_src"))
{
    produces<std::vector<reco::SecondaryVertex> >();
}


PlainSVProducer::~PlainSVProducer()
{
}


//
// member functions
//

// ------------ method called to produce the data  ------------
void
PlainSVProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    edm::Handle<std::vector<reco::Vertex> > sv_src;
    iEvent.getByLabel(sv_src_, sv_src);

    edm::Handle<std::vector<reco::Vertex> > pv_src;
    iEvent.getByLabel(pv_src_, pv_src);
    const reco::Vertex &pv = pv_src->at(0);

    std::vector<reco::SecondaryVertex>* out = new std::vector<reco::SecondaryVertex>();
    for (unsigned i=0; i<sv_src->size(); ++i) {
        const reco::Vertex sv = sv_src->at(i);
    	GlobalVector fd = GlobalVector(
    	    sv.x() - pv.x(),
            sv.y() - pv.y(),
            sv.z() - pv.z()
        );
        out->push_back(reco::SecondaryVertex(pv, sv, fd, true));
    }

    std::auto_ptr<std::vector<reco::SecondaryVertex> > pOut(out);
    iEvent.put(pOut);
}

// ------------ method called once each job just before starting event loop  ------------
void 
PlainSVProducer::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
PlainSVProducer::endJob() {
}

// ------------ method called when starting to processes a run  ------------
void 
PlainSVProducer::beginRun(edm::Run&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a run  ------------
void 
PlainSVProducer::endRun(edm::Run&, edm::EventSetup const&)
{
}

// ------------ method called when starting to processes a luminosity block  ------------
void 
PlainSVProducer::beginLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a luminosity block  ------------
void 
PlainSVProducer::endLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&)
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
PlainSVProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(PlainSVProducer);
