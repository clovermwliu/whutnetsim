#include "StdAfx.h"
#include "EngMatlab.h"
#include <iostream>

CEngMatlab::~CEngMatlab(void)
{
	CloseEngine();
}

bool
CEngMatlab::OpenEngine()
{
	if( !(ep = engOpen("\0")))
	{
		SetError(OPEN_FAILURE);
		return false;
	}
	return true;
}

bool
CEngMatlab::CloseEngine()
{
	if( engClose(ep))//成功退出返回0
	{
		SetError(CLOSE_FAILURE);
		return false;
	}
	return true;
}

bool
CEngMatlab::SendCommand(const char *string)
/*
描述：执行发送给matlab的命令
参数：[IN]string：让matlab执行的命令的字符串
返回值：成功返回true
		不成功返回false
*/
{
	if(engEvalString(ep, string))//0为成功
	{
		SetError(COMMAND_SEND_FAILURE);
		return false;
	}
	return true;
}

int
CEngMatlab::Transpose(mxArray **ppMatrix)
/*
描述：重新生成转置矩阵,将原矩阵销毁
参数：[IN][OUT]ppMatrix:指向待转置矩阵的地址;指向转置后矩阵的新地址
返回值：成功返回1
WRITE_VARIABLE_FAILURE出错，返回0
COMMAND_SEND_FAILURE出错，返回-1
备注：注意函数结束后原矩阵的地址和新矩阵的地址不同，原矩阵自动销毁
*/

{
	if (engPutVariable(ep, "matrix", *ppMatrix))
	{
		SetError(WRITE_VARIABLE_FAILURE);
		return 0;
	}
	if (engEvalString(ep, "matrix = matrix.'"))
	{
		SetError(COMMAND_SEND_FAILURE);
		return -1;
	}
	DestoryMxPointer(*ppMatrix);//销毁原矩阵
	*ppMatrix = engGetVariable(ep, "matrix");
	return 1;
}

int 
CEngMatlab::Transpose(const mxArray *pSrcMatrix, 
					  mxArray **ppNewMatrix)
/*
描述：将pSrcMatrix指向的矩阵转置，ppNewMatrix指向新矩阵的地址
参数：[IN]pSrcMatrix:指向待转置的矩阵的地址
[IN][OUT]ppNewMatrix:指向转置后矩阵指针的地址
返回值：成功返回1
WRITE_VARIABLE_FAILURE出错，返回0
COMMAND_SEND_FAILURE出错，返回-1
备注：原矩阵未被销毁
*/
{
	if (engPutVariable(ep, "matrix", pSrcMatrix))
	{
		SetError(WRITE_VARIABLE_FAILURE);
		return 0;
	}
	if (engEvalString(ep, "matrix = matrix.'"))
	{
		SetError(COMMAND_SEND_FAILURE);
		return -1;
	}
	*ppNewMatrix = engGetVariable(ep, "matrix");
	return 1;
}

void
CEngMatlab::DestoryMxPointer(mxArray *pArray)
/*
描述：销毁mxArray指针
参数：[IN]pArray:待销毁的mxArray指针
返回值：无
*/
{
	mxDestroyArray(pArray);
}


int
CEngMatlab::GetBufferToArray(void *pSrc,
							 mxArray **ppDst,
							 const unsigned long DataType,
							 int RowCount,
							 int ColCount)
/*
描述：将C中的数组转为matlab中的数组
参数：[IN]pSrc:指向C中数组地址
      [IN][OUT]ppDst:指向转换后的matlab中的数组的地址
	  [IN]DataType:待转换的数组的类型
	  [IN]RowCount:C中数组的行数
	  [IN]ColCount:C中数组的列数
返回值：成功返回1
        要转换的数据类型不支持则返回0
        创建matlab mxArray失败返回-1
备注：
eg.
------------       --------
|1 2 3 4   |       |1 5 9 |   转置  ------------
|5 6 7 8   | --->  |2 6 10|   --->  |1 2 3 4   |
|9 10 11 12|       |3 7 11|         |5 6 7 8   |
------------       |4 8 12|         |9 10 11 12|
                   --------         ------------ 
double z[3][4]= {{1,2,3,4},{5,6,7,8},{9,10,11,12}};
转化为matlab中的3行4列，
应为
1 2 3 4
5 6 7 8
9 10 11 12 ---》结果1
若*ppDst = mxCreateDoubleMatrix(RowCount, ColCount, mxREAL);，
则存储到matlab中为1 4 7 10
2 5 8 11
3 6 9 12 ----》结果2
所以要想得到结果1，必须	*ppDst = mxCreateDoubleMatrix(ColCount, RowCount, mxREAL);
此时存储到matlab中为
1 5 9
2 6 10
3 7 11
4 8 12---》结果3
结果3为结果1的转置矩阵
所以最后调用转置函数
MATLAB中的存储结果为
1 2 3 4
5 6 7 8
9 10 11 12 ---》结果4
即和z相等
*/

{
	size_t data_size;
	switch(DataType)
	{
	case DATA_INT: 
		{
			data_size = sizeof(int);
			*ppDst = mxCreateNumericMatrix(ColCount, RowCount, mxINT32_CLASS, mxREAL);//ColCount行RowCount列
		}
		break;
	case DATA_DOUBLE: 
		{
			data_size = sizeof(double);
			*ppDst = mxCreateDoubleMatrix(ColCount, RowCount, mxREAL);//ColCount行RowCount列
		}
		break;
	case DATA_UNSIGNED_INT:
		{
			data_size = sizeof(double);
			*ppDst = mxCreateNumericMatrix(ColCount, RowCount, mxUINT32_CLASS, mxREAL);//ColCount行RowCount列
		}
	default:
		{
			SetError(DATA_TYPE_NO_SUPPORT);
			return 0;
		}
	}
	if(*ppDst == NULL)
	{
		SetError(ALLOC_MATARRAY_FAILURE);
		return -1;
	}

	memcpy((void *)mxGetPr(*ppDst), pSrc, (RowCount*ColCount)*data_size);
	Transpose(ppDst);//转置
	return 1;
}

