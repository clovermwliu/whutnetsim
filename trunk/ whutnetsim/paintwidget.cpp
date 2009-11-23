#include "paintwidget.h"
#include <QtGui>

PaintWidget::PaintWidget(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{

}

PaintWidget::~PaintWidget()
{

}

void PaintWidget::paintEvent(QPaintEvent *event) {   
	QPainter painter(this);     // 建立QPainter 
	
	//绘制线条
	//painter.setPen(PenStyle(Qt::red,3,Qt::SolidLine));   // 设定笔为点线型  
	painter.drawLine(10, 10, 100, 10); // 指定x、y、width、height绘制线条       
	
	//带斜线的矩形
	painter.setBrush(QBrush(Qt::red, Qt::BDiagPattern));  // 设定笔刷为蓝色，对角线型         
	painter.drawRect(10, 20, 100, 50);   // 指定x、y、width、height绘制矩形    
	
	
	//颜色渐变的圆角矩形
	QLinearGradient gradient(50, 100, 300, 350);    // 渐近，x1,y1为起点，x2,y2为终点 
	gradient.setColorAt(0.0, Qt::white);   // 设定渐进层颜色过渡
	gradient.setColorAt(0.2, Qt::green);   
	gradient.setColorAt(1.0, Qt::black);    
	painter.setBrush(QBrush(gradient));   // 为渐进色设定笔刷   
	painter.drawRoundRect(10, 80, 100, 50);    // 绘制圆角矩形  
	painter.drawPie(10, 150, 100, 50, 45 * 16, 300 * 16);     // 绘制扇形，单位为1/16度，从45度到300度 
	
	//图片显示
	painter.drawPixmap(150, 10, QPixmap("1.png")); //图片显示

    //文字显示
	QFont font("Brush Script MT", 24);  //设定文字类型
    painter.setFont(font);              
    QTextCodec::setCodecForTr(QTextCodec::codecForName("GBK"));//中文显示
	QTextCodec *codec = QTextCodec::codecForLocale();
	painter.drawText(10,300,codec->toUnicode("Hello world!你好"));
}