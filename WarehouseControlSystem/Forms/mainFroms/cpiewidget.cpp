#include "cpiewidget.h"
#include <QDebug>

CPieWidget::CPieWidget(int w, int h, QWidget *parent)
	: QWidget(parent)
{
	
    //resize(600,500);
    this->w = w;
    this->h = h;
    this->setStyleSheet("background-color: rgb(18, 162, 111, 40);");
    //init();
}

CPieWidget::~CPieWidget()
{

}
void CPieWidget::setData( QMap<QString,float> datamap )
{
    if(m_datamap.size() == 0)
    {
        m_datamap = datamap;
        return;
    }
    for(auto it  = datamap.begin(); it != datamap.end(); ++it)
    {
        if(m_datamap.contains(it.key()) )
        {
            if(m_datamap[it.key()] != it.value())
            {
                m_datamap[it.key()] = it.value();
            }
        }
    }
}

void CPieWidget::addData( QString title,float value )
{
	m_datamap.insert(title,value);
}

void CPieWidget::removeData( QString title )
{
	m_datamap.remove(title);
}

void CPieWidget::removeAll()
{
	m_datamap.clear();
}


QMap<QString,float> CPieWidget::getDataMap()
{
	return m_datamap;

}

void CPieWidget::setColors( QList<QColor> colorlist )
{
    m_colorlist.clear();
	for (int i = colorlist.count()-1; i > -1; --i)
	{
		m_colorlist.push_front(colorlist.at(i));
	}
}


void CPieWidget::setPieStyleSheet( PieStyle piestyle /*= Default*/ )
{
	m_piestyle = piestyle;
}

void CPieWidget::setExplodedAll( bool isExploded /*= true*/ )
{
	m_isexploded = isExploded;
}

void CPieWidget::setExplodedIndex( int index )
{
	m_explodedindex = index;
}


void CPieWidget::init()
{
	setPieStyleSheet(RingShadingDount);
	m_explodedindex = -1;
	m_isexploded = false;
	




//  m_colorlist << QColor(238,187,34);
//	m_colorlist << QColor(102,170,238);
//	m_colorlist << QColor(136,68,255);

	m_colorlist << QColor(116,169,26);
    m_colorlist << QColor(120,120,120);
    m_colorlist << QColor(168,209,207);
    m_colorlist << QColor(201,64,73);
    m_colorlist << QColor(210,151,153);
    m_colorlist << QColor(77,82,175);
    m_colorlist << QColor(158,102,155);
    m_colorlist << QColor(172,126,126);
    m_colorlist << QColor(90,179,86);


//    m_datamap.insert("Empty:",3);
//    m_datamap.insert("Full:",8);
//    m_datamap.insert("Err:",2);


    int interval = w / m_datamap.size();
    int i = 0;
    for(auto it = m_datamap.begin() ;it != m_datamap.end()&&i < m_colorlist.size();it++)
    {
        QFont font("Times",11);
        QLabel *l = new QLabel(this);
        l->setText(it.key());    
        l->setFont(font);
        l->setStyleSheet("background:transparent;");
        l->move(interval*i+10,10);
        QFontMetrics fm(font);
        QRect rec = fm.boundingRect(l->text());
        int ii = rec.width();
       // l->setAttribute(Qt::WA_TranslucentBackground, true);
        QLabel *r = new QLabel(this);
       // interval = interval *0.6;
        r->move(interval*i+10+ii+10,10);
        r->setStyleSheet("background:transparent;background-color: "+ m_colorlist.at(i).name()+"");
        r->resize(w/50, w/50);
        i++;
    }
//    i = 0;
//    QLabel *r = new QLabel(this);
//    interval = interval *0.6;
//    r->move(interval*(i+1),10);
//    i++;
// //   r->setStyleSheet("background-color: rgb(18, 62, 111);");
//     r->setStyleSheet("background-color: "+ m_colorlist.at(0).name()+"");
//    //r->sets
//    QLabel *g = new QLabel(this);
//    g->move(interval*(i+1.7),10);
//    i++;
//  //  g->setStyleSheet("background-color: rgb(201,64,73);");
//      g->setStyleSheet("background-color: "+ m_colorlist.at(1).name()+"");

//    QLabel *b = new QLabel(this);
//    b->move(interval*(i+2.5),10);
//    i++;
//    //b->setStyleSheet("background-color: rgb(90,179,86);");
//    g->setStyleSheet("background-color: "+ m_colorlist.at(2).name()+"");
//    interval = w/50;
//    r->resize(interval,interval);
//    g->resize(interval,interval);
//    b->resize(interval,interval);

    m_pcheckbox = new QCheckBox(this);
    m_pcheckbox->move(w-100,h/3);
    connect(m_pcheckbox,&QCheckBox::clicked,this,&CPieWidget::onCheckClicked);
    m_pcheckbox->setText("Crack");
    m_pcheckbox->setStyleSheet("background:transparent;");
}

