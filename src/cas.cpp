#include "cas.h"

#include <QDebug>
#include <QTimer>
#include <QStringList>

QString TIME_FMT ("dd.MM.yyyy hh_mm_ss.zzz");

CASChan::CASChan(const QString n, QObject *parent) : QObject(parent), name(n)
{
    //qDebug() << name << "created"; // << this;
}

void CASChan::setVal(const QString v)
{
	val = v;
	//qDebug() << name << val;

	emit updateSignal(val);
	emit updateSignal(val.toInt());
	emit updateSignal();
}

void CASChan::setDateTime(const QString t)
{
	dateTime=QDateTime::fromString(t, TIME_FMT);
}

void CASChan::setDateTime()
{
	dateTime=QDateTime::currentDateTime();
}
// =================================================

//QMap<QString, CASChan*> CAS::chMap;

CAS::CAS(const QString h, const int p, QObject *parent)
	: QObject(parent), host(h), port(p), reconnectTime(1000), lexSep("|"), is_reconnect(false)
{
	casSock = new QTcpSocket(this);

	connect(casSock, SIGNAL(connected()), SLOT(connectedSlot()));
	connect(casSock, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(socketErrorSlot(QAbstractSocket::SocketError)));
	connect(casSock, SIGNAL(readyRead()), SLOT(readSlot()));

	casSock->connectToHost(host, port);
}

CAS::~CAS()
{
    //qDebug() << "Socket" << toString() << "destroyed";
}

// ------- ????????? --------------------
void CAS::reconnect ()
{
    // qDebug() << "Trying to recoonect to " << toString();
	casSock->connectToHost(host, port);
}

void CAS::connectedSlot()
{
    //qDebug() << "Socket " << toString() << "connected.";
	emit stateSignal(true);

	if(is_reconnect)
	{
		// ?????????? ?? ??? ?????? (??? ???????????????)
		QList<QString> s_l(chMap.keys());
		QStringListIterator itr(s_l);
		while(itr.hasNext()) subscr(itr.next());

		is_reconnect= false;
	}
}

void CAS::socketErrorSlot(QAbstractSocket::SocketError socketError)
{
    //qDebug() << "Socket " << toString() << "error:" << socketError << "Starting to reconnect.";
	emit stateSignal(false);
	is_reconnect = true;
	QTimer::singleShot(reconnectTime, this, SLOT(reconnect()));
}

void CAS::setReconnectTime(const int rc)
{
	reconnectTime = rc;
}

void CAS::sendMap(QMap<QString, QString> m)
{
	if(!casSock->isValid()) return;
	QStringList s_l;
	QMapIterator<QString, QString> itr(m);
	while(itr.hasNext())
	{
		itr.next();
		s_l << itr.key()+":"+itr.value();
	}
	QTextStream os(casSock);
	os << s_l.join(lexSep)+'\n';
}

void CAS::setChan(const QString &name, const QString &val)
{
	QMap<QString, QString> m;
	m["name"]=name;
	m["method"]="set";
	m["val"]=val;
	sendMap(m);
}

void CAS::setChan(const QString &name, const double &val)
{
	setChan(name, QString::number(val, 'f', 6));
}

void CAS::readSlot()
{
	while(casSock->canReadLine())
	{
		QString line(tr(casSock->readLine()).remove('\n'));
		// qDebug() << line;

		QMap<QString, QString> map;
		QStringList s_l(line.split(lexSep, QString::SkipEmptyParts));
		QStringListIterator itr(s_l);
		while(itr.hasNext())
		{
			QString lexem(itr.next());
			map[lexem.section(':',0,0)] = lexem.section(':',1,1);
		}

		if (map.value("method") == "error") qDebug() << line;
		// ?????? ?????. ?? ????, ????????? ?? ?? ?????, ?.?.????????? ?????? ?? ???????? ??????.
		CASChan *ch = chMap.value(map.value("name"));
		if(ch!=0)
		{
			if (map.contains("descr")) ch->setDescr(map.value("descr"));
			if (map.contains("units")) ch->setUnits(map.value("units"));
			if (map.contains("time")) ch->setDateTime(map.value("time"));
			else ch->setDateTime(); // ???????? ????? ??-????????? (?.?. ???????)
			if (map.contains("val")) ch->setVal(map.value("val")); // ? ??? ?????? ????? ????????? ??????...
		}
		qDebug() << "CAS:" << ch->getName() << ch->getVal();
	}
	emit readSignal();
}

// ---------------------------------------

QString CAS::toString()
{
	return host+QString(":%1").arg(port);
}

CASChan* CAS::addChan(const QString name)
{
//	qDebug() << "CASChan:" << name;

	if(chMap.find(name)==chMap.end()) // ?????? ??? - ???????? ???.
	{
		chMap[name] = new CASChan(name, this);
		subscr(name);// ?????????? ?? ????.
	}
	// ? ????? ??????, ?????? ????????? ?? ??? ???????????? ??? ????? ?????????.
	return chMap.value(name);
}

void CAS::subscr(const QString name)
{
//	qDebug() << name << "Subscription";
	QMap<QString, QString> map;
	map["name"] = name;
	map["method"] = "subscr";
	sendMap(map);
}