int
CEngMatlab::GetBufferToArray(void **ppSrc,
							 mxArray **ppDst,
							 const unsigned long DataType,
							 int RowCount,
							 int ColCount)
/*
描述：//将C中的二维数组转为matlab中的数组
参数：[IN]ppSrc:指向二维数组的指针
      [IN][OUT]ppDst:指向转换后的matlab中的数组的地址
	  [IN]DataType:待转换的数组的类型
	  [IN]RowCount:C中数组的行数
	  [IN]ColCount:C中数组的列数
返回值：成功返回1
        要转换的数据类型不支持则返回0
        创建matlab mxArray失败返回-1
备注：
eg.
------------       --------
|1 2 3 4   |       |1 5 9 |   转置  ------------
|5 6 7 8   | --->  |2 6 10|   --->  |1 2 3 4   |
|9 10 11 12|       |3 7 11|         |5 6 7 8   |
------------       |4 8 12|         |9 10 11 12|
                   --------         ------------ 

int **ppSrc = new int*[3];
int count = 1;
for(int i = 0; i < 3; ++i)
{
	ppSrc[i] = new int[4];
	for(int j = 0; j < 4; ++j)
	{
		*(*(ppSrc+i)+j) = count++;
	}
}	
mxArray *xx = NULL;
GetBufferToArray((void **)ppSrc, &xx, DATA_INT, 3, 4);
*/

{
	size_t data_size;
	switch(DataType)
	{
	case DATA_INT: 
		{
			data_size = sizeof(int);
			*ppDst = mxCreateNumericMatrix(ColCount, RowCount, mxINT32_CLASS, mxREAL);//ColCount行RowCount列
			if(*ppDst == NULL)
			{
				SetError(ALLOC_MATARRAY_FAILURE);
				return -1;
			}
			for(int i = 0; i < RowCount; ++i)
			{
				memcpy((int *)mxGetPr(*ppDst)+i*ColCount, (int*)ppSrc[i], ColCount*data_size);

			}
		}
		break;
	case DATA_DOUBLE: 
		{
			data_size = sizeof(double);
			*ppDst =  mxCreateDoubleMatrix(ColCount, RowCount, mxREAL);//ColCount行RowCount列
			if(*ppDst == NULL)
			{
				SetError(ALLOC_MATARRAY_FAILURE);
				return -1;
			}
			for(int i = 0; i < RowCount; ++i)
			{
				memcpy((double *)mxGetPr(*ppDst)+i*ColCount, (double*)ppSrc[i], ColCount*data_size);

			}

		}
		break;
	case DATA_UNSIGNED_INT:
		{
			data_size = sizeof(double);
			*ppDst = mxCreateNumericMatrix(ColCount, RowCount, mxUINT32_CLASS, mxREAL);//ColCount行RowCount列
			if(*ppDst == NULL)
			{
				SetError(ALLOC_MATARRAY_FAILURE);
				return -1;
			}
			for(int i = 0; i < RowCount; ++i)
			{
				memcpy((size_t *)mxGetPr(*ppDst)+i*ColCount, (size_t *)ppSrc[i], ColCount*data_size);
			}

		}
	default:
		{
			SetError(DATA_TYPE_NO_SUPPORT);
			return 0;
		}
	}

	Transpose(ppDst);//转置
	return 1;
}

int
CEngMatlab::GetArrayToBuffer(mxArray *pSrc,//源
							 double ***pppDst)//目的
/*
描述：将matlab中的数组转为double型的二维数组
参数：[IN]pSrc:指向matlab中数组的地址
	  [IN][OUT]pppDst:指向转换后的二维数组的指针的地址
返回值：成功返回1
		数据为空返回-1
		数据类型不匹配返回0
备注：
eg.
------------       --------
|1 2 3 4   | 转置  |1 5 9 |         ------------
|5 6 7 8   | --->  |2 6 10|   --->  |1 2 3 4   |
|9 10 11 12|       |3 7 11|         |5 6 7 8   |
------------       |4 8 12|         |9 10 11 12|
                   --------         ------------
*/
{	
	if (mxIsEmpty(pSrc))
	{
		SetError(DATA_EMPTY);
		return -1;
	}
	Transpose(&pSrc);//转置
	int RowCount = (int)mxGetM(pSrc);
	int ColCount = (int)mxGetN(pSrc);
	if (RowCount == 1 ||ColCount == 1)
	{
		SetError(DATA_TYPE_NO_MATCH);
		return 0;
	}
	
	size_t data_size = sizeof(double);
	//动态开数组,ColCount行，RowCount列//内存空间不连续
	*pppDst = new double *[ColCount];
	for (int i = 0; i < ColCount; ++i)
	{
		(*pppDst)[i] = new double[RowCount];
		memcpy((*pppDst)[i], mxGetPr(pSrc)+i*RowCount, RowCount*data_size);
	}
	return 1;
}

int
CEngMatlab::GetArrayToBuffer(mxArray *pSrc,//源
							 int ***pppDst)//目的