void CPieWidget::setCharTitle(QString title)
{
    QFont font("Times",16);
    QLabel *l = new QLabel(title,this);
    l->setFont(font);
    l->setAttribute(Qt::WA_TranslucentBackground, true);
    l->move(w/2-l->width()/2,h/2-20);
}

void CPieWidget::setSubTitle(QString sub_title)
{
    QFont font("Times",14);
    QLabel *l = new QLabel(sub_title,this);
    l->setFont(font);
    l->setAttribute(Qt::WA_TranslucentBackground, true);
    l->move(w/2-l->width()/2,h/2+20);
}


void CPieWidget::paintEvent( QPaintEvent *e )
{
    Q_UNUSED(e);
    QPainter *painter = new QPainter();
    painter->begin(this);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setRenderHint(QPainter::SmoothPixmapTransform);
    painter->setPen(QColor(200, 200, 200));
    painter->drawLine(0, 0, w - 1, 0);
    painter->drawLine(0, 0, 0, h - 1);
    painter->drawLine(w-1 , 0, w - 1, h-1 );
    painter->drawLine(0, h-1 , w - 1, h-1 );
    painter->setBrush(QColor(18, 62, 111, 40));
    painter->drawRect(0,0,w - 1,h-1);
    drawEllipse(painter);
    painter->end ();
}

void CPieWidget::drawEllipse( QPainter *painter )
{
	switch (m_piestyle)
	{
		case DefaultPie: drawDefaultPie(painter); break;
		case ConcaveShading:drawConcaveShading(painter);break;
		case RadialGradient:drawRadialGradient(painter);break;
		case DountDefault: drawDountDefault(painter);break;
		case RingShadingDount: drawRingShadingDount(painter);break;
		case ConcaveShadingDount: drawConcaveShadingDount(painter);break;
		//case ExplodedDount: drawExplodedDount(painter);break;
        default:drawDefaultPie(painter);break;
	}
}

void CPieWidget::drawDefaultPie( QPainter *painter )
{
    qreal sum = getSumValue();
    int radius = qMin(w,h)*0.8;
	QRect rect(w/2-radius/2,h/2-radius/2,radius,radius);

	painter->save();
	painter->setPen(Qt::NoPen);
    QMapIterator<QString, float> i(m_datamap);
	
    qreal index = 30;
	int colorindex=0;
	QStringList keylist = m_datamap.keys();
	for (int i = 0; i < keylist.count(); ++i)
	{
		qreal v = m_datamap.value(keylist.at(i));
		v =v/sum*(360);
		QRect newrect = rect;
		if (m_explodedindex == i || m_isexploded)
		{
			QPoint newcenter = newrect.center();
			int midangel = index+v/2;
			QPoint tp = getMovePoint(midangel);
			newcenter += tp;
			newrect.moveCenter(newcenter);
		}

		QPoint centerPoint = newrect.center();
	 	QColor firstColor = m_colorlist.at(colorindex);
	 	QRadialGradient firstGradient(centerPoint, radius/2);
 	 	firstGradient.setColorAt(0, firstColor.lighter(80));
	 	firstGradient.setColorAt(1.0, firstColor.darker(80));
	 	painter->setBrush(firstGradient);
		painter->drawPie(newrect, index * 16, v * 16);
		index+=v;
		colorindex++;
		if (colorindex==m_colorlist.count())
		{
			colorindex = 0;
		}
	}
	painter->restore();
}


