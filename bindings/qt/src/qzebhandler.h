/*
################################################################################
#  THIS FILE IS 100% GENERATED BY ZPROJECT; DO NOT EDIT EXCEPT EXPERIMENTALLY  #
#  Please refer to the README for information about making permanent changes.  #
################################################################################
*/
#ifndef Q_ZEB_HANDLER_H
#define Q_ZEB_HANDLER_H

#include "qzwebrap.h"

class QT_ZWEBRAP_EXPORT QZebHandler : public QObject
{
    Q_OBJECT
public:

    //  Copy-construct to return the proper wrapped c types
    QZebHandler (zeb_handler_t *self, QObject *qObjParent = 0);

    //  Create a new zeb_handler
    explicit QZebHandler (const QString &endpoint, QObject *qObjParent = 0);

    //  Destroy the zeb_handler
    ~QZebHandler ();

    //  Add a new offer this handler will handle. Returns 0 if successful,
    //  otherwise -1.                                                     
    int addOffer (int method, const QString &uri);

    //  Add a new accept type that this handler can deliver. May be a regular
    //  expression. Returns 0 if successfull, otherwise -1.                  
    int addAccept (const QString &accept);

    //  Set a callback handler to handle incoming requests. Returns the response
    //  to be send back to the client.                                          
    void setHandleRequestFn (zeb_handler_handle_request_fn *handleRequestFn);

    //  Set a callback handler to check if provided etag matches the current one.
    //  Returns true if etags match, otherwise false.                            
    void setCheckEtagFn (zeb_handler_check_etag_fn *checkEtagFn);

    //  Set a callback handler to check if provided last_modified timestamp matches
    //  the current one. Returns true if timestamp match, otherwise false.         
    void setCheckLastModifiedFn (zeb_handler_check_last_modified_fn *lastModifiedFn);

    //  Self test of this class.
    static void test (bool verbose);

    zeb_handler_t *self;
};
#endif //  Q_ZEB_HANDLER_H
/*
################################################################################
#  THIS FILE IS 100% GENERATED BY ZPROJECT; DO NOT EDIT EXCEPT EXPERIMENTALLY  #
#  Please refer to the README for information about making permanent changes.  #
################################################################################
*/
