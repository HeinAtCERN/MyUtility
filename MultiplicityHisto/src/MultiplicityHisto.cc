// -*- C++ -*-
//
// Package:    MultiplicityHisto
// Class:      MultiplicityHisto
// 
/**\class MultiplicityHisto MultiplicityHisto.cc BTagDeltaR/MultiplicityHisto/src/MultiplicityHisto.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Heiner Josef Antonius Tholen,68/128,2997,
//         Created:  Thu Mar 20 10:18:31 CET 2014
// $Id$
//
//


// system include files
#include <memory>
#include <vector>
#include <string>
#include <TH1.h>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/Candidate/interface/Candidate.h"
//
// class declaration
//

class MultiplicityHisto : public edm::EDAnalyzer {
   public:
      explicit MultiplicityHisto(const edm::ParameterSet&);
      ~MultiplicityHisto();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


   private:
      virtual void beginJob() ;
      virtual void analyze(const edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;

      virtual void beginRun(edm::Run const&, edm::EventSetup const&);
      virtual void endRun(edm::Run const&, edm::EventSetup const&);
      virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);
      virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);

      // ----------member data ---------------------------
      edm::InputTag src_;
      edm::InputTag weights_;
      bool useWeights_;
      TH1D *histo_;
};

//
// constructors and destructor
//
MultiplicityHisto::MultiplicityHisto(const edm::ParameterSet& iConfig):
    src_(iConfig.getParameter<edm::InputTag>("src")),
    weights_(iConfig.getUntrackedParameter<edm::InputTag>(
        "weights",
        edm::InputTag()
    )),
    useWeights_(!(weights_ == edm::InputTag()))
{
    edm::Service<TFileService> fs;
    histo_ = fs->make<TH1D>(
        iConfig.getUntrackedParameter<std::string>("name", "histo").c_str(),
        iConfig.getUntrackedParameter<std::string>(
            "title",
            ";candidate multiplicity; number of events"
        ).c_str(),
        iConfig.getUntrackedParameter<int>("nbins", 26),
        - .5,
        iConfig.getUntrackedParameter<int>("nbins", 26) - .5
    );
}

MultiplicityHisto::~MultiplicityHisto()
{
}

// ------------ method called for each event  ------------
void
MultiplicityHisto::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    float weight = 1.;
    if (useWeights_ && !iEvent.isRealData()) {
        edm::Handle<double> weightHandle;
        iEvent.getByLabel(weights_, weightHandle);
        weight = *weightHandle.product();
        if (isnan(weight)) {
            weight = 1.;
        }
    }

    edm::Handle<std::vector<reco::Candidate> > candidateHandle;
    iEvent.getByLabel(src_, candidateHandle);

    histo_->Fill(candidateHandle->size(), weight);
}


// ------------ method called once each job just before starting event loop  ------------
void 
MultiplicityHisto::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
MultiplicityHisto::endJob() 
{
}

// ------------ method called when starting to processes a run  ------------
void 
MultiplicityHisto::beginRun(edm::Run const&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a run  ------------
void 
MultiplicityHisto::endRun(edm::Run const&, edm::EventSetup const&)
{
}

// ------------ method called when starting to processes a luminosity block  ------------
void 
MultiplicityHisto::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a luminosity block  ------------
void 
MultiplicityHisto::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
MultiplicityHisto::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(MultiplicityHisto);