void CPieWidget::drawConcaveShading( QPainter *painter )
{
	qreal sum = getSumValue();
//	int w = width();
//	int h = height();
    int radius = qMin(w,h)*0.8;
	QRect rect(w/2-radius/2,h/2-radius/2,radius,radius);

	painter->save();
	painter->setPen(Qt::NoPen);
    QMapIterator<QString, float> i(m_datamap);

    qreal index = 30;
	int colorindex=0;
	QStringList keylist = m_datamap.keys();
    for(int i = 0; i < keylist.count(); ++i)
	{
		qreal v = m_datamap.value(keylist.at(i));
		v =v/sum*(360);
		QRect newrect = rect;
		if (m_explodedindex == i || m_isexploded)
		{
			QPoint newcenter = newrect.center();
			int midangel = index+v/2;
			QPoint tp = getMovePoint(midangel);
			newcenter += tp;
			newrect.moveCenter(newcenter);
		}

		QPoint centerPoint = newrect.center();
		QColor firstColor = m_colorlist.at(colorindex);
		QRadialGradient firstGradient(centerPoint, radius/2);
		firstGradient.setColorAt(0, firstColor.darker(80));
	 	firstGradient.setColorAt(0.90, firstColor.lighter(100));
	 	firstGradient.setColorAt(1.0, firstColor.darker(150));
		painter->setBrush(firstGradient);
		painter->drawPie(newrect, index * 16, v * 16);
		index+=v;
		colorindex++;
		if (colorindex==m_colorlist.count())
		{
			colorindex = 0;
		}
	}
	painter->restore();
}

void CPieWidget::drawRadialGradient( QPainter *painter )
{
	qreal sum = getSumValue();
//	int w = width();
//	int h = height();
	int radius = qMin(w,h)*0.8; //????????
	QRect rect(w/2-radius/2,h/2-radius/2,radius,radius);

	painter->save();
	painter->setPen(Qt::NoPen);
    QMapIterator<QString, float> i(m_datamap);

	qreal index = 30;  //????????????????
	int colorindex=0;
	QStringList keylist = m_datamap.keys();
	for (int i = 0; i < keylist.count(); ++i)
	{
		qreal v = m_datamap.value(keylist.at(i));
		v =v/sum*(360);
		QRect newrect = rect;
		if (m_explodedindex == i || m_isexploded)
		{
			QPoint newcenter = newrect.center();
			int midangel = index+v/2;
			QPoint tp = getMovePoint(midangel);
			newcenter += tp;
			newrect.moveCenter(newcenter);
		}

		QPoint centerPoint = newrect.center();
		QColor firstColor = m_colorlist.at(colorindex);
		QRadialGradient firstGradient(centerPoint, radius/2);
		firstGradient.setColorAt(0, firstColor.lighter(80));
		firstGradient.setColorAt(0.61, firstColor.lighter(90));
		firstGradient.setColorAt(0.80, firstColor.lighter(100));
		firstGradient.setColorAt(1.0, firstColor.darker(150));
		painter->setBrush(firstGradient);
		painter->drawPie(newrect, index * 16, v * 16);
		index+=v;
		colorindex++;
		if (colorindex==m_colorlist.count())
		{
			colorindex = 0;
		}
	}
	painter->restore();
}