/*
描述：将matlab中的数组转为int型的二维数组
参数：[IN]pSrc:指向matlab中数组的地址
	  [IN][OUT]pppDst:指向转换后的二维数组的指针的地址
返回值：成功返回1
数据为空返回-1
数据类型不匹配返回0
备注：
eg.
------------       --------
|1 2 3 4   | 转置  |1 5 9 |         ------------
|5 6 7 8   | --->  |2 6 10|   --->  |1 2 3 4   |
|9 10 11 12|       |3 7 11|         |5 6 7 8   |
------------       |4 8 12|         |9 10 11 12|
--------         ------------
*/
{	
	if (mxIsEmpty(pSrc))
	{
		SetError(DATA_EMPTY);
		return -1;
	}
	Transpose(&pSrc);//转置
	int RowCount = (int)mxGetM(pSrc);
	int ColCount = (int)mxGetN(pSrc);
	if (RowCount == 1 ||ColCount == 1)
	{
		SetError(DATA_TYPE_NO_MATCH);
		return 0;
	}
	size_t data_size = sizeof(int);
	//动态开数组,ColCount行，RowCount列//内存空间不连续
	*pppDst = new int *[ColCount];
	for (int i = 0; i < ColCount; ++i)
	{
		(*pppDst)[i] = new int[RowCount];
		memcpy((*pppDst)[i], (int*)mxGetPr(pSrc)+i*RowCount, RowCount*data_size);
	}
	return 1;
}

int 
CEngMatlab::GetArrayToDouble(mxArray *pSrc,
							 double *pDst)
/*
描述：把matlab中的单个数据转为C中的double数据
返回值：成功返回1
数据为空返回-1
数据类型不匹配返回0
*/
{
	if (mxIsEmpty(pSrc))
	{
		SetError(DATA_EMPTY);
		return -1;
	}
	if (!mxIsDouble(pSrc))
	{
		SetError(DATA_TYPE_NO_SUPPORT);
		return -0;
	}
	size_t data_size = sizeof(double);
	//pDst = new double[1];
	memcpy(pDst, (void *)mxGetPr(pSrc), data_size);
	return 1;
}

int
CEngMatlab::GetArrayToBuffer(mxArray *pSrc,//源
							 double **ppDst)//目的
/*
描述：将matlab中的数组转为double型的一维数组
参数：[IN]pSrc:指向matlab中数组的地址
      [IN][OUT]ppDst:指向转换后的一维数组的指针的地址
返回值：成功返回1
		数据为空返回-1
		数据类型不匹配返回0
备注：
eg.
----------       ----------
|1 2 3 4 |  ---> |1 2 3 4 |
----------       -----------
*/
{	
	if (mxIsEmpty(pSrc))
	{
		SetError(DATA_EMPTY);
		return -1;
	}
	int RowCount = (int)mxGetM(pSrc);
	int ColCount = (int)mxGetN(pSrc);
	if(RowCount == 1)
		RowCount = ColCount;
	else if(ColCount != 1)
	{
		SetError(DATA_TYPE_NO_MATCH);
		return 0;
	}
	size_t data_size = sizeof(double);
	*ppDst = new double[RowCount];
	memcpy(*ppDst, (void *)mxGetPr(pSrc), RowCount*data_size);
	return 1;
}

int
CEngMatlab::GetArrayToBuffer(mxArray *pSrc,//源
							 int **ppDst)//目的
/*
描述：将matlab中的数组转为int型的一维数组
参数：[IN]pSrc:指向matlab中数组的地址
      [IN][OUT]ppDst:指向转换后的一维数组的指针的地址
返回值：成功返回1
		数据为空返回-1
		数据类型不匹配返回0
备注：
eg.
----------       ----------
|1 2 3 4 |  ---> |1 2 3 4 |
----------       -----------
*/
{	
	if (mxIsEmpty(pSrc))
	{
		SetError(DATA_EMPTY);
		return -1;
	}
	int RowCount = (int)mxGetM(pSrc);
	int ColCount = (int)mxGetN(pSrc);
	if(RowCount == 1)
		RowCount = ColCount;
	else if(ColCount != 1)
	{
		SetError(DATA_TYPE_NO_MATCH);
		return 0;
	}
	size_t data_size = sizeof(int);
	*ppDst = new int[RowCount];
	memcpy(*ppDst, (int *)mxGetPr(pSrc), RowCount*data_size);
	return 1;
}

int
CEngMatlab::GetVectorToArray(vector<int> src, mxArray **ppDst)
/*
描述：将int型一维vetcor中的内容转化为matlab中的数组
参数：[IN]src:待转换的vevtor名
      [IN][OUT]ppDst:指向转换后的一维数组的指针的地址
返回值：成功返回1
		转换出错返回-1
		要转换的vector为空时返回0
*/
{
	size_t size = src.size();
	if(size == 0)
	{
		SetError(DATA_EMPTY);
		return 0;
	}
	int *pBuffer = new int[(int)size];

	for (int i = 0; i < (int)size; ++i)
	{
		*(pBuffer+i) = src[i];

	}
	if(GetBufferToArray(pBuffer, ppDst, DATA_INT, 1, (int)size) != 1)
		return -1;
	delete [] pBuffer;	
	return 1;
}


int
CEngMatlab::GetVectorToArray(vector<double> src, mxArray **ppDst)
/*
描述：将double型一维vetcor中的内容转化为matlab中的数组
参数：[IN]src:待转换的vevtor名
	  [IN][OUT]ppDst:指向转换后的一维数组的指针的地址
返回值：成功返回1
		转换出错返回-1
		要转换的vector为空时返回0
*/
{
	size_t size = src.size();
	if(size == 0)
	{
		SetError(DATA_EMPTY);
		return 0;
	}
	double *pBuffer = new double[(int)size];

	for (int i = 0; i < (int)size; ++i)
	{
		*(pBuffer+i) = src[i];

	}
	if(GetBufferToArray(pBuffer, ppDst, DATA_DOUBLE, 1, (int)size) != 1)
	{
		SetError(BUFFER_TO_ARRAY_FAILURE);
		return -1;
	}
	delete [] pBuffer;	
	return 1;
}

