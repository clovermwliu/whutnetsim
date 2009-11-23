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
	QPainter painter(this);     // ����QPainter 
	
	//��������
	//painter.setPen(PenStyle(Qt::red,3,Qt::SolidLine));   // �趨��Ϊ������  
	painter.drawLine(10, 10, 100, 10); // ָ��x��y��width��height��������       
	
	//��б�ߵľ���
	painter.setBrush(QBrush(Qt::red, Qt::BDiagPattern));  // �趨��ˢΪ��ɫ���Խ�����         
	painter.drawRect(10, 20, 100, 50);   // ָ��x��y��width��height���ƾ���    
	
	
	//��ɫ�����Բ�Ǿ���
	QLinearGradient gradient(50, 100, 300, 350);    // ������x1,y1Ϊ��㣬x2,y2Ϊ�յ� 
	gradient.setColorAt(0.0, Qt::white);   // �趨��������ɫ����
	gradient.setColorAt(0.2, Qt::green);   
	gradient.setColorAt(1.0, Qt::black);    
	painter.setBrush(QBrush(gradient));   // Ϊ����ɫ�趨��ˢ   
	painter.drawRoundRect(10, 80, 100, 50);    // ����Բ�Ǿ���  
	painter.drawPie(10, 150, 100, 50, 45 * 16, 300 * 16);     // �������Σ���λΪ1/16�ȣ���45�ȵ�300�� 
	
	//ͼƬ��ʾ
	painter.drawPixmap(150, 10, QPixmap("1.png")); //ͼƬ��ʾ

    //������ʾ
	QFont font("Brush Script MT", 24);  //�趨��������
    painter.setFont(font);              
    QTextCodec::setCodecForTr(QTextCodec::codecForName("GBK"));//������ʾ
	QTextCodec *codec = QTextCodec::codecForLocale();
	painter.drawText(10,300,codec->toUnicode("Hello world!���"));
}