void CPieWidget::drawDountDefault( QPainter *painter )
{
	qreal sum = getSumValue();
//	int w = width();
//	int h = height();
	int radius = qMin(w,h)*0.8; //????????
	QRect rect(w/2-radius/2,h/2-radius/2,radius,radius);

	painter->save();
	painter->setPen(Qt::NoPen);
    QMapIterator<QString, float> i(m_datamap);

	qreal index = 30;  //????????????????
	int colorindex=0;
	QStringList keylist = m_datamap.keys();
	for (int i = 0; i < keylist.count(); ++i)
	{
		qreal v = m_datamap.value(keylist.at(i));
		v =v/sum*(360);
		QRect newrect = rect;
		if (m_explodedindex == i || m_isexploded)
		{
			QPoint newcenter = newrect.center();
			int midangel = index+v/2;
			QPoint tp = getMovePoint(midangel);
			newcenter += tp;
			newrect.moveCenter(newcenter);
		}
		QPoint centerPoint = newrect.center();
		QColor firstColor = m_colorlist.at(colorindex);
		QRadialGradient firstGradient(centerPoint, radius/2);
		firstGradient.setColorAt(0, Qt::transparent);
		firstGradient.setColorAt(0.6, Qt::transparent);
		firstGradient.setColorAt(0.61, firstColor.lighter(100));
		firstGradient.setColorAt(0.80, firstColor.lighter(100));
		firstGradient.setColorAt(1.0, firstColor.lighter(100));
		painter->setBrush(firstGradient);
		painter->drawPie(newrect, index * 16, v * 16);
		index+=v;
		colorindex++;
		if (colorindex==m_colorlist.count())
		{
			colorindex = 0;
		}
	}
	painter->restore();
}

void CPieWidget::drawRingShadingDount( QPainter *painter )
{
	qreal sum = getSumValue();
//	int w = width();
//	int h = height();
	int radius = qMin(w,h)*0.8; //????????
	QRect rect(w/2-radius/2,h/2-radius/2,radius,radius);

	painter->save();
	painter->setPen(Qt::NoPen);
    QMapIterator<QString, float> i(m_datamap);

	qreal index = 30;  //????????????????
	int colorindex=0;
	QStringList keylist = m_datamap.keys();
	for (int i = 0; i < keylist.count(); ++i)
	{
		qreal v = m_datamap.value(keylist.at(i));
		v =v/sum*(360);
		QRect newrect = rect;
		if (m_explodedindex == i || m_isexploded)
		{
			QPoint newcenter = newrect.center();
			int midangel = index+v/2;
			QPoint tp = getMovePoint(midangel);
			newcenter += tp;
			newrect.moveCenter(newcenter);
		}
		QPoint centerPoint = newrect.center();
		QColor firstColor = m_colorlist.at(colorindex);
		QRadialGradient firstGradient(centerPoint, radius/2);
		firstGradient.setColorAt(0, Qt::transparent);
		firstGradient.setColorAt(0.6, Qt::transparent);
		firstGradient.setColorAt(0.61, firstColor.dark(150));
		firstGradient.setColorAt(0.80, firstColor.lighter(100));
		firstGradient.setColorAt(1.0, firstColor.dark(150));
		painter->setBrush(firstGradient);
		painter->drawPie(newrect, index * 16, v * 16);
		index+=v;
		colorindex++;
		if (colorindex==m_colorlist.count())
		{
			colorindex = 0;
		}
	}
	painter->restore();
}