void
CEngMatlab::MultiCurvesToSingleFig(bool bFlag)
/*
描述：把多条曲线画到一幅图上或者解除把多条曲线画到一幅图上
参数：[INflag:把多条曲线画到一幅图上 bFlab为true，解除把多条曲线画到一幅图上 bFlab为false
[IN][OUT]ppDst:指向转换后的一维数组的指针的地址
返回值：无
备注：相当于“hold on”和“hold off”
*/
{
	if(bFlag)
	{
		if( engEvalString(ep, "hold on;"))
		{
			SetError(COMMAND_SEND_FAILURE);
		}
	}
	else
	{
		if( engEvalString(ep, "hold off;"))
		{
			SetError(COMMAND_SEND_FAILURE);
		}
	}
}

int
CEngMatlab::NewFigDlg()
/*
描述：产生一个新图对话框
参数：无
返回值：无
*/
{
	char strTemp[16];
	sprintf(strTemp, "figure(%d)", FigNum++);
	if (engEvalString(ep, strTemp))
	{
		SetError(COMMAND_SEND_FAILURE);
	}
	return FigNum-1;
}


int
CEngMatlab::LegendSign(vector<string> legend)
/*
描述：画图例
参数：[IN]legend:vector中的数据为各图例内容
返回值：成功返回1
参数为空返回-1
matlab执行命令时出错返回0
备注：
eg.
vector<string> legend;
legend.push_back(node1);
legend.push_back(node2);
int test = LegendSign(legend);
则matlab执行命令legend('node1', 'node2');
*/
{
	string strTemp = "legend(";
	size_t size = legend.size();
	if(size == 0)
		return -1;
	int size_legend = (int)size;
	for(int i= 0; i < size_legend - 1; ++i)
	{	
		strTemp = strTemp + "'" + legend[i] + "',";
	}
	strTemp = strTemp + "'" + legend[size_legend - 1] + "')";
	const char *chTemp = strTemp.c_str();

	if(engEvalString(ep, chTemp))//0为成功
	{
		SetError(COMMAND_SEND_FAILURE);
		return 0;
	}
	return 1;
}

const char*
CEngMatlab::RanLineSpec(size_t demand)
/*
描述：随机产生曲线特征
参数：[IN]demand:曲线特征的要求
返回值：返回曲线特征字符串
备注：
eg.
const char* strTemp = RanLineSpec(LEGEND_COLOR|LEGEND_POINT);
*/

{
	string color[] = {"b","g","r","c","m","k"};
	string point[] = {".","o","x","+","*","s","d","v","^","<",">","p","h"};
	string line[] = {"-",":","-.","--"};
	string *pTemp = new string("");
	if( (demand&LEGEND_COLOR) == LEGEND_COLOR )
		*pTemp += color[CRandom::Random_U(0,COLOR_NUM-1)];
	if( (demand&LEGEND_POINT) == LEGEND_POINT )
		*pTemp += point[CRandom::Random_U(0,POINT_NUM-1)];
	if( (demand&LEGEND_LINE) == LEGEND_LINE )
		*pTemp += line[CRandom::Random_U(0,LINE_NUM-1)];

	return pTemp->c_str();
}

int
//成功返回1
//失败返回0
CEngMatlab::SubPlot(int m, int n, int p)
/*
描述：执行subplot(m, n, p)命令
参数：[IN]m
	  [IN]n
	  [IN]p
返回值：成功返回1
		失败返回0
*/
{
	char strTemp[16];
	sprintf(strTemp, "subplot(%d,%d,%d)",m, n, p);
	if( engEvalString(ep, strTemp))
	{
		SetError(COMMAND_SEND_FAILURE);
		return 0;
	}
	return 1;
}
int 
CEngMatlab::PlotCurve_2D(vector<double> x, 
						 vector<double> y, 
						 const char *LineSpec,
						 const char *title,
						 const char *xlabel,
						 const char *ylabel)
/**
描述：绘制二维曲线
参数：[IN]x:x坐标
[IN]y：y坐标
[IN]LineSpec:曲线特征
[IN]title:标题
[IN]xlabel:x坐标名称
[IN]ylabel:y坐标名称
返回值：数据转换时出错返回0
		matlab执行命令时出错返回-1
		向Matlab引擎工作空间写入变量时出错返回2
		横纵坐标长度不相同返回-2
		成功返回1
备注：
eg.
vector<double> x;
vector<double> y;
x.push_back(1);
x.push_back(2);
y.push_back(3);
y.push_back(4);

PlotCurve_2D(x,
			y,
			CEngMatlab::RanLineSpec(LEGEND_COLOR), 
			"test PlotCurve_2D",
			"x", 
			"y");
*/
{
	if(x.size() != y.size())
	{
		SetError(PLOT_XY_SIZE_UNEQUAL);
		return -2;
	}
	mxArray *xx = NULL, *yy = NULL;
	if(GetVectorToArray(x, &xx) != 1)
	{
		SetError(VECTOR_TO_MATRIX_FAILURE);
		return 0;
	}
	if(GetVectorToArray(y, &yy) != 1)
	{
		SetError(VECTOR_TO_MATRIX_FAILURE);
		return 0;
	}
	if(engPutVariable(ep, "xx", xx))
	{
		SetError(WRITE_VARIABLE_FAILURE);
		return 2;
	}
	if(engPutVariable(ep, "yy", yy))
	{
		SetError(WRITE_VARIABLE_FAILURE);
		return 2;
	}
	//拼凑曲线特征
	char strLineSpec[80], strTitle[80], strXlabel[80], strYlabel[80];
	sprintf(strLineSpec, "plot(xx, yy, '%s')", LineSpec);
	if(engEvalString(ep,strLineSpec))
	{
		SetError(COMMAND_SEND_FAILURE);
		return -1;
	}
	if(title != NULL)
	{
		sprintf(strTitle, "title('%s')", title);
		if(engEvalString(ep,strTitle))
		{
			SetError(COMMAND_SEND_FAILURE);
			return -1;
		}
	}
	if(xlabel != NULL)
	{
		sprintf(strXlabel, "xlabel('%s')", xlabel);
		if(engEvalString(ep,strXlabel))
		{
			SetError(COMMAND_SEND_FAILURE);
			return -1;
		}
	}
	if (ylabel != NULL)
	{
		sprintf(strYlabel, "ylabel('%s')", ylabel);
		if(engEvalString(ep,strYlabel))
		{
			SetError(COMMAND_SEND_FAILURE);
			return -1;
		}
	}
	DestoryMxPointer(xx);
	DestoryMxPointer(yy);
	return 1;
}

