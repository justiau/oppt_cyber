#ifndef _CYBER_HEURISTIC_PLUGIN_HPP_
#define _CYBER_HEURISTIC_PLUGIN_HPP_
#include "oppt/plugin/Plugin.hpp"
#include "../CyberUtils/CyberUtils.hpp"
#include "../CyberUtils/Scenario.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

namespace oppt
{
class CyberES2HeuristicPlugin: public HeuristicPlugin
{
public:
    CyberES2HeuristicPlugin():
        HeuristicPlugin() {
    }

    virtual ~CyberES2HeuristicPlugin() {

    }

    virtual bool load(const std::string& optionsFile) override {        
        parseOptions_<CyberOptions>(optionsFile);
        CyberOptions* generalOptions = static_cast<CyberOptions*>(options_.get());
        maxReward = generalOptions->maxReward;
        scenario = generalOptions->getScenario();
        return true;
    }

    virtual FloatType getHeuristicValue(const HeuristicInfo* heuristicInfo) const override {
        // state table
        // 0 - httpd_attacked
        // 1 - httpd_hacked
        // 2 - ftpd_attacked
        // 3 - ftpd_hacked
        // 4 - webserver_sniffer_installed
        // 5 - workstation_hacked
        // 6 - workstation_data_stolen
        // httpd_attacked -> httpd_hacked -> webserver_sniffer_installed -> workstation_hacked -> workstation_data_stolen
        // ftpd_attacked -> ftpd_hacked -> webserver_sniffer_installed -> workstation_hacked -> workstation_data_stolen
        VectorFloat currentState = heuristicInfo->currentState->as<VectorState>()->asVector();
        scenario->setOpptState(currentState);
        std::vector<SVar> stateVars = scenario->getStateVars();
        int position = 0;
        for(size_t i=0; i<stateVars.size(); ++i) {
            SVar var = stateVars[i];
            Assignment a = var.createAssignment("true");
            if (scenario->isAssignTrue(a)) {
                position = std::max(position, stateTable.at(var.name_));
            }
        }
        int distance = stateTable.size() - 1 - position;
        FloatType currentDiscount = std::pow(heuristicInfo->discountFactor, heuristicInfo->currentStep);
        currentDiscount *= std::pow(heuristicInfo->discountFactor, distance);
        return currentDiscount * maxReward;
    }   

private:
    Scenario* scenario;
    FloatType maxReward;
    std::unordered_map<std::string, int> stateTable {
        {"httpd_attacked",1},
        {"httpd_hacked",2},
        {"ftpd_attacked",1},
        {"ftpd_hacked",2},
        {"webserver_sniffer_installed",3},
        {"workstation_hacked",4},
        {"workstation_data_stolen",5}
    };

};

OPPT_REGISTER_HEURISTIC_PLUGIN(CyberES2HeuristicPlugin)

}

#endif
