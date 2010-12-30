//最后修改时间：10.05.07
//程序员：张笑盈
//版本号：1.5
/*
vs2005设置：
[1]在C/C++  -> 常规 -> 附加包目录 中添加 matlab的extern\include  
[2]在链接器  ->  常规 -> 附加库目录 中添加 matlab 的extern\lib\win32\microsoft   
[3]在链接器 -> 输入 -> 附加依赖项中添加libeng.lib  libmx.lib libmat.lib
*/   

#pragma once
#include "engine.h"
#include <string>
#include <vector>
#include "Random.h"
using namespace std;

//错误码
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
//数据类型
#define DATA_INT 0x00000001
#define DATA_DOUBLE 0x00000010
#define DATA_UNSIGNED_INT 0x00000100
//图例类型
#define LEGEND_COLOR 0x00000001
#define LEGEND_POINT 0x00000010
#define LEGEND_LINE 0x00000100
//图例数量
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

	Engine *ep;//引擎指针
	
	unsigned long dwLastError;//错误码
	
	size_t FigNum;

	void SetError(unsigned long error_code) { dwLastError = error_code; }//设置错误码
	

	//------------------------------------------------------
	//引擎操作方法
	//----------------------------------------------------
	bool OpenEngine();
	bool CloseEngine();	


	//------------------------------------------------------
	//数据接口
	//----------------------------------------------------

	int GetBufferToArray(void *pSrc, //源
		                 mxArray **ppDst,//目的
		                 const unsigned long DataType,//源的数据类型
		                 int RowCount,//Array的行
		                 int ColCount); //Array的列 //将C中的一维或二维数组转为matlab中的数组,待转换的数组在内存中需连续

	int GetBufferToArray(void **ppSrc, //指向二维数组的指针
						mxArray **ppDst,//目的
						const unsigned long DataType,//源的数据类型
						int RowCount,//**pSrc的行
						int ColCount); //**pSrc的列 //将C中的二维数组转为matlab中的数组

	int GetArrayToBuffer(mxArray *pSrc,//源
		                 double ***pppDst);  //将matlab中的数组转为double型的二维数组
	
	int GetArrayToBuffer(mxArray *pSrc,//源
		                 int ***pppDst); //将matlab中的数组转为int型的二维数组

	int GetArrayToBuffer(mxArray *pSrc,//源
		                 double **ppDst);  //将matlab中的数组转为double型的一维数组
	
	int GetArrayToBuffer(mxArray *pSrc,//源
		                 int **ppDst);  //将matlab中的数组转为int型的一维数组

	int GetVectorToArray(vector<int> src, 
	                     mxArray **ppDst);//把一维int型vector转化成matlab中的一维数组
	
	int GetVectorToArray(vector<double> src, 
		                 mxArray **ppDst);///把一维double型vector转化成matlab中的一维数组

	int GetArrayToDouble(mxArray *pSrc,
		                 double *pDst);//把matlab中的单个数据转为C中的double数据

	
	void DestoryMxPointer(mxArray *pArray);//销毁mxArray指针
	
	
	
	//------------------------------------------------------
	//矩阵操作方法
	//----------------------------------------------------

	int Transpose(mxArray **ppMatrix);//重新生成转置矩阵,将原矩阵销毁

	int Transpose(const mxArray *pSrcMatrix, 
		          mxArray **ppNewMatrix);//将pSrcMatrix指向的矩阵转置，ppNewMatrix指向新矩阵的地址
    
	virtual int Matrix_Compute(const mxArray *pSrc1Matrix,
		                       const mxArray *pSrc2Matrix,
							   mxArray **ppResultMatrix) { return 1;}
		
public:		
	
	//------------------------------------------------------
	//画图方法
	//-----------------------------------------------------

    
    /*------------ 绘制二维曲线---------------------------*/

	int PlotCurve_2D(vector<double> x, //横坐标
		             vector<double> y, //纵坐标
		             const char *LineSpec,//曲线特征
		             const char *title,//标题
		             const char *xlabel,//横坐标名称
		             const char *ylabel);//纵坐标名称
 
	
	int PlotCurve_2D(void *x, //横坐标
		             void *y,//纵坐标
		             const unsigned long DataTypeX,//数据类型
		             const unsigned long DataTypeY,//数据类型
		             size_t size,
		             const char *LineSpec,//曲线特征
		             const char *title,//标题
		             const char *xlabel,//横坐标名称
		             const char *ylabel);//纵坐标名称
 
	/*------------ 绘制三维曲线---------------------------*/
	int PlotCurve_3D(vector<double> x, 
		             vector<double> y,
					 vector<double> z,
		             const char *LineSpec,//曲线特征
	        	     const char *title,//标题
		             const char *xlabel,//x坐标名称
		             const char *ylabel,//y坐标名称
					 const char *zlabel);//z坐标名称


	int PlotCurve_3D(void *x, 
		             void *y,
					 void *z,
		             const unsigned long DataTypeX,//数据类型
		             const unsigned long DataTypeY,//数据类型
					 const unsigned long DataTypeZ,//数据类型
		             size_t size,
		             const char *LineSpec,//曲线特征
		             const char *title,//标题
		             const char *xlabel,//x坐标名称
		             const char *ylabel,//y坐标名称
					 const char *zlabel);//z坐标名称

	///*------------ 绘制三维曲面---------------------------*/
	
	int PlotMesh_3D(double **z,
					int RowCount,
					int ColCount,
					const char *title,//标题
					const char *xlabel,//x坐标名称
					const char *ylabel,//y坐标名称
					const char *zlabel);//z坐标名称


	/*------------ 绘制二维双对数曲线---------------------------*/
	int LogLogCurve_2D( void *x, //横坐标
						void *y,//纵坐标
						const unsigned long DataTypeX,//数据类型
						const unsigned long DataTypeY,//数据类型
						size_t size,
						const char *LineSpec,//曲线特征
						const char *title,//标题
						const char *xlabel,//横坐标名称
						const char *ylabel);//纵坐标名称

	/*------------ 绘制二维直方图---------------------------*/
	int Bar(void *x,
			void *y,
			const unsigned long DataTypeX,
			const unsigned long DataTypeY,
			size_t size,
			const char *LineSpec,//曲线特征
			const char *title,//标题
			const char *xlabel,//横坐标名称
			const char *ylabel);

	/*------------ 绘图辅助函数---------------------------*/
	
	int SubPlot(int m,
		        int n,
				int p);//subplot(m, n, p)

	void MultiCurvesToSingleFig(bool bFlag);//把多条曲线画到一幅图上 bFlab为true;解除把多条曲线画到一幅图上 bFlab为false	
	
	int NewFigDlg();//产生一个新图对话框
	
	int LegendSign(vector<string> legend);//画图例，vector中的数据为各图例内容

	static const char* RanLineSpec(size_t demand = LEGEND_COLOR|LEGEND_POINT|LEGEND_LINE);//随机产生曲线特征
	
	bool SendCommand(const char *string);//参数为命令，该命令无参数


	//------------------------------------------------------
	//错误处理方法
	//----------------------------------------------------
	unsigned long GetError() { return dwLastError;}


	void test();
};

