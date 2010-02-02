#include "TopoAnalysis.h"

typedef map<int,int>::value_type degreeValtype;
CTopoAnalysis::CTopoAnalysis(void)
{
}

CTopoAnalysis::~CTopoAnalysis(void)
{
}
void CTopoAnalysis::AnalysisDegree(CPlatTopoBase* base)
{	
	int num = 0;
	if (!(base->topoattribute))
	{
		base->topoattribute=new TopoAnalysisAttribute();
	}
	
	Node* iternode;
    iternode = base->Begin();
	while(iternode)                                                         //有点问题，End，begin的标记
	{
		NodeWeightVec_t nwv;
		iternode->Neighbors(nwv);
		num = nwv.size();
		if ( base->topoattribute->distribution.count(num) )
		{ 
			base->topoattribute->distribution[num]++;
		} 
		else
		{
			base->topoattribute->distribution.insert(degreeValtype(num,1));
		}
	    iternode = base->Next();
	} 	
}