void CPieWidget::drawConcaveShadingDount( QPainter *painter )
{
	qreal sum = getSumValue();
//	int w = width();
//	int h = height();
    int radius = qMin(w,h)*0.8;
	QRect rect(w/2-radius/2,h/2-radius/2,radius,radius);

	painter->save();
	painter->setPen(Qt::NoPen);
    QMapIterator<QString, float> i(m_datamap);

    qreal index = 30;
	int colorindex=0;
	QStringList keylist = m_datamap.keys();
	for (int i = 0; i < keylist.count(); ++i)
	{
		qreal v = m_datamap.value(keylist.at(i));
		v =v/sum*(360);
		QRect newrect = rect;
		if (m_explodedindex == i || m_isexploded)
		{
			QPoint newcenter = newrect.center();
			int midangel = index+v/2;
			QPoint tp = getMovePoint(midangel);
			newcenter += tp;
			newrect.moveCenter(newcenter);
		}
		QPoint centerPoint = newrect.center();
		QColor firstColor = m_colorlist.at(colorindex);
		QRadialGradient firstGradient(centerPoint, radius/2);
		firstGradient.setColorAt(0, Qt::transparent);
		firstGradient.setColorAt(0.6, Qt::transparent);
		firstGradient.setColorAt(0.61, firstColor.dark(100));
		firstGradient.setColorAt(0.90, firstColor.lighter(100));
		firstGradient.setColorAt(1.0, firstColor.dark(180));
		painter->setBrush(firstGradient);
		painter->drawPie(newrect, index * 16, v * 16);
		index+=v;
		colorindex++;
		if (colorindex==m_colorlist.count())
		{
			colorindex = 0;
		}
	}
	painter->restore();
}

void CPieWidget::drawExplodedDount( QPainter *painter )
{
	qreal sum = getSumValue();
//	int w = width();
//	int h = height();
    int radius = qMin(w,h)*0.8; //
	QRect rect(w/2-radius/2,h/2-radius/2,radius,radius);

	painter->save();
	painter->setPen(Qt::NoPen);
    QMapIterator<QString, float> i(m_datamap);
	
    qreal index = 30;  //
	int colorindex = 0;

	QStringList keylist = m_datamap.keys();
	for (int i = 0; i < keylist.count(); ++i)
	{
		qreal v = m_datamap.value(keylist.at(i));
		v =v/sum*(360);
		QRect newrect = rect;
		if (m_explodedindex == i || m_isexploded)
		{
			QPoint newcenter = newrect.center();
			int midangel = index+v/2;
			QPoint tp = getMovePoint(midangel);
			newcenter += tp;
			newrect.moveCenter(newcenter);
		}

		QPoint centerPoint = newrect.center();
		QColor firstColor = m_colorlist.at(colorindex);
		QRadialGradient firstGradient(centerPoint, radius/2);
		firstGradient.setColorAt(0, Qt::transparent);
		firstGradient.setColorAt(0.6, Qt::transparent);
		firstGradient.setColorAt(0.61, firstColor.dark(150));
		firstGradient.setColorAt(0.80, firstColor.lighter(100));
		firstGradient.setColorAt(1.0, firstColor.dark(150));
		painter->setBrush(firstGradient);
		painter->drawPie(newrect, index * 16, v * 16);
		index+=v;		
		colorindex++;
		if (colorindex==m_colorlist.count())
		{
			colorindex = 0;
		}
	}

    painter->restore();
}

void CPieWidget::onCheckClicked()
{
    if(m_pcheckbox->isChecked())
       setExplodedAll(true);
    else
        setExplodedAll(false);
    update();
}

qreal CPieWidget::getSumValue()
{
	qreal sum = 0;
    QMapIterator<QString, float> i(m_datamap);
	while (i.hasNext()) {
		i.next();
		sum+= i.value();
	}
	if (sum == 0.0)
		sum = 1;
	return sum;
}

QPoint CPieWidget::getMovePoint( qreal angel )
{
	int wl = 0;
	int hl = 0;
	qreal ang = (angel)*3.14/180;
	wl = 20*qCos(ang);
	hl = 20*qSin(ang);

	if (angel > 90 && angel < 270)
		wl = qAbs(wl)*-1;
	else
		wl = qAbs(wl);

	if (angel < 180)
		hl = qAbs(hl)*-1;
	else
		hl = qAbs(hl);

	return QPoint(wl,hl);
}

