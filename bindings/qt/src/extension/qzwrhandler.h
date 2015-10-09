#ifndef QZWRHANDLER_H
#define QZWRHANDLER_H

#include <QObject>
#include <QThread>
#include <QString>
#include <QSet>
#include <QRegExp>
#include <QFile>
#include <QCoreApplication>
#include <qczmq.h>
#include "qzwebrap.h"

class QZwrOffer
{
public:
   int offerId;
   QString method;
   QString route;
};

class QZwrHandler : public QThread
{
   Q_OBJECT
public:
   explicit QZwrHandler(QString endpoint, QObject *parent = 0);

   void stop();

signals:

public slots:
   void run();

protected:
   virtual QSet<QString> getAcceptFormats() = 0;
   virtual QSet<QZwrOffer> getOffers() = 0;
   virtual bool processRequest(int offerId) = 0;

   void setResponseData(QString &data);
   void setResponseStatusCode(int code);
   void setResponseEtag(QString &etag);
   void setResponseMediaType (QString &key, QString &value);
   void setResponseCollectionLinks(QString &links);
   void setResponseCollectionSize(size_t size);

   static  QString buildLinks (QString offer, int current, int last, int per_page);
   QString responseFormat();
   QString responseVersion();

private:
   void initHandler();

   int resolveOffer(xrap_msg_t *xrequest);
   bool resolveAccept (QString accept);

   bool _terminated;
   QString _endpoint;

   QString _defaultVersion;
   QSet<QString> _acceptFormats;
   QSet<QZwrOffer> _offers;
   QRegExp _acceptRegex;
   /* Response Data */
   xrap_msg_t *_xresponse;
   QString _responseFormat;
   QString _responseVersion;
};

#endif // QZWRHANDLER_H
