//����޸�ʱ�䣺10.05.07
//����Ա����Цӯ
//�汾�ţ�1.5
/*
vs2005���ã�
[1]��C/C++  -> ���� -> ���Ӱ�Ŀ¼ ����� matlab��extern\include  
[2]��������  ->  ���� -> ���ӿ�Ŀ¼ ����� matlab ��extern\lib\win32\microsoft   
[3]�������� -> ���� -> ���������������libeng.lib  libmx.lib libmat.lib
*/   

#pragma once
#include "engine.h"
#include <string>
#include <vector>
#include "Random.h"
using namespace std;

//������
#define OPEN_FAILURE 0x00000001
#define CLOSE_FAILURE 0x00000002
#define COMMAND_SEND_FAILURE 0x00000003
#define DATA_TYPE_NO_SUPPORT 0x00000004
#define DATA_EMPTY 0x00000005
#define ALLOC_MATARRAY_FAILURE 0x00000006
#define VECTOR_TO_MATRIX_FAILURE 0x00000007
#define WRITE_VARIABLE_FAILURE 0x00000008
#define PLOT_XY_SIZE_UNEQUAL 0x00000009
#define ARRAY_TO_BUFFER_FAILURE 0x0000000A
#define POLY_XY_SIZE_UNEQUAL 0x0000000B
#define PARAMETER_ERROR 0x0000000C
#define BUFFER_TO_ARRAY_FAILURE 0x0000000D
#define DOUBLE_VECTOR_TO_ARRAY_FAILURE 0x0000000E
#define DATA_TYPE_NO_MATCH 0x0000000F
#define ARRAY_TO_DOUBLE_FAILURE 0x00000010
//��������
#define DATA_INT 0x00000001
#define DATA_DOUBLE 0x00000010
#define DATA_UNSIGNED_INT 0x00000100
//ͼ������
#define LEGEND_COLOR 0x00000001
#define LEGEND_POINT 0x00000010
#define LEGEND_LINE 0x00000100
//ͼ������
#define COLOR_NUM 6
#define POINT_NUM 13
#define LINE_NUM 4

class CEngMatlab
{
public:
	CEngMatlab(void) { OpenEngine(); dwLastError = 0; FigNum = 1; };

public:
	virtual ~CEngMatlab(void);

protected:

	Engine *ep;//����ָ��
	
	unsigned long dwLastError;//������
	
	size_t FigNum;

	void SetError(unsigned long error_code) { dwLastError = error_code; }//���ô�����
	

	//------------------------------------------------------
	//�����������
	//----------------------------------------------------
	bool OpenEngine();
	bool CloseEngine();	


	//------------------------------------------------------
	//���ݽӿ�
	//----------------------------------------------------

	int GetBufferToArray(void *pSrc, //Դ
		                 mxArray **ppDst,//Ŀ��
		                 const unsigned long DataType,//Դ����������
		                 int RowCount,//Array����
		                 int ColCount); //Array���� //��C�е�һά���ά����תΪmatlab�е�����,��ת�����������ڴ���������

	int GetBufferToArray(void **ppSrc, //ָ���ά�����ָ��
						mxArray **ppDst,//Ŀ��
						const unsigned long DataType,//Դ����������
						int RowCount,//**pSrc����
						int ColCount); //**pSrc���� //��C�еĶ�ά����תΪmatlab�е�����

	int GetArrayToBuffer(mxArray *pSrc,//Դ
		                 double ***pppDst);  //��matlab�е�����תΪdouble�͵Ķ�ά����
	
	int GetArrayToBuffer(mxArray *pSrc,//Դ
		                 int ***pppDst); //��matlab�е�����תΪint�͵Ķ�ά����

	int GetArrayToBuffer(mxArray *pSrc,//Դ
		                 double **ppDst);  //��matlab�е�����תΪdouble�͵�һά����
	
	int GetArrayToBuffer(mxArray *pSrc,//Դ
		                 int **ppDst);  //��matlab�е�����תΪint�͵�һά����

	int GetVectorToArray(vector<int> src, 
	                     mxArray **ppDst);//��һάint��vectorת����matlab�е�һά����
	
	int GetVectorToArray(vector<double> src, 
		                 mxArray **ppDst);///��һάdouble��vectorת����matlab�е�һά����

