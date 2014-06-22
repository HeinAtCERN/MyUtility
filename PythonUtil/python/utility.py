import FWCore.ParameterSet.Config as cms


def make_histo_analyzer(
        src,
        plot_quantity,
        n_bins,
        low=0.,
        high=0.,
        x_label="",
        y_label="number of candidates",
        weights=""
):
    """tokens: (low, high, n_bins, x_label, y_label)"""
    if not x_label:
        x_label = plot_quantity
    histo_analyzer = cms.EDAnalyzer(
        "CandViewHistoAnalyzer",
        src=cms.InputTag(src),
        histograms=cms.VPSet(
            cms.PSet(
                lazyParsing=cms.untracked.bool(True),
                min=cms.untracked.double(low),
                max=cms.untracked.double(high),
                nbins=cms.untracked.int32(n_bins),
                name=cms.untracked.string("histo"),
                description=cms.untracked.string(
                    ";" + x_label + ";" + y_label
                ),
                plotquantity=cms.untracked.string(plot_quantity),
            )
        )
    )
    if weights:
        histo_analyzer.histograms[0].weights = cms.untracked.InputTag(weights)
    return histo_analyzer
