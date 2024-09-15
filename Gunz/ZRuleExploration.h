#ifndef _ZRULE_EXPLORATION_H
#define _ZRULE_EXPLORATION_H


#include "ZRule.h"
#include "ZRuleDeathMatch.h"


class ZRuleExploration : public ZRuleSoloDeathMatch
{
public:
	ZRuleExploration(ZMatch* pMatch);
	virtual ~ZRuleExploration();
};





#endif