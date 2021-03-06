#include "storeitem.h"
#include <QDebug>
#include <QEvent>
#include <QHelpEvent>
#include <QToolTip>
#include <QScrollBar>
#define MIN(x,y)    ((x) < (y) ? (x) : (y))
StoreItem::StoreItem(double StartX, double StartY, double StopX, double StopY, const Qt::PenStyle &LineStyle, \
                     const int LineWidth, const QColor &LineColor, const QColor &BackColor):\
    QGraphicsRectItem(StartX,StartY,StopX,StopY/*,LineStyle,LineWidth,LineColor,BackColor*/)
{
    m_StartX = StartX;
    m_StartY = StartY;
    m_StopX = StopX;
    m_StopY =StopY;
    m_state = 0;
    m_BackGroundColor = BackColor;
    m_text = tr("");
    m_FontColor =QColor(255,255,255);
    m_LineWidth = LineWidth;
    // 画笔 - 边框色
    p.setWidth(LineWidth);
    p.setStyle(LineStyle);
    p.setColor(LineColor);
    // 画刷 - 背景色
    setBrush(QColor(255,0,0));
    // 可选择
    //setFlags(QGraphicsItem::ItemIsSelectable);
    m_font.setPointSize(4);  //字体大小
    m_font.setBold(false);    //字体加粗
    m_font.setFamily("宋体"); //设置字体样式
    textItem = new QGraphicsTextItem(this);
    setAcceptHoverEvents(true);
    m_Timer = new QTimer();
    connect(m_Timer,&QTimer::timeout,this,&StoreItem::UpdateState);
   // m_Timer->start(200);
   // setToolTip(m_text);
    setCursor(Qt::OpenHandCursor);    //改变光标形状,光标变为了手型
}

StoreItem::~StoreItem()
{
    m_Timer->stop();
    delete m_Timer;
    delete textItem;
    deleteLater();
}
///
/// \brief StoreItem::AddTextItem
/// \param text
/// 添加文字信息
void StoreItem::AddTextItem(QString text)
{
    textItem->setPlainText(text);
    textItem->setTextWidth(this->boundingRect().width());  //设置编辑框宽度
    textItem->setDefaultTextColor(QColor(255,255,255));   //设置字体颜色
    //qDebug()<<this->boundingRect().width();
    //文字居中
    QTextBlockFormat format;
    format.setAlignment(Qt::AlignCenter);  //设置字体居中
    QTextCursor cursor = textItem->textCursor();
    cursor.select(QTextCursor::Document);
    cursor.mergeBlockFormat(format);
    cursor.clearSelection();
    textItem->setTextCursor(cursor);
    //设置item放置位置
    // QPointF p =this->pos();
    QPointF pos(100,100);
    // qDebug()<<pos.rx() <<pos.ry() << p.rx() << p.ry() <<this->boundingRect().height() <<this->boundingRect().width() ;
    textItem->setPos(pos);
    textItem->setFont(m_font);
}

void StoreItem::SetText(QString text)
{
    m_text = text;
    setToolTip(text);
    update();
}
///
/// \brief StoreItem::SetStoreSate
/// \param state
///仓位状态更新背景颜色的变化
void StoreItem::SetStoreSate(char state)//
{
    m_state = state;
    switch (m_state) {
    case 0://空
        m_BackGroundColor = QColor(160,160,160);
        break;
    case 1://占用
        m_BackGroundColor = QColor(0,200,0);
        break;
    case 2://锁定
        m_BackGroundColor = QColor(200,0,0);
        break;
    case 3://未定义
        m_BackGroundColor = QColor(34,180,180);
        break;
    case 9://未定义
        m_BackGroundColor = QColor(220,220,0);
        break;
    case 10:
        m_BackGroundColor = QColor(173 ,216, 230);
    default:
        break;
    }
    update();
}

void StoreItem::SetIndexID(QString ID)
{
    m_ID = ID;
}

QString StoreItem::GetIndexID()
{
    return m_ID;
}

QString StoreItem::GetText()
{
    return m_text;
}

char StoreItem::GetState()
{
    return m_state;
}

void StoreItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {

    //设置画笔 边框颜色和背景色
    painter->setPen(p);
    painter->setBrush(m_BackGroundColor);
    painter->setRenderHint(QPainter::Antialiasing, true);
   //绘制图形
    Q_UNUSED(option);
    QStyleOptionGraphicsItem op;
    op.initFrom(widget);
    double Width = m_StopX - m_StartX;        // 绘图区域的宽度
    double Height = m_StopY - m_StartY;       // 绘图区域的高度
    double CenterX = m_StartX + Width / 2;   // 绘图区域的中心X

    double Roundness = MIN(Width, Height) * 0.1; //圆角
    QLinearGradient lg1(CenterX, m_StartY, CenterX, m_StopY);
    lg1.setColorAt(0, GetNewColor(m_BackGroundColor,-20));
    lg1.setColorAt(0.4, m_BackGroundColor);
    lg1.setColorAt(1, GetNewColor(m_BackGroundColor,20));
    painter->setBrush(lg1);
    QPainterPath OutLine;
    OutLine.addRoundedRect(m_StartX, m_StartY, Width, Height, Roundness, Roundness);
    painter->drawPath(OutLine);
    painter->fillPath(OutLine, lg1);
    //文字
    painter->setPen(QPen(QBrush(m_FontColor),2.0));
    painter->setFont(m_font);
    painter->drawText(m_StartX, m_StartY, Width, Height, Qt::AlignVCenter | Qt::AlignHCenter |Qt::TextWordWrap, m_text);
}

void StoreItem::UpdateState()
{
    //    //*************测试
    //    if(m_state == 0)
    //    {
    //        SetStoreSate(1);
    //    }
    //    else  if(m_state == 1){
    //        SetStoreSate(2);
    //    }
    //    else  if(m_state == 2){
    //        SetStoreSate(3);
    //    }
    //    else  if(m_state == 3){
    //        SetStoreSate(0);
    //    }
    //    //*************测试
   // update();
   // setPos();
}

QColor StoreItem::GetNewColor(const QColor &Source, int Value)
{
    int r = Source.red() + Value;
    int g = Source.green() + Value;
    int b = Source.blue() + Value;
    int a = Source.alpha();

    if(r > 255)
    {
        r = 255;
    }
    else if(r < 0)
    {
        r = 0;
    }

    if(g > 255)
    {
        g = 255;
    }
    else if(g < 0)
    {
        g = 0;
    }

    if(b > 255)
    {
        b = 255;
    }
    else if(b < 0)
    {
        b = 0;
    }

    return QColor(r, g, b, a);
}
//int itemAt(const QPoint &point);
//bool StoreItem::event(QEvent *event)
//{
//    if (event->type() == QEvent::ToolTip) {
//    QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);

//    QPoint p = helpEvent->pos();
//    // 获取水平拖动条
//    QScrollBar* pHorizontalScrollBar = horizontalScrollBar();
//    // 获取垂直拖动条
//    QScrollBar* pVerticalScrollBar = verticalScrollBar();
//    // 拖过滚动条得到视图中鼠标点坐标
//    QPoint point(pHorizontalScrollBar->value() + p.x(), pVerticalScrollBar->value() + p.y());

//    //itemAt(const QPoint &point); 这里随便写的一个函数，通过坐标的得到视图中的项信息
//    int index = itemAt(point);
//    if (index != -1) {
//       QString strToolTip;

//       QToolTip::showText(QCursor::pos(), m_text);
//    }
//    else
//       QToolTip::hideText();
//    return QWidget::event(event);
//}

//void StoreItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
//{
//    qDebug()<<"id"<<m_ID << "hoverEnterEvent" << m_text << m_StartX <<m_StartY << m_StopX <<m_StopY;
//   //if (event->type() == QEvent::ToolTip)
//    {
//        QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
//       QString strToolTip ;
//        QToolTip::showText(QCursor::pos(), m_text);
//    }
//}

//void StoreItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
//{
//    qDebug()<<"id"<<m_ID << "hoverLeaveEvent";
//    QToolTip::hideText();
//}

QRectF StoreItem::boundingRect() const//一定要重写此函数，用来响应图形的有效区域 比如鼠标事件
{
    QRectF mRect(m_StartX, m_StartY, m_StopX - m_StartX, m_StopY - m_StartY);

    const int padding = (m_LineWidth + 1) / 2;
    mRect.adjust(-padding, -padding, +padding, +padding);

    return mRect;
}
