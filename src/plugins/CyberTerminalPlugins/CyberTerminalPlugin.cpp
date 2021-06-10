#include "oppt/plugin/Plugin.hpp"
#include "../CyberUtils/CyberUtils.hpp"

namespace oppt
{
class CyberTerminalPlugin: public TerminalPlugin
{
public:
    CyberTerminalPlugin():
        TerminalPlugin() {
    }

    virtual ~CyberTerminalPlugin() = default;

    virtual bool load(const std::string& optionsFile) override {
        parseOptions_<CyberOptions>(optionsFile);
        CyberOptions* generalOptions = static_cast<CyberOptions*>(options_.get());
        return true;
    }

    virtual ValidityReportSharedPtr isValid(const PropagationResultSharedPtr& propagationResult) override {
        ValidityReportSharedPtr vr(new ValidityReport(propagationResult->nextState));        
        VectorFloat stateVec = propagationResult->nextState->as<VectorState>()->asVector();
        vr->isValid = true;
        return vr;
    }

    virtual bool isTerminal(const PropagationResultSharedPtr& propagationResult) override {
        if (!isValid(propagationResult)->isValid) {
            return true;
        }
        VectorFloat stateVec = propagationResult->nextState->as<VectorState>()->asVector();
        if (stateVec[0] == 3.0) {
            return true;
        }
        return false;
    }
    
};

OPPT_REGISTER_TERMINAL_PLUGIN(CyberTerminalPlugin)
}