int 
CEngMatlab::PlotCurve_2D(void *x, 
						 void *y, 
						 const unsigned long DataTypeX,
						 const unsigned long DataTypeY,
						 const size_t size,
						 const char *LineSpec,
						 const char *title,
						 const char *xlabel,
						 const char *ylabel)
/**
描述：绘制二维曲线
参数：[IN]x:x坐标
[IN]y：y坐标
[IN]DataTypeX:x的类型
[IN]DataTypeY:y的类型
[IN]size：x和y的长度
[IN]LineSpec:曲线特征
[IN]title:标题
[IN]xlabel:x坐标名称
[IN]ylabel:y坐标名称
返回值：数据转换时出错返回0
		matlab执行命令时出错返回-1
		向Matlab引擎工作空间写入变量时出错返回2
		横纵坐标长度不相同返回-2
		成功返回1
备注：
eg.
int x[] = {1,2,3};
double y[] = {4,5,6};

PlotCurve_2D(x,
			y,
			DATA_INT,
			DATA_DOUBLE,
			3, 
			CEngMatlab::RanLineSpec(LEGEND_COLOR), 
			"test PlotCurve_2D",
			"x", 
			"y");
*/
{
	mxArray *xx = NULL, *yy = NULL;
	if(size == 0)
	{
		SetError(DATA_EMPTY);
		return -2;
	}

	if(GetBufferToArray(x, &xx, DataTypeX, 1, (int)size) != 1)
	{
		SetError(VECTOR_TO_MATRIX_FAILURE);
		return 0;
	}
	if(GetBufferToArray(y, &yy, DataTypeY, 1, (int)size) != 1)
	{
		SetError(VECTOR_TO_MATRIX_FAILURE);
		return 0;
	}
	if(engPutVariable(ep, "xx", xx))
	{
		SetError(WRITE_VARIABLE_FAILURE);
		return 2;
	}
	if(engPutVariable(ep, "yy", yy))
	{
		SetError(WRITE_VARIABLE_FAILURE);
		return 2;
	}
	//拼凑曲线特征
	char strLineSpec[100], strTitle[100], strXlabel[100], strYlabel[100];
	sprintf(strLineSpec, "plot(xx, yy, '%s')", LineSpec);
	if(engEvalString(ep,strLineSpec))
	{
		SetError(COMMAND_SEND_FAILURE);
		return -1;
	}
	if(title != NULL)
	{
		sprintf(strTitle, "title('%s')", title);
		if(engEvalString(ep,strTitle))
		{
			SetError(COMMAND_SEND_FAILURE);
			return -1;
		}
	}
	if(xlabel != NULL)
	{
		sprintf(strXlabel, "xlabel('%s')", xlabel);
		if(engEvalString(ep,strXlabel))
		{
			SetError(COMMAND_SEND_FAILURE);
			return -1;
		}
	}
	if (ylabel != NULL)
	{
		sprintf(strYlabel, "ylabel('%s')", ylabel);
		if(engEvalString(ep,strYlabel))
		{
			SetError(COMMAND_SEND_FAILURE);
			return -1;
		}
	}
	DestoryMxPointer(xx);
	DestoryMxPointer(yy);
	return 1;
}
int 
CEngMatlab::PlotCurve_3D(vector<double> x, 
						 vector<double> y,
						 vector<double> z,
						 const char *LineSpec,
						 const char *title,
						 const char *xlabel,
						 const char *ylabel,
						 const char *zlabel)
