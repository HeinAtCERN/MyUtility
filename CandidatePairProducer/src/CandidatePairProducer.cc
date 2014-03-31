// -*- C++ -*-
//
// Package:    CandidatePairProducer
// Class:      CandidatePairProducer
// 
/**\class CandidatePairProducer CandidatePairProducer.cc BTagDeltaR/CandidatePairProducer/src/CandidatePairProducer.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Heiner Josef Antonius Tholen,68/128,2997,
//         Created:  Thu Mar 20 11:02:02 CET 2014
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
#include "DataFormats/Common/interface/Ptr.h"
#include "DataFormats/Candidate/interface/CompositePtrCandidate.h"
#include "CommonTools/CandUtils/interface/AddFourMomenta.h"

//
// class declaration
//

class CandidatePairProducer : public edm::EDProducer {
   public:
      explicit CandidatePairProducer(const edm::ParameterSet&);
      ~CandidatePairProducer();

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
      edm::InputTag src_;
      AddFourMomenta adder_;
};

//
// constructors and destructor
//
CandidatePairProducer::CandidatePairProducer(const edm::ParameterSet& iConfig):
    src_(iConfig.getParameter<edm::InputTag>("src")),
    adder_(AddFourMomenta())
{
    produces<std::vector<reco::CompositePtrCandidate> >();
}


CandidatePairProducer::~CandidatePairProducer()
{
}


//
// member functions
//

// ------------ method called to produce the data  ------------
void
CandidatePairProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    edm::Handle<reco::CandidateView> src;
    iEvent.getByLabel(src_, src);

    std::vector<reco::CompositePtrCandidate>* out = new std::vector<reco::CompositePtrCandidate>();
    if (src->size() > 1) {
        for (unsigned i=0; i<src->size()-1; ++i) {
            for (unsigned j=i+1; j<src->size(); ++j) {
                out->push_back(reco::CompositePtrCandidate());
                out->back().addDaughter(reco::CandidatePtr(src, i));
                out->back().addDaughter(reco::CandidatePtr(src, j));
                adder_.set(out->back());
            }
        }
    }

    std::auto_ptr<std::vector<reco::CompositePtrCandidate> > pOut(out);
    iEvent.put(pOut);
}

// ------------ method called once each job just before starting event loop  ------------
void 
CandidatePairProducer::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
CandidatePairProducer::endJob() {
}

// ------------ method called when starting to processes a run  ------------
void 
CandidatePairProducer::beginRun(edm::Run&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a run  ------------
void 
CandidatePairProducer::endRun(edm::Run&, edm::EventSetup const&)
{
}

// ------------ method called when starting to processes a luminosity block  ------------
void 
CandidatePairProducer::beginLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a luminosity block  ------------
void 
CandidatePairProducer::endLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&)
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
CandidatePairProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(CandidatePairProducer);
