#include "qzwrhandler.h"

QZwrHandler::QZwrHandler(QString endpoint, QObject *parent) : QThread(parent)
{
   this->_terminated = false;
   this->_endpoint = endpoint;
   this->_defaultVersion = "v1";
}

void QZwrHandler::initHandler() {
   this->_offers = getOffers();

   //  Setup accepted formats
   this->_acceptFormats = getAcceptFormats();
   QString formatString;
   foreach (QString format, this->_acceptFormats) {
      format.isEmpty() ?
               formatString.append(format) :
               format.append("|").append(format);
   }
   QString regexString  =
         QString("application\\/(?:(%1)|vnd.eckelmann.ci.(v\\d+)\\+(%1))")
            .arg(formatString);
   this->_acceptRegex = QRegExp(regexString);
}

bool QZwrHandler::resolveAccept(QString accept)
{
   if (this->_acceptRegex.exactMatch(accept)) {
      if (!this->_acceptRegex.cap(1).isEmpty()) {
         _responseVersion = this->_defaultVersion;
         _responseFormat = this->_acceptRegex.cap(1);
      }
      else {
         _responseVersion = this->_acceptRegex.cap(2);
         _responseFormat = this->_acceptRegex.cap(3);
      }
   }
   else {
      return false;
   }
   return true;
}

int QZwrHandler::resolveOffer(xrap_msg_t *xrequest)
{
   QString route;
   switch (xrap_msg_id (xrequest)) {
      case XRAP_MSG_GET:
      case XRAP_MSG_PUT:
      case XRAP_MSG_DELETE:
         route = QString(xrap_msg_resource (xrequest));
         break;
      case XRAP_MSG_POST:
         route = QString(xrap_msg_location (xrequest));
         break;
   }
   foreach (QZwrOffer offer, this->_offers) {
      if (QRegExp(offer.route).exactMatch(route))
         return offer.offerId;
   }
   return -1;
}

QString QZwrHandler::buildLinks (QString uri, int current, int last, int per_page)
{
   QString result;
   QString format("<%1?page=%2&per_page=%3>; rel=\"%4\"%5");
   if (current > 1) {
      result.append(QString(format).arg(uri).arg(1).arg(per_page).arg("first").arg(", "));
      if (current < last)
         result.append(QString(format).arg(uri).arg(current - 1).arg(per_page).arg("prev").arg(", "));
      else
         result.append(QString(format).arg(uri).arg(current - 1).arg(per_page).arg("prev").arg(""));
   }
   if (current < last) {
      result.append(QString(format).arg(uri).arg(current + 1).arg(per_page).arg("next").arg(", "));
      result.append(QString(format).arg(uri).arg(last).arg(per_page).arg("last").arg(""));
   }
   return result;
}

void QZwrHandler::run()
{
   initHandler();
   int rc;
   //  Connect and register handler at dispatcher
   QZwrClient *handler = new QZwrClient();
   rc = handler->connect(this->_endpoint, 1000, "alm_handler");
   assert (rc == 0);

   foreach (const QZwrOffer offer, this->_offers) {
      rc = handler->setHandler(offer.method, offer.route);
      assert (rc == 0);
   }

   //  Setup poller to peek a incoming traffic
   QZpoller *poller = new QZpoller(zpoller_new(handler->msgpipe()->self, NULL));
   while (!this->_terminated) {
      if (!poller->wait(1000)) {
         continue;   //  Timeout triggered or interupted
      }
      QZmsg *request = handler->recv();
      xrap_msg_t *xrequest = xrap_msg_decode (&request->self);

      //  Check if accept is supported
      QString accept (xrap_msg_content_type (xrequest));
      if (!resolveAccept(accept)) {
         xrap_msg_t *xresponse = xrap_msg_new (XRAP_MSG_ERROR);
         xrap_msg_set_status_code (xresponse, 406);
         xrap_msg_set_status_text (xresponse, "Accept type not supported!");
         zmsg_t *zresponse = xrap_msg_encode (&xresponse);
         QZmsg *response = new QZmsg (zresponse);
         handler->deliver(handler->sender(), response);
         continue;
      }

      _xresponse = xrap_msg_new (XRAP_MSG_ERROR);
      QZhash(xrap_msg_metadata(_xresponse)).autofree();
      int offerId = resolveOffer(xrequest);
      if (offerId >= 0 && processRequest(offerId)) {
         //  Send xrap response
         xrap_msg_set_content_type (_xresponse, QString("application/%1")
                                                .arg(_responseFormat)
                                                .toUtf8()
                                                .data());
      }
      zmsg_t *zresponse = xrap_msg_encode (&_xresponse);
      QZmsg *response = new QZmsg (zresponse);
      handler->deliver(handler->sender(), response);

      delete handler->sender();
      delete request;
      delete response;
   }
   delete handler;
}

void QZwrHandler::stop ()
{
   this->_terminated = true;
}

void QZwrHandler::setResponseData(QString &data)
{
   xrap_msg_set_content_body(_xresponse, data.toUtf8().data());
}

void QZwrHandler::setResponseStatusCode(int code)
{
   xrap_msg_set_status_code (_xresponse, code);
}

void QZwrHandler::setResponseEtag(QString &etag)
{
   xrap_msg_set_etag (_xresponse, etag.toUtf8().data());
}

void QZwrHandler::setResponseMediaType (QString &key, QString &value)
{
   xrap_msg_metadata_insert(_xresponse,
                            key.toUtf8().data(),
                            value.toUtf8().data());
}

void QZwrHandler::setResponseCollectionLinks(QString &links)
{
   xrap_msg_metadata_insert(_xresponse,
                            "Links",
                            links.toUtf8().data());
}

void QZwrHandler::setResponseCollectionSize(size_t size)
{
   xrap_msg_metadata_insert(_xresponse,
                            "X-Total-Size",
                            QString("%1").arg(size).toUtf8().data());
}