/**
描述：绘制三维曲线
参数：[IN]x:x坐标
	[IN]y：y坐标
	[IN]z：z坐标
	[IN]LineSpec:曲线特征
	[IN]title:标题
	[IN]xlabel:x坐标名称
	[IN]ylabel:y坐标名称
	[IN]zlabel:z坐标名称
返回值：成功返回1
		xyz坐标长度不相同返回-2
		数据转换时出错返回0	
		向Matlab引擎工作空间写入变量时出错返回2
		matlab执行命令时出错返回-1
		
备注：
eg.
vector<double> x;
vector<double> y;
vector<double> z;
x.push_back(1);
x.push_back(2);
y.push_back(3);
y.push_back(4);
z.push_back(5);
z.push_back(6);

PlotCurve_3D(x,
			y,
			z,
			CEngMatlab::RanLineSpec(LEGEND_COLOR), 
			"test PlotCurve_3D",
			"x", 
			"y",
			"z");
*/
{
	if(x.size() != y.size() || x.size() != z.size())
	{
		SetError(PLOT_XY_SIZE_UNEQUAL);
		return -2;
	}
	mxArray *xx = NULL, *yy = NULL, *zz = NULL;
	if(GetVectorToArray(x, &xx) != 1)
	{
		SetError(VECTOR_TO_MATRIX_FAILURE);
		return 0;
	}
	if(GetVectorToArray(y, &yy) != 1)
	{
		SetError(VECTOR_TO_MATRIX_FAILURE);
		return 0;
	}
	if(GetVectorToArray(z, &zz) != 1)
	{
		SetError(VECTOR_TO_MATRIX_FAILURE);
		return 0;
	}

	if(engPutVariable(ep, "xx", xx))
	{
		SetError(WRITE_VARIABLE_FAILURE);
		return 2;
	}
	if(engPutVariable(ep, "yy", yy))
	{
		SetError(WRITE_VARIABLE_FAILURE);
		return 2;
	}
	if(engPutVariable(ep, "zz", zz))
	{
		SetError(WRITE_VARIABLE_FAILURE);
		return 2;
	}
	//拼凑曲线特征
	char strLineSpec[80], strTitle[80], strXlabel[80], strYlabel[80], strZlabel[80];
	sprintf(strLineSpec, "plot3(xx, yy, zz, '%s')", LineSpec);
	if(engEvalString(ep,strLineSpec))
	{
		SetError(COMMAND_SEND_FAILURE);
		return -1;
	}
	if(title != NULL)
	{
		sprintf(strTitle, "title('%s')", title);
		if(engEvalString(ep,strTitle))
		{
			SetError(COMMAND_SEND_FAILURE);
			return -1;
		}
	}
	if(xlabel != NULL)
	{
		sprintf(strXlabel, "xlabel('%s')", xlabel);
		if(engEvalString(ep,strXlabel))
		{
			SetError(COMMAND_SEND_FAILURE);
			return -1;
		}
	}
	if (ylabel != NULL)
	{
		sprintf(strYlabel, "ylabel('%s')", ylabel);
		if(engEvalString(ep,strYlabel))
		{
			SetError(COMMAND_SEND_FAILURE);
			return -1;
		}
	}
	if (zlabel != NULL)
	{
		sprintf(strZlabel, "zlabel('%s')", zlabel);
		if(engEvalString(ep,strZlabel))
		{
			SetError(COMMAND_SEND_FAILURE);
			return -1;
		}
	}
	DestoryMxPointer(xx);
	DestoryMxPointer(yy);
	DestoryMxPointer(zz);
	return 1;
}

int 
CEngMatlab::PlotCurve_3D(void *x, 
						 void *y, 
						 void *z,
						 const unsigned long DataTypeX,
						 const unsigned long DataTypeY,
						 const unsigned long DataTypeZ,
						 const size_t size,
						 const char *LineSpec,
						 const char *title,
						 const char *xlabel,
						 const char *ylabel,
						 const char *zlabel)
/**
描述：绘制三维曲线
参数：  [IN]x:x坐标
		[IN]y：y坐标
		[IN]z：z坐标
		[IN]DataTypeX:x的类型
		[IN]DataTypeY:y的类型
		[IN]DataTypeZ:z的类型
		[IN]size：x和y的长度
		[IN]LineSpec:曲线特征
		[IN]title:标题
		[IN]xlabel:x坐标名称
		[IN]ylabel:y坐标名称
返回值：数据转换时出错返回0
		matlab执行命令时出错返回-1
		向Matlab引擎工作空间写入变量时出错返回2
		横纵坐标长度不相同返回-2
		成功返回1
备注：
eg.
int x[] = {1,2,3};
double y[] = {4,5,6};
double z[] = {7,8,9};

PlotCurve_3D(x,
			y,
			z,
			DATA_INT,
			DATA_DOUBLE,
			DATA_DOUBLE,
			3, 
			CEngMatlab::RanLineSpec(LEGEND_COLOR), 
			"test PlotCurve_3D",
			"x", 
			"y",
			"z");
*/
{
	mxArray *xx = NULL, *yy = NULL, *zz = NULL;
	if(size == 0)
	{
		SetError(DATA_EMPTY);
		return -2;
	}

	if(GetBufferToArray(x, &xx, DataTypeX, 1, (int)size) != 1)
	{
		SetError(VECTOR_TO_MATRIX_FAILURE);
		return 0;
	}
	if(GetBufferToArray(y, &yy, DataTypeY, 1, (int)size) != 1)
	{
		SetError(VECTOR_TO_MATRIX_FAILURE);
		return 0;
	}
	if(GetBufferToArray(z, &zz, DataTypeZ, 1, (int)size) != 1)
	{
		SetError(VECTOR_TO_MATRIX_FAILURE);
		return 0;
	}

	if(engPutVariable(ep, "xx", xx))
	{
		SetError(WRITE_VARIABLE_FAILURE);
		return 2;
	}
	if(engPutVariable(ep, "yy", yy))
	{
		SetError(WRITE_VARIABLE_FAILURE);
		return 2;
	}
	if(engPutVariable(ep, "zz", zz))
	{
		SetError(WRITE_VARIABLE_FAILURE);
		return 2;
	}
	//拼凑曲线特征
	char strLineSpec[100], strTitle[100], strXlabel[100], strYlabel[100], strZlabel[100];
	sprintf(strLineSpec, "plot3(xx, yy, zz, '%s')", LineSpec);
	if(engEvalString(ep,strLineSpec))
	{
		SetError(COMMAND_SEND_FAILURE);
		return -1;
	}
	if(title != NULL)
	{
		sprintf(strTitle, "title('%s')", title);
		if(engEvalString(ep,strTitle))
		{
			SetError(COMMAND_SEND_FAILURE);
			return -1;
		}
	}
	if(xlabel != NULL)
	{
		sprintf(strXlabel, "xlabel('%s')", xlabel);
		if(engEvalString(ep,strXlabel))
		{
			SetError(COMMAND_SEND_FAILURE);
			return -1;
		}
	}
	if (ylabel != NULL)
	{
		sprintf(strYlabel, "ylabel('%s')", ylabel);
		if(engEvalString(ep,strYlabel))
		{
			SetError(COMMAND_SEND_FAILURE);
			return -1;
		}
	}
	if (zlabel != NULL)
	{
		sprintf(strZlabel, "zlabel('%s')", zlabel);
		if(engEvalString(ep,strZlabel))
		{
			SetError(COMMAND_SEND_FAILURE);
			return -1;
		}
	}
	DestoryMxPointer(xx);
	DestoryMxPointer(yy);
	DestoryMxPointer(zz);
	return 1;
}

