#ifndef CAS_H
#define CAS_H

#include <QObject>
#include <QTcpSocket>
#include <QDateTime>

class CASChan : public QObject
{
	Q_OBJECT
public:
	CASChan(const QString name, QObject* parent);
	QString getName(){return name;}

	void setVal(const QString v);
	void setUnits(const QString u){units=u;}
	void setDescr(const QString d){descr=d;}
	void setDateTime(const QString t);
	void setDateTime(); // Установить текущее время.

	QString getVal(){return val;}
	QString getUnits(){return units;}
	QString getDescr(){return descr;}
	QDateTime getDateTime(){return dateTime;}

private:
	QString name, val, units, descr;
	QDateTime dateTime;

signals:
	void updateSignal(QString);
	void updateSignal(int);
	void updateSignal();
};

// =================================================

class CAS : public QObject
{
	Q_OBJECT
public:
	CAS(const QString host, const int port, QObject *parent=0);

	void setReconnectTime(const int rc);
	int getReconnectTime(){return reconnectTime;}
	CASChan* addChan(const QString name);
	~CAS();

	QString toString();
	void sendMap(QMap<QString, QString>);
	void setChan(const QString &name, const QString &val);
	void setChan(const QString &name, const double &val);

protected:
	void subscr(const QString name);

private:
	QString host;
	int port;
	int reconnectTime;
	QString lexSep;

	QTcpSocket *casSock;
	QMap<QString, CASChan*> chMap;

	bool is_reconnect;

private slots:
	void reconnect();
	void connectedSlot();
	void socketErrorSlot(QAbstractSocket::SocketError socketError);
	void readSlot();

signals:
	void stateSignal(bool ok);
	void readSignal();

};

#endif // CAS_H