	int GetArrayToDouble(mxArray *pSrc,
		                 double *pDst);//��matlab�еĵ�������תΪC�е�double����

	
	void DestoryMxPointer(mxArray *pArray);//����mxArrayָ��
	
	
	
	//------------------------------------------------------
	//�����������
	//----------------------------------------------------

	int Transpose(mxArray **ppMatrix);//��������ת�þ���,��ԭ��������

	int Transpose(const mxArray *pSrcMatrix, 
		          mxArray **ppNewMatrix);//��pSrcMatrixָ��ľ���ת�ã�ppNewMatrixָ���¾���ĵ�ַ
    
	virtual int Matrix_Compute(const mxArray *pSrc1Matrix,
		                       const mxArray *pSrc2Matrix,
							   mxArray **ppResultMatrix) { return 1;}
		
public:		
	
	//------------------------------------------------------
	//��ͼ����
	//-----------------------------------------------------

    
    /*------------ ���ƶ�ά����---------------------------*/

	int PlotCurve_2D(vector<double> x, //������
		             vector<double> y, //������
		             const char *LineSpec,//��������
		             const char *title,//����
		             const char *xlabel,//����������
		             const char *ylabel);//����������
 
	
	int PlotCurve_2D(void *x, //������
		             void *y,//������
		             const unsigned long DataTypeX,//��������
		             const unsigned long DataTypeY,//��������
		             size_t size,
		             const char *LineSpec,//��������
		             const char *title,//����
		             const char *xlabel,//����������
		             const char *ylabel);//����������
 
	/*------------ ������ά����---------------------------*/
	int PlotCurve_3D(vector<double> x, 
		             vector<double> y,
					 vector<double> z,
		             const char *LineSpec,//��������
	        	     const char *title,//����
		             const char *xlabel,//x��������
		             const char *ylabel,//y��������
					 const char *zlabel);//z��������


	int PlotCurve_3D(void *x, 
		             void *y,
					 void *z,
		             const unsigned long DataTypeX,//��������
		             const unsigned long DataTypeY,//��������
					 const unsigned long DataTypeZ,//��������
		             size_t size,
		             const char *LineSpec,//��������
		             const char *title,//����
		             const char *xlabel,//x��������
		             const char *ylabel,//y��������
					 const char *zlabel);//z��������

	///*------------ ������ά����---------------------------*/
	
	int PlotMesh_3D(double **z,
					int RowCount,
					int ColCount,
					const char *title,//����
					const char *xlabel,//x��������
					const char *ylabel,//y��������
					const char *zlabel);//z��������


	/*------------ ���ƶ�ά˫��������---------------------------*/
	int LogLogCurve_2D( void *x, //������
						void *y,//������
						const unsigned long DataTypeX,//��������
						const unsigned long DataTypeY,//��������
						size_t size,
						const char *LineSpec,//��������
						const char *title,//����
						const char *xlabel,//����������
						const char *ylabel);//����������

	/*------------ ���ƶ�άֱ��ͼ---------------------------*/
	int Bar(void *x,
			void *y,
			const unsigned long DataTypeX,
			const unsigned long DataTypeY,
			size_t size,
			const char *LineSpec,//��������
			const char *title,//����
			const char *xlabel,//����������
			const char *ylabel);

	/*------------ ��ͼ��������---------------------------*/
	
	int SubPlot(int m,
		        int n,
				int p);//subplot(m, n, p)

	void MultiCurvesToSingleFig(bool bFlag);//�Ѷ������߻���һ��ͼ�� bFlabΪtrue;����Ѷ������߻���һ��ͼ�� bFlabΪfalse	
	
	int NewFigDlg();//����һ����ͼ�Ի���
	
	int LegendSign(vector<string> legend);//��ͼ����vector�е�����Ϊ��ͼ������

	static const char* RanLineSpec(size_t demand = LEGEND_COLOR|LEGEND_POINT|LEGEND_LINE);//���������������
	
	bool SendCommand(const char *string);//����Ϊ����������޲���


	//------------------------------------------------------
	//��������
	//----------------------------------------------------
	unsigned long GetError() { return dwLastError;}


	void test();
};