int
CEngMatlab::PlotMesh_3D(double **z,
						int RowCount,
						int ColCount,
						const char *title,//标题
						const char *xlabel,//x坐标名称
						const char *ylabel,//y坐标名称
						const char *zlabel)//z坐标名称
/**
描述：绘制三维曲面(mesh只能画double型数据)
参数：  [IN]z:z坐标,double型
		[IN]RowCount：z的行数
		[IN]ColCount：z的列数
		[IN]title:标题
		[IN]xlabel:x坐标名称
		[IN]ylabel:y坐标名称
		[IN]ylabel:z坐标名称
返回值：数据为空返回-2
		数据转换时出错返回0
		matlab执行命令时出错返回-1
		向Matlab引擎工作空间写入变量时出错返回2
		成功返回1
备注：
eg.
double count = 1;
double **ppSrc = new double*[3];
for(int i = 0; i < 3; ++i)
{
	ppSrc[i] = new double[4];
	for(int j = 0; j < 4; ++j)
	{
		*(*(ppSrc+i)+j) = count++;
	}
}
example.PlotMesh_3D(ppSrc,3,4,"title","x","y","z");

*/
{
	mxArray *zz = NULL;
	if(RowCount*ColCount == 0)
	{
		SetError(DATA_EMPTY);
		return -2;
	}
	if(GetBufferToArray((void **)z, &zz, DATA_DOUBLE, RowCount, ColCount) != 1)
	{
		SetError(VECTOR_TO_MATRIX_FAILURE);
		return 0;
	}

	if(engPutVariable(ep, "zz", zz))
	{
		SetError(WRITE_VARIABLE_FAILURE);
		return 2;
	}
	//拼凑命令
	char strLineSpec[100], strTitle[100], strXlabel[100], strYlabel[100], strZlabel[100];
	sprintf(strLineSpec, "mesh(zz)");
	if(engEvalString(ep, strLineSpec))
	{
		SetError(COMMAND_SEND_FAILURE);
		return -1;
	}
	if(title != NULL)
	{
		sprintf(strTitle, "title('%s')", title);
		if(engEvalString(ep,strTitle))
		{
			SetError(COMMAND_SEND_FAILURE);
			return -1;
		}
	}
	if(xlabel != NULL)
	{
		sprintf(strXlabel, "xlabel('%s')", xlabel);
		if(engEvalString(ep,strXlabel))
		{
			SetError(COMMAND_SEND_FAILURE);
			return -1;
		}
	}
	if (ylabel != NULL)
	{
		sprintf(strYlabel, "ylabel('%s')", ylabel);
		if(engEvalString(ep,strYlabel))
		{
			SetError(COMMAND_SEND_FAILURE);
			return -1;
		}
	}
	if (zlabel != NULL)
	{
		sprintf(strZlabel, "zlabel('%s')", zlabel);
		if(engEvalString(ep,strZlabel))
		{
			SetError(COMMAND_SEND_FAILURE);
			return -1;
		}
	}
	DestoryMxPointer(zz);
	return 1;
}



int 
CEngMatlab::LogLogCurve_2D(void *x, //横坐标
						   void *y, //纵坐标
						   const unsigned long DataTypeX,
						   const unsigned long DataTypeY,
						   const size_t size,
						   const char *LineSpec,//曲线特征
						   const char *title,//标题
						   const char *xlabel,//横坐标名称
						   const char *ylabel)//纵坐标名称
