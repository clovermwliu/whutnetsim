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
	if( engClose(ep))//�ɹ��˳�����0
	{
		SetError(CLOSE_FAILURE);
		return false;
	}
	return true;
}

bool
CEngMatlab::SendCommand(const char *string)
/*
������ִ�з��͸�matlab������
������[IN]string����matlabִ�е�������ַ���
����ֵ���ɹ�����true
		���ɹ�����false
*/
{
	if(engEvalString(ep, string))//0Ϊ�ɹ�
	{
		SetError(COMMAND_SEND_FAILURE);
		return false;
	}
	return true;
}

int
CEngMatlab::Transpose(mxArray **ppMatrix)
/*
��������������ת�þ���,��ԭ��������
������[IN][OUT]ppMatrix:ָ���ת�þ���ĵ�ַ;ָ��ת�ú������µ�ַ
����ֵ���ɹ�����1
WRITE_VARIABLE_FAILURE��������0
COMMAND_SEND_FAILURE��������-1
��ע��ע�⺯��������ԭ����ĵ�ַ���¾���ĵ�ַ��ͬ��ԭ�����Զ�����
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
	DestoryMxPointer(*ppMatrix);//����ԭ����
	*ppMatrix = engGetVariable(ep, "matrix");
	return 1;
}

int 
CEngMatlab::Transpose(const mxArray *pSrcMatrix, 
					  mxArray **ppNewMatrix)
/*
��������pSrcMatrixָ��ľ���ת�ã�ppNewMatrixָ���¾���ĵ�ַ
������[IN]pSrcMatrix:ָ���ת�õľ���ĵ�ַ
[IN][OUT]ppNewMatrix:ָ��ת�ú����ָ��ĵ�ַ
����ֵ���ɹ�����1
WRITE_VARIABLE_FAILURE��������0
COMMAND_SEND_FAILURE��������-1
��ע��ԭ����δ������
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
����������mxArrayָ��
������[IN]pArray:�����ٵ�mxArrayָ��
����ֵ����
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
��������C�е�����תΪmatlab�е�����
������[IN]pSrc:ָ��C�������ַ
      [IN][OUT]ppDst:ָ��ת�����matlab�е�����ĵ�ַ
	  [IN]DataType:��ת�������������
	  [IN]RowCount:C�����������
	  [IN]ColCount:C�����������
����ֵ���ɹ�����1
        Ҫת�����������Ͳ�֧���򷵻�0
        ����matlab mxArrayʧ�ܷ���-1
��ע��
eg.
------------       --------
|1 2 3 4   |       |1 5 9 |   ת��  ------------
|5 6 7 8   | --->  |2 6 10|   --->  |1 2 3 4   |
|9 10 11 12|       |3 7 11|         |5 6 7 8   |
------------       |4 8 12|         |9 10 11 12|
                   --------         ------------ 
double z[3][4]= {{1,2,3,4},{5,6,7,8},{9,10,11,12}};
ת��Ϊmatlab�е�3��4�У�
ӦΪ
1 2 3 4
5 6 7 8
9 10 11 12 ---�����1
��*ppDst = mxCreateDoubleMatrix(RowCount, ColCount, mxREAL);��
��洢��matlab��Ϊ1 4 7 10
2 5 8 11
3 6 9 12 ----�����2
����Ҫ��õ����1������	*ppDst = mxCreateDoubleMatrix(ColCount, RowCount, mxREAL);
��ʱ�洢��matlab��Ϊ
1 5 9
2 6 10
3 7 11
4 8 12---�����3
���3Ϊ���1��ת�þ���
����������ת�ú���
MATLAB�еĴ洢���Ϊ
1 2 3 4
5 6 7 8
9 10 11 12 ---�����4
����z���
*/

{
	size_t data_size;
	switch(DataType)
	{
	case DATA_INT: 
		{
			data_size = sizeof(int);
			*ppDst = mxCreateNumericMatrix(ColCount, RowCount, mxINT32_CLASS, mxREAL);//ColCount��RowCount��
		}
		break;
	case DATA_DOUBLE: 
		{
			data_size = sizeof(double);
			*ppDst = mxCreateDoubleMatrix(ColCount, RowCount, mxREAL);//ColCount��RowCount��
		}
		break;
	case DATA_UNSIGNED_INT:
		{
			data_size = sizeof(double);
			*ppDst = mxCreateNumericMatrix(ColCount, RowCount, mxUINT32_CLASS, mxREAL);//ColCount��RowCount��
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
	Transpose(ppDst);//ת��
	return 1;
}

int
CEngMatlab::GetBufferToArray(void **ppSrc,
							 mxArray **ppDst,
							 const unsigned long DataType,
							 int RowCount,
							 int ColCount)
/*
������//��C�еĶ�ά����תΪmatlab�е�����
������[IN]ppSrc:ָ���ά�����ָ��
      [IN][OUT]ppDst:ָ��ת�����matlab�е�����ĵ�ַ
	  [IN]DataType:��ת�������������
	  [IN]RowCount:C�����������
	  [IN]ColCount:C�����������
����ֵ���ɹ�����1
        Ҫת�����������Ͳ�֧���򷵻�0
        ����matlab mxArrayʧ�ܷ���-1
��ע��
eg.
------------       --------
|1 2 3 4   |       |1 5 9 |   ת��  ------------
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
			*ppDst = mxCreateNumericMatrix(ColCount, RowCount, mxINT32_CLASS, mxREAL);//ColCount��RowCount��
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
			*ppDst =  mxCreateDoubleMatrix(ColCount, RowCount, mxREAL);//ColCount��RowCount��
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
			*ppDst = mxCreateNumericMatrix(ColCount, RowCount, mxUINT32_CLASS, mxREAL);//ColCount��RowCount��
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

	Transpose(ppDst);//ת��
	return 1;
}

int
CEngMatlab::GetArrayToBuffer(mxArray *pSrc,//Դ
							 double ***pppDst)//Ŀ��
/*
��������matlab�е�����תΪdouble�͵Ķ�ά����
������[IN]pSrc:ָ��matlab������ĵ�ַ
	  [IN][OUT]pppDst:ָ��ת����Ķ�ά�����ָ��ĵ�ַ
����ֵ���ɹ�����1
		����Ϊ�շ���-1
		�������Ͳ�ƥ�䷵��0
��ע��
eg.
------------       --------
|1 2 3 4   | ת��  |1 5 9 |         ------------
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
	Transpose(&pSrc);//ת��
	int RowCount = (int)mxGetM(pSrc);
	int ColCount = (int)mxGetN(pSrc);
	if (RowCount == 1 ||ColCount == 1)
	{
		SetError(DATA_TYPE_NO_MATCH);
		return 0;
	}
	
	size_t data_size = sizeof(double);
	//��̬������,ColCount�У�RowCount��//�ڴ�ռ䲻����
	*pppDst = new double *[ColCount];
	for (int i = 0; i < ColCount; ++i)
	{
		(*pppDst)[i] = new double[RowCount];
		memcpy((*pppDst)[i], mxGetPr(pSrc)+i*RowCount, RowCount*data_size);
	}
	return 1;
}

int
CEngMatlab::GetArrayToBuffer(mxArray *pSrc,//Դ
							 int ***pppDst)//Ŀ��
/*
��������matlab�е�����תΪint�͵Ķ�ά����
������[IN]pSrc:ָ��matlab������ĵ�ַ
	  [IN][OUT]pppDst:ָ��ת����Ķ�ά�����ָ��ĵ�ַ
����ֵ���ɹ�����1
����Ϊ�շ���-1
�������Ͳ�ƥ�䷵��0
��ע��
eg.
------------       --------
|1 2 3 4   | ת��  |1 5 9 |         ------------
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
	Transpose(&pSrc);//ת��
	int RowCount = (int)mxGetM(pSrc);
	int ColCount = (int)mxGetN(pSrc);
	if (RowCount == 1 ||ColCount == 1)
	{
		SetError(DATA_TYPE_NO_MATCH);
		return 0;
	}
	size_t data_size = sizeof(int);
	//��̬������,ColCount�У�RowCount��//�ڴ�ռ䲻����
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
��������matlab�еĵ�������תΪC�е�double����
����ֵ���ɹ�����1
����Ϊ�շ���-1
�������Ͳ�ƥ�䷵��0
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
CEngMatlab::GetArrayToBuffer(mxArray *pSrc,//Դ
							 double **ppDst)//Ŀ��
/*
��������matlab�е�����תΪdouble�͵�һά����
������[IN]pSrc:ָ��matlab������ĵ�ַ
      [IN][OUT]ppDst:ָ��ת�����һά�����ָ��ĵ�ַ
����ֵ���ɹ�����1
		����Ϊ�շ���-1
		�������Ͳ�ƥ�䷵��0
��ע��
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
CEngMatlab::GetArrayToBuffer(mxArray *pSrc,//Դ
							 int **ppDst)//Ŀ��
/*
��������matlab�е�����תΪint�͵�һά����
������[IN]pSrc:ָ��matlab������ĵ�ַ
      [IN][OUT]ppDst:ָ��ת�����һά�����ָ��ĵ�ַ
����ֵ���ɹ�����1
		����Ϊ�շ���-1
		�������Ͳ�ƥ�䷵��0
��ע��
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
��������int��һάvetcor�е�����ת��Ϊmatlab�е�����
������[IN]src:��ת����vevtor��
      [IN][OUT]ppDst:ָ��ת�����һά�����ָ��ĵ�ַ
����ֵ���ɹ�����1
		ת��������-1
		Ҫת����vectorΪ��ʱ����0
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
��������double��һάvetcor�е�����ת��Ϊmatlab�е�����
������[IN]src:��ת����vevtor��
	  [IN][OUT]ppDst:ָ��ת�����һά�����ָ��ĵ�ַ
����ֵ���ɹ�����1
		ת��������-1
		Ҫת����vectorΪ��ʱ����0
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
�������Ѷ������߻���һ��ͼ�ϻ��߽���Ѷ������߻���һ��ͼ��
������[INflag:�Ѷ������߻���һ��ͼ�� bFlabΪtrue������Ѷ������߻���һ��ͼ�� bFlabΪfalse
[IN][OUT]ppDst:ָ��ת�����һά�����ָ��ĵ�ַ
����ֵ����
��ע���൱�ڡ�hold on���͡�hold off��
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
����������һ����ͼ�Ի���
��������
����ֵ����
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
��������ͼ��
������[IN]legend:vector�е�����Ϊ��ͼ������
����ֵ���ɹ�����1
����Ϊ�շ���-1
matlabִ������ʱ������0
��ע��
eg.
vector<string> legend;
legend.push_back(node1);
legend.push_back(node2);
int test = LegendSign(legend);
��matlabִ������legend('node1', 'node2');
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

	if(engEvalString(ep, chTemp))//0Ϊ�ɹ�
	{
		SetError(COMMAND_SEND_FAILURE);
		return 0;
	}
	return 1;
}

const char*
CEngMatlab::RanLineSpec(size_t demand)
/*
���������������������
������[IN]demand:����������Ҫ��
����ֵ���������������ַ���
��ע��
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
//�ɹ�����1
//ʧ�ܷ���0
CEngMatlab::SubPlot(int m, int n, int p)
/*
������ִ��subplot(m, n, p)����
������[IN]m
	  [IN]n
	  [IN]p
����ֵ���ɹ�����1
		ʧ�ܷ���0
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
���������ƶ�ά����
������[IN]x:x����
[IN]y��y����
[IN]LineSpec:��������
[IN]title:����
[IN]xlabel:x��������
[IN]ylabel:y��������
����ֵ������ת��ʱ������0
		matlabִ������ʱ������-1
		��Matlab���湤���ռ�д�����ʱ������2
		�������곤�Ȳ���ͬ����-2
		�ɹ�����1
��ע��
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
	//ƴ����������
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
���������ƶ�ά����
������[IN]x:x����
[IN]y��y����
[IN]DataTypeX:x������
[IN]DataTypeY:y������
[IN]size��x��y�ĳ���
[IN]LineSpec:��������
[IN]title:����
[IN]xlabel:x��������
[IN]ylabel:y��������
����ֵ������ת��ʱ������0
		matlabִ������ʱ������-1
		��Matlab���湤���ռ�д�����ʱ������2
		�������곤�Ȳ���ͬ����-2
		�ɹ�����1
��ע��
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
	//ƴ����������
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
������������ά����
������[IN]x:x����
	[IN]y��y����
	[IN]z��z����
	[IN]LineSpec:��������
	[IN]title:����
	[IN]xlabel:x��������
	[IN]ylabel:y��������
	[IN]zlabel:z��������
����ֵ���ɹ�����1
		xyz���곤�Ȳ���ͬ����-2
		����ת��ʱ������0	
		��Matlab���湤���ռ�д�����ʱ������2
		matlabִ������ʱ������-1
		
��ע��
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
	//ƴ����������
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
������������ά����
������  [IN]x:x����
		[IN]y��y����
		[IN]z��z����
		[IN]DataTypeX:x������
		[IN]DataTypeY:y������
		[IN]DataTypeZ:z������
		[IN]size��x��y�ĳ���
		[IN]LineSpec:��������
		[IN]title:����
		[IN]xlabel:x��������
		[IN]ylabel:y��������
����ֵ������ת��ʱ������0
		matlabִ������ʱ������-1
		��Matlab���湤���ռ�д�����ʱ������2
		�������곤�Ȳ���ͬ����-2
		�ɹ�����1
��ע��
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
	//ƴ����������
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
						const char *title,//����
						const char *xlabel,//x��������
						const char *ylabel,//y��������
						const char *zlabel)//z��������
/**
������������ά����(meshֻ�ܻ�double������)
������  [IN]z:z����,double��
		[IN]RowCount��z������
		[IN]ColCount��z������
		[IN]title:����
		[IN]xlabel:x��������
		[IN]ylabel:y��������
		[IN]ylabel:z��������
����ֵ������Ϊ�շ���-2
		����ת��ʱ������0
		matlabִ������ʱ������-1
		��Matlab���湤���ռ�д�����ʱ������2
		�ɹ�����1
��ע��
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
	//ƴ������
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
CEngMatlab::LogLogCurve_2D(void *x, //������
						   void *y, //������
						   const unsigned long DataTypeX,
						   const unsigned long DataTypeY,
						   const size_t size,
						   const char *LineSpec,//��������
						   const char *title,//����
						   const char *xlabel,//����������
						   const char *ylabel)//����������
/*
���������ƶ�ά����
������[IN]x:x����
	[IN]y��y����
	[IN]DataTypeX:x������
	[IN]DataTypeX:y������
	[IN]size��x��y�ĳ���
	[IN]LineSpec:��������
	[IN]title:����
	[IN]xlabel:x��������
	[IN]ylabel:y��������
����ֵ������ת��ʱ������0
	matlabִ������ʱ������-1
	��Matlab���湤���ռ�д�����ʱ������2
	����Ϊ�շ���-2
	�ɹ�����1

��ע��
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
	//ƴ����������
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
				const char *LineSpec,//��������
				const char *title,//����
				const char *xlabel,//����������
				const char *ylabel)
/*
����������ֱ��ͼ
������[IN]x:x����
	[IN]y��y����
	[IN]DataTypeX:x������
	[IN]DataTypeX:y������
	[IN]size��x��y�ĳ���
	[IN]LineSpec:��������
	[IN]title:����
	[IN]xlabel:x��������
	[IN]ylabel:y��������
����ֵ������Ϊ�շ���-2
		VECTOR_TO_MATRIX_FAILURE���󷵻�0
		matlabִ������ʱ������-1
		��Matlab���湤���ռ�д�����ʱ������2
		�ɹ�����1
��ע��
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
	//ƴ����������
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