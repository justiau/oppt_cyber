#ifndef _CYBER_ACTION_SPACE_DISCRETE_HPP_
#define _CYBER_ACTION_SPACE_DISCRETE_HPP_

#include "oppt/robotHeaders/ActionSpaceDiscretizer.hpp"
#include "../CyberUtils/CyberUtils.hpp"

namespace oppt {

class CyberActionSpaceDiscretizer : public ActionSpaceDiscretizer {
public:
    CyberActionSpaceDiscretizer(ActionSpaceSharedPtr &actionSce):
        ActionSpaceDiscretizer(actionSpace) {
            LOGGING("Custom Control Host Action Space Discretizer");
    }

    virtual ~CyberActionSpaceDiscretizer(){}

    virtual std::vector<ActionSharedPtr> getAllActionsInOrder(const unsigned int& numStepsPerDimension) const {
        std::vector<ActionSharedPtr> allActionsOrdered_;
        long code = 0;
        for (int i=firstActionIndex_; i<= lastActionIndex_; ++i) {
            VectorFloat actionVec{i};
            ActionSharedPtr action(new DiscreteVectorAction(actionVec));
            action->as<DiscreteVectorAction>()->setBinNumber(code);
            code++;
            allActionsOrdered_.push_back(action);
        }
        return allActionsOrdered_;
    }

    void setActionIndices(int firstActionIndex, int lastActionIndex) {
        firstActionIndex_ = firstActionIndex;
        lastActionIndex_ = lastActionIndex;
    }

private:
    int firstActionIndex_;
    int lastActionIndex_;

};
}

#endif