/*
描述：绘制二维曲线
参数：[IN]x:x坐标
	[IN]y：y坐标
	[IN]DataTypeX:x的类型
	[IN]DataTypeX:y的类型
	[IN]size：x和y的长度
	[IN]LineSpec:曲线特征
	[IN]title:标题
	[IN]xlabel:x坐标名称
	[IN]ylabel:y坐标名称
返回值：数据转换时出错返回0
	matlab执行命令时出错返回-1
	向Matlab引擎工作空间写入变量时出错返回2
	数据为空返回-2
	成功返回1

备注：
eg.
int x[] = {1,2,3};
double y[] = {4,5,6};

LogLogCurve_2D(x,
				y,
				DATA_INT,
				DATA_DOUBLE,
				3, 
				CEngMatlab::RanLineSpec(LEGEND_COLOR), 
				"test loglog",
				"x", 
				"y");
*/
{
	mxArray *xx = NULL, *yy = NULL;
	if(size == 0)
	{
		SetError(DATA_EMPTY);
		return -2;
	}
	if(GetBufferToArray(x, &xx, DataTypeX, 1, (int)size) != 1)
	{
		SetError(VECTOR_TO_MATRIX_FAILURE);
		return 0;
	}
	if(GetBufferToArray(y, &yy, DataTypeY, 1, (int)size) != 1)
	{
		SetError(VECTOR_TO_MATRIX_FAILURE);
		return 0;
	}
	if(engPutVariable(ep, "xx", xx))
	{
		SetError(WRITE_VARIABLE_FAILURE);
		return 2;
	}
	if(engPutVariable(ep, "yy", yy))
	{
		SetError(WRITE_VARIABLE_FAILURE);
		return 2;
	}
	//拼凑曲线特征
	char strLineSpec[100], strTitle[100], strXlabel[100], strYlabel[100];
	sprintf(strLineSpec, "loglog(xx, yy, '%s')", LineSpec);
	if(engEvalString(ep,strLineSpec))
	{
		SetError(COMMAND_SEND_FAILURE);
		return -1;
	}
	if(title != NULL)
	{
		sprintf(strTitle, "title('%s')", title);
		if(engEvalString(ep,strTitle))
		{
			SetError(COMMAND_SEND_FAILURE);
			return -1;
		}
	}
	if(xlabel != NULL)
	{
		sprintf(strXlabel, "xlabel('%s')", xlabel);
		if(engEvalString(ep,strXlabel))
		{
			SetError(COMMAND_SEND_FAILURE);
			return -1;
		}
	}
	if (ylabel != NULL)
	{
		sprintf(strYlabel, "ylabel('%s')", ylabel);
		if(engEvalString(ep,strYlabel))
		{
			SetError(COMMAND_SEND_FAILURE);
			return -1;
		}
	}
	DestoryMxPointer(xx);
	DestoryMxPointer(yy);
	return 1;
}

int
CEngMatlab::Bar(void *x,
				void *y,
				const unsigned long DataTypeX,
				const unsigned long DataTypeY,
				const size_t size,
				const char *LineSpec,//曲线特征
				const char *title,//标题
				const char *xlabel,//横坐标名称
				const char *ylabel)
/*
描述：绘制直方图
参数：[IN]x:x坐标
	[IN]y：y坐标
	[IN]DataTypeX:x的类型
	[IN]DataTypeX:y的类型
	[IN]size：x和y的长度
	[IN]LineSpec:曲线特征
	[IN]title:标题
	[IN]xlabel:x坐标名称
	[IN]ylabel:y坐标名称
返回值：数据为空返回-2
		VECTOR_TO_MATRIX_FAILURE错误返回0
		matlab执行命令时出错返回-1
		向Matlab引擎工作空间写入变量时出错返回2
		成功返回1
备注：
eg.
int x[] = {1,2,3};
double y[] = {4,5,6};

Bar(x,
	y,
	DATA_INT,
	DATA_DOUBLE,
	3, 
	CEngMatlab::RanLineSpec(LEGEND_COLOR), 
	"test bar",
	"x", 
	"y");

*/
{
	mxArray *xx = NULL, *yy = NULL;
	if(size == 0)
	{
		SetError(DATA_EMPTY);
		return -2;
	}
	if(GetBufferToArray(x, &xx, DataTypeX, 1, (int)size) != 1)
	{
		SetError(VECTOR_TO_MATRIX_FAILURE);
		return 0;
	}
	if(GetBufferToArray(y, &yy, DataTypeY, 1, (int)size) != 1)
	{
		SetError(VECTOR_TO_MATRIX_FAILURE);
		return 0;
	}
	if(engPutVariable(ep, "xx", xx))
	{
		SetError(WRITE_VARIABLE_FAILURE);
		return 2;
	}
	if(engPutVariable(ep, "yy", yy))
	{
		SetError(WRITE_VARIABLE_FAILURE);
		return 2;
	}
	//拼凑曲线特征
	char strLineSpec[100], strTitle[100], strXlabel[100], strYlabel[100];
	sprintf(strLineSpec, "bar(xx, yy, '%s')", LineSpec);
	sprintf(strTitle, "title('%s')", title);
	sprintf(strXlabel, "xlabel('%s')", xlabel);
	sprintf(strYlabel, "ylabel('%s')", ylabel);
	if(engEvalString(ep,strLineSpec))
	{
		SetError(COMMAND_SEND_FAILURE);
		return -1;
	}
	if(engEvalString(ep,strTitle))
	{
		SetError(COMMAND_SEND_FAILURE);
		return -1;
	}
	if(engEvalString(ep,strXlabel))
	{
		SetError(COMMAND_SEND_FAILURE);
		return -1;
	}
	if(engEvalString(ep,strYlabel))
	{
		SetError(COMMAND_SEND_FAILURE);
		return -1;
	}
	DestoryMxPointer(xx);
	DestoryMxPointer(yy);
	return 1;

}

void
CEngMatlab::test()
{
	int count = 1;
	int **ppSrc = new int*[3];
	for(int i = 0; i < 3; ++i)
	{
		ppSrc[i] = new int[4];
		for(int j = 0; j < 4; ++j)
		{
			*(*(ppSrc+i)+j) = count++;
		}
	}
	
	mxArray *xx = NULL;
	GetBufferToArray((void **)ppSrc, &xx, DATA_INT, 3, 4);
	int **ppDst = NULL;
	GetArrayToBuffer(xx, &ppDst);

	for(int i = 0; i < 3; ++i)
	{
		for(int j = 0; j < 4; ++j)
			cout << ppDst[i][j] << "  ";
		cout << endl;
	}
	cout << endl << endl << endl;
	int *pSrc = new int[3];
	for(int i = 0; i < 3; ++i)
		*(pSrc+i) = count++;
	mxArray *yy = NULL;
	GetBufferToArray(pSrc, &yy, DATA_INT, 3, 1);
	int *pDst = NULL;
	GetArrayToBuffer(yy, &pDst);
	for(int i = 0; i < 3; ++i)
		cout << *(pDst+i) << " ";
	cout << endl << endl;

}