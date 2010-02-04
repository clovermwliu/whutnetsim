#include "StdTS.h"


CStdTS::CStdTS()
:CHiberTopoBase()
/*
描述：层次拓扑的派生类
参数：无
返回值：无
*/
{
}

CStdTS::~CStdTS(void)
{
}
bool CStdTS::GenerateTopo()
/*
描述：生成拓扑
参数：无
返回值：是否生成成功
备注：是基类中纯虚函数的派生类实现
*/
{
	/*CTopoFile* file = new CTopoFile(configFile);
	file->ReadOneLayerInfo(1,transitTopoVec);
	file->ReadOneLayerInfo(2,stubTopoVec);
	file->ReadOneLayerInfo(3,lanTopoVec);
	file->ReadAllConnectInfo(this);
	delete file;*/
	return true;
}
void CStdTS::SetLocationViaBoundBox(const Location& BoundBoxLeftDown, 
									const Location& BoundBoxRightUpper,
									BoxType  type)
/*
描述：通过绑定位置来给节点设置坐标
参数：[in]BoundBoxLeftDown   ——左下角的位置
      [in]BoundBoxRightUpper ——右上角的位置
	  [in]type               ——设置位置的类型
返回值：无
*/
{
	Count_t LoneNum = GetLay1Num();
	Count_t LtwoNum = GetLay2Num();
	Count_t LthreeNum = GetLay3Num();
	Meters_t  xBound = BoundBoxRightUpper.X() - BoundBoxLeftDown.X();
	Meters_t  yBound = BoundBoxRightUpper.Y() - BoundBoxLeftDown.Y();

	Meters_t  yPase = yBound/4;
    Location  baseLow =BoundBoxLeftDown;
	Location  baseLowcopy = baseLow;

	//设置最下面一层的拓扑的坐标
	for (int l3 = 0;l3 < LthreeNum;l3++)
	{
		Meters_t xPase = xBound/LthreeNum;
		Location low(baseLow.X()+l3*xPase,baseLow.Y());
		Location high(baseLow.X()+(l3+1)*xPase,baseLow.Y()+yPase);
		//lanTopoVec[l3]->SetLocationViaBoundBox(low,high);
		lanTopoVec[l3]->StarSetLocationViaBoundBox(low,high,-M_PI/4,-M_PI/2);
	}
	baseLow.Y(baseLowcopy.Y()+yPase*3/2);
	baseLowcopy = baseLow;

	//设置中间一层的坐标
	for (int l2 = 0;l2 < LtwoNum;l2++)
	{
		Meters_t xPase = xBound/LtwoNum;
		Location low(baseLow.X()+l2*xPase,baseLow.Y());
		Location high(baseLow.X()+(l2+1)*xPase,baseLow.Y()+(l2+1)*yPase);
		stubTopoVec[l2]->SetLocationViaBoundBox(low,high);
	}
	baseLow.Y(baseLowcopy.Y()+yPase*3/2);
	baseLowcopy = baseLow;

	//设置最上面一层的坐标
	for (int l1 = 0;l1 < LoneNum;l1++)
	{
		Meters_t xPase = xBound/LoneNum;
		Location low(baseLow.X()+l1*xPase,baseLow.Y());
		Location high(baseLow.X()+(l1+1)*xPase,baseLow.Y()+(l1+1)*yPase);
		transitTopoVec[l1]->SetLocationViaBoundBox(low,high);
	}
}