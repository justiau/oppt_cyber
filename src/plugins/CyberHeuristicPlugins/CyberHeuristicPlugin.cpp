#ifndef _CYBER_HEURISTIC_PLUGIN_HPP_
#define _CYBER_HEURISTIC_PLUGIN_HPP_
#include "oppt/plugin/Plugin.hpp"
#include "../CyberUtils/CyberUtils.hpp"

namespace oppt
{
class CyberHeuristicPlugin: public HeuristicPlugin
{
public:
    CyberHeuristicPlugin():
        HeuristicPlugin() {
    }

    virtual ~CyberHeuristicPlugin() {

    }

    virtual bool load(const std::string& optionsFile) override {        
        optionsFile_ = optionsFile;
        return true;
    }

    virtual FloatType getHeuristicValue(const HeuristicInfo* heuristicInfo) const override {        
        return 1.0;
    }   

private:
    std::string optionsFile_;

};

OPPT_REGISTER_HEURISTIC_PLUGIN(CyberHeuristicPlugin)

}

#endif
