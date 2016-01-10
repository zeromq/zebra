################################################################################
#  THIS FILE IS 100% GENERATED BY ZPROJECT; DO NOT EDIT EXCEPT EXPERIMENTALLY  #
#  Please refer to the README for information about making permanent changes.  #
################################################################################

from __future__ import print_function
import os, sys
from ctypes import *
from ctypes.util import find_library
import czmq

# zebra
try:
    # If LD_LIBRARY_PATH or your OSs equivalent is set, this is the only way to
    # load the library.  If we use find_library below, we get the wrong result.
    if os.name == 'posix':
        if sys.platform == 'darwin':
            lib = cdll.LoadLibrary('libzebra.0.dylib')
        else:
            lib = cdll.LoadLibrary("libzebra.so.0")
    elif os.name == 'nt':
        lib = cdll.LoadLibrary('libzebra.dll')
except OSError:
    libpath = find_library("zebra")
    if not libpath:
        raise ImportError("Unable to find libzebra")
    lib = cdll.LoadLibrary(libpath)

class xrap_msg_t(Structure):
    pass # Empty - only for type checking
xrap_msg_p = POINTER(xrap_msg_t)

class number_t(Structure):
    pass # Empty - only for type checking
number_p = POINTER(number_t)


# zeb_handler
lib.zeb_handler_add_offer.restype = c_int
lib.zeb_handler_add_offer.argtypes = [czmq.zactor_p, c_int, c_char_p]
lib.zeb_handler_add_accept.restype = c_int
lib.zeb_handler_add_accept.argtypes = [czmq.zactor_p, c_char_p]
lib.zeb_handler_test.restype = None
lib.zeb_handler_test.argtypes = [c_bool]

class ZebHandler(object):
    """Handler for XRAP requests"""

    allow_destruct = False
    def __bool__(self):
        "Determine whether the object is valid by converting to boolean" # Python 3
        return self._as_parameter_.__bool__()

    def __nonzero__(self):
        "Determine whether the object is valid by converting to boolean" # Python 2
        return self._as_parameter_.__nonzero__()

    @staticmethod
    def add_offer(self, method, uri):
        """Add a new offer this handler will handle. Returns 0 if successful,
otherwise -1."""
        return lib.zeb_handler_add_offer(self, method, uri)

    @staticmethod
    def add_accept(self, accept):
        """Add a new accept type that this handler can deliver. May be a regular
expression. Returns 0 if successfull, otherwise -1."""
        return lib.zeb_handler_add_accept(self, accept)

    @staticmethod
    def test(verbose):
        """Self test of this class."""
        return lib.zeb_handler_test(verbose)


# xrap msg
lib.xrap_msg_new.restype = xrap_msg_p
lib.xrap_msg_new.argtypes = [c_int]
lib.xrap_msg_destroy.restype = None
lib.xrap_msg_destroy.argtypes = [POINTER(xrap_msg_p)]
lib.xrap_msg_decode.restype = xrap_msg_p
lib.xrap_msg_decode.argtypes = [POINTER(czmq.zmsg_p)]
lib.xrap_msg_encode.restype = czmq.zmsg_p
lib.xrap_msg_encode.argtypes = [POINTER(xrap_msg_p)]
lib.xrap_msg_recv.restype = xrap_msg_p
lib.xrap_msg_recv.argtypes = [c_void_p]
lib.xrap_msg_recv_nowait.restype = xrap_msg_p
lib.xrap_msg_recv_nowait.argtypes = [c_void_p]
lib.xrap_msg_send.restype = c_int
lib.xrap_msg_send.argtypes = [POINTER(xrap_msg_p), c_void_p]
lib.xrap_msg_send_again.restype = c_int
lib.xrap_msg_send_again.argtypes = [xrap_msg_p, c_void_p]
lib.xrap_msg_id.restype = c_int
lib.xrap_msg_id.argtypes = [xrap_msg_p]
lib.xrap_msg_set_id.restype = None
lib.xrap_msg_set_id.argtypes = [xrap_msg_p, c_int]
lib.xrap_msg_parent.restype = c_char_p
lib.xrap_msg_parent.argtypes = [xrap_msg_p]
lib.xrap_msg_set_parent.restype = None
lib.xrap_msg_set_parent.argtypes = [xrap_msg_p, c_char_p]
lib.xrap_msg_content_type.restype = c_char_p
lib.xrap_msg_content_type.argtypes = [xrap_msg_p]
lib.xrap_msg_set_content_type.restype = None
lib.xrap_msg_set_content_type.argtypes = [xrap_msg_p, c_char_p]
lib.xrap_msg_content_body.restype = c_char_p
lib.xrap_msg_content_body.argtypes = [xrap_msg_p]
lib.xrap_msg_set_content_body.restype = None
lib.xrap_msg_set_content_body.argtypes = [xrap_msg_p, c_char_p]
lib.xrap_msg_status_code.restype = number_p
lib.xrap_msg_status_code.argtypes = [xrap_msg_p]
lib.xrap_msg_set_status_code.restype = None
lib.xrap_msg_set_status_code.argtypes = [xrap_msg_p, number_p]
lib.xrap_msg_location.restype = c_char_p
lib.xrap_msg_location.argtypes = [xrap_msg_p]
lib.xrap_msg_set_location.restype = None
lib.xrap_msg_set_location.argtypes = [xrap_msg_p, c_char_p]
lib.xrap_msg_etag.restype = c_char_p
lib.xrap_msg_etag.argtypes = [xrap_msg_p]
lib.xrap_msg_set_etag.restype = None
lib.xrap_msg_set_etag.argtypes = [xrap_msg_p, c_char_p]
lib.xrap_msg_date_modified.restype = number_p
lib.xrap_msg_date_modified.argtypes = [xrap_msg_p]
lib.xrap_msg_set_date_modified.restype = None
lib.xrap_msg_set_date_modified.argtypes = [xrap_msg_p, number_p]
lib.xrap_msg_resource.restype = c_char_p
lib.xrap_msg_resource.argtypes = [xrap_msg_p]
lib.xrap_msg_set_resource.restype = None
lib.xrap_msg_set_resource.argtypes = [xrap_msg_p, c_char_p]
lib.xrap_msg_parameters.restype = czmq.zhash_p
lib.xrap_msg_parameters.argtypes = [xrap_msg_p]
lib.xrap_msg_get_parameters.restype = czmq.zhash_p
lib.xrap_msg_get_parameters.argtypes = [xrap_msg_p]
lib.xrap_msg_set_parameters.restype = None
lib.xrap_msg_set_parameters.argtypes = [xrap_msg_p, POINTER(czmq.zhash_p)]
lib.xrap_msg_parameters_string.restype = c_char_p
lib.xrap_msg_parameters_string.argtypes = [xrap_msg_p, c_char_p, c_char_p]
lib.xrap_msg_parameters_insert.restype = None
lib.xrap_msg_parameters_insert.argtypes = [xrap_msg_p, c_char_p, c_char_p]
lib.xrap_msg_if_modified_since.restype = number_p
lib.xrap_msg_if_modified_since.argtypes = [xrap_msg_p]
lib.xrap_msg_set_if_modified_since.restype = None
lib.xrap_msg_set_if_modified_since.argtypes = [xrap_msg_p, number_p]
lib.xrap_msg_if_none_match.restype = c_char_p
lib.xrap_msg_if_none_match.argtypes = [xrap_msg_p]
lib.xrap_msg_set_if_none_match.restype = None
lib.xrap_msg_set_if_none_match.argtypes = [xrap_msg_p, c_char_p]
lib.xrap_msg_metadata.restype = czmq.zhash_p
lib.xrap_msg_metadata.argtypes = [xrap_msg_p]
lib.xrap_msg_get_metadata.restype = czmq.zhash_p
lib.xrap_msg_get_metadata.argtypes = [xrap_msg_p]
lib.xrap_msg_set_metadata.restype = None
lib.xrap_msg_set_metadata.argtypes = [xrap_msg_p, POINTER(czmq.zhash_p)]
lib.xrap_msg_metadata_string.restype = c_char_p
lib.xrap_msg_metadata_string.argtypes = [xrap_msg_p, c_char_p, c_char_p]
lib.xrap_msg_metadata_insert.restype = None
lib.xrap_msg_metadata_insert.argtypes = [xrap_msg_p, c_char_p, c_char_p]
lib.xrap_msg_if_unmodified_since.restype = number_p
lib.xrap_msg_if_unmodified_since.argtypes = [xrap_msg_p]
lib.xrap_msg_set_if_unmodified_since.restype = None
lib.xrap_msg_set_if_unmodified_since.argtypes = [xrap_msg_p, number_p]
lib.xrap_msg_if_match.restype = c_char_p
lib.xrap_msg_if_match.argtypes = [xrap_msg_p]
lib.xrap_msg_set_if_match.restype = None
lib.xrap_msg_set_if_match.argtypes = [xrap_msg_p, c_char_p]
lib.xrap_msg_status_text.restype = c_char_p
lib.xrap_msg_status_text.argtypes = [xrap_msg_p]
lib.xrap_msg_set_status_text.restype = None
lib.xrap_msg_set_status_text.argtypes = [xrap_msg_p, c_char_p]
lib.xrap_msg_test.restype = None
lib.xrap_msg_test.argtypes = [c_bool]

class XrapMsg(object):
    """    XRAP serialization over ZMTP

/*  These are the xrap_msg messages:

    POST - Create a new, dynamically named resource in some parent.
        parent              string      Schema/type/name
        content_type        string      Content type
        content_body        longstr     New resource specification

    POST_OK - Success response for POST.
        status_code         number 2    Response status code 2xx
        location            string      Schema/type/name
        etag                string      Opaque hash tag
        date_modified       number 8    Date and time modified
        content_type        string      Content type
        content_body        longstr     Resource contents

    GET - Retrieve a known resource.
        resource            string      Schema/type/name
        if_modified_since   number 8    GET if more recent
        if_none_match       string      GET if changed
        content_type        string      Desired content type

    GET_OK - Success response for GET.
        status_code         number 2    Response status code 2xx
        content_type        string      Actual content type
        content_body        longstr     Resource specification

    GET_EMPTY - Conditional GET returned 304 Not Modified.
        status_code         number 2    Response status code 3xx

    PUT - Update a known resource.
        resource            string      Schema/type/name
        if_unmodified_since  number 8   Update if same date
        if_match            string      Update if same ETag
        content_type        string      Content type
        content_body        longstr     New resource specification

    PUT_OK - Success response for PUT.
        status_code         number 2    Response status code 2xx
        location            string      Schema/type/name
        etag                string      Opaque hash tag
        date_modified       number 8    Date and time modified

    DELETE - Remove a known resource.
        resource            string      schema/type/name
        if_unmodified_since  number 8   DELETE if same date
        if_match            string      DELETE if same ETag

    DELETE_OK - Success response for DELETE.
        status_code         number 2    Response status code 2xx

    ERROR - Error response for any request.
        status_code         number 2    Response status code, 4xx or 5xx
        status_text         string      Response status text
*/"""

    POST = 1 # 
    POST OK = 2 # 
    GET = 3 # 
    GET OK = 4 # 
    GET EMPTY = 5 # 
    PUT = 6 # 
    PUT OK = 7 # 
    DELETE = 8 # 
    DELETE OK = 9 # 
    ERROR = 10 # 
    allow_destruct = False
    def __init__(self, *args):
        """Create a new xrap_msg"""
        if len(args) == 2 and type(args[0]) is c_void_p and isinstance(args[1], bool):
            self._as_parameter_ = cast(args[0], xrap_msg_p) # Conversion from raw type to binding
            self.allow_destruct = args[1] # This is a 'fresh' value, owned by us
        elif len(args) == 2 and type(args[0]) is xrap_msg_p and isinstance(args[1], bool):
            self._as_parameter_ = args[0] # Conversion from raw type to binding
            self.allow_destruct = args[1] # This is a 'fresh' value, owned by us
        else:
            assert(len(args) == 1)
            self._as_parameter_ = lib.xrap_msg_new(args[0]) # Creation of new raw type
            self.allow_destruct = True

    def __del__(self):
        """Destroy the xrap_msg"""
        if self.allow_destruct:
            lib.xrap_msg_destroy(byref(self._as_parameter_))

    def __bool__(self):
        "Determine whether the object is valid by converting to boolean" # Python 3
        return self._as_parameter_.__bool__()

    def __nonzero__(self):
        "Determine whether the object is valid by converting to boolean" # Python 2
        return self._as_parameter_.__nonzero__()

    @staticmethod
    def decode(msg_p):
        """Parse a xrap_msg from zmsg_t. Returns a new object, or NULL if
the message could not be parsed, or was NULL. Destroys msg and
nullifies the msg reference."""
        return lib.xrap_msg_decode(byref(czmq.zmsg_p.from_param(msg_p)))

    @staticmethod
    def encode(xrap_msg_p):
        """Encode xrap_msg into zmsg and destroy it. Returns a newly created
object or NULL if error. Use when not in control of sending the message."""
        return czmq.Zmsg(lib.xrap_msg_encode(byref(xrap_msg_p.from_param(xrap_msg_p))), False)

    @staticmethod
    def recv(input):
        """Receive and parse a xrap_msg from the socket. Returns new object,
or NULL if error. Will block if there's no message waiting."""
        return lib.xrap_msg_recv(input)

    @staticmethod
    def recv_nowait(input):
        """Receive and parse a xrap_msg from the socket. Returns new object,
or NULL either if there was no input waiting, or the recv was interrupted."""
        return lib.xrap_msg_recv_nowait(input)

    @staticmethod
    def send(self_p, output):
        """Send the xrap_msg to the output, and destroy it"""
        return lib.xrap_msg_send(byref(xrap_msg_p.from_param(self_p)), output)

    def send_again(self, output):
        """Send the xrap_msg to the output, and destroy it"""
        return lib.xrap_msg_send_again(self._as_parameter_, output)

    def id(self):
        """Get the xrap_msg id and printable command"""
        return lib.xrap_msg_id(self._as_parameter_)

    def set_id(self, id):
        """"""
        return lib.xrap_msg_set_id(self._as_parameter_, id)

    def parent(self):
        """Get/set the parent field"""
        return lib.xrap_msg_parent(self._as_parameter_)

    def set_parent(self, format, *args):
        """"""
        return lib.xrap_msg_set_parent(self._as_parameter_, format, *args)

    def content_type(self):
        """Get/set the content_type field"""
        return lib.xrap_msg_content_type(self._as_parameter_)

    def set_content_type(self, format, *args):
        """"""
        return lib.xrap_msg_set_content_type(self._as_parameter_, format, *args)

    def content_body(self):
        """Get/set the parent field"""
        return lib.xrap_msg_content_body(self._as_parameter_)

    def set_content_body(self, format, *args):
        """"""
        return lib.xrap_msg_set_content_body(self._as_parameter_, format, *args)

    def status_code(self):
        """Get/set the status_code field"""
        return lib.xrap_msg_status_code(self._as_parameter_)

    def set_status_code(self, status_code):
        """"""
        return lib.xrap_msg_set_status_code(self._as_parameter_, status_code)

    def location(self):
        """Get/set the location field"""
        return lib.xrap_msg_location(self._as_parameter_)

    def set_location(self, format, *args):
        """"""
        return lib.xrap_msg_set_location(self._as_parameter_, format, *args)

    def etag(self):
        """Get/set the etag field"""
        return lib.xrap_msg_etag(self._as_parameter_)

    def set_etag(self, format, *args):
        """"""
        return lib.xrap_msg_set_etag(self._as_parameter_, format, *args)

    def date_modified(self):
        """Get/set the date_modified field"""
        return lib.xrap_msg_date_modified(self._as_parameter_)

    def set_date_modified(self, date_modified):
        """"""
        return lib.xrap_msg_set_date_modified(self._as_parameter_, date_modified)

    def resource(self):
        """Get/set the resource field"""
        return lib.xrap_msg_resource(self._as_parameter_)

    def set_resource(self, format, *args):
        """"""
        return lib.xrap_msg_set_resource(self._as_parameter_, format, *args)

    def parameters(self):
        """//  Get/set the parameters field"""
        return czmq.Zhash(lib.xrap_msg_parameters(self._as_parameter_), False)

    def get_parameters(self):
        """//  Get the parameters field and transfer ownership to caller"""
        return czmq.Zhash(lib.xrap_msg_get_parameters(self._as_parameter_), False)

    def set_parameters(self, parameters_p):
        """"""
        return lib.xrap_msg_set_parameters(self._as_parameter_, byref(czmq.zhash_p.from_param(parameters_p)))

    def parameters_string(self, key, default_value):
        """Get/set the parameters field"""
        return lib.xrap_msg_parameters_string(self._as_parameter_, key, default_value)

    def parameters_insert(self, key, format, *args):
        """"""
        return lib.xrap_msg_parameters_insert(self._as_parameter_, key, format, *args)

    def if_modified_since(self):
        """Get/set the if_modified_since field"""
        return lib.xrap_msg_if_modified_since(self._as_parameter_)

    def set_if_modified_since(self, if_modified_since):
        """"""
        return lib.xrap_msg_set_if_modified_since(self._as_parameter_, if_modified_since)

    def if_none_match(self):
        """Get/set the if_none_match field"""
        return lib.xrap_msg_if_none_match(self._as_parameter_)

    def set_if_none_match(self, format, *args):
        """"""
        return lib.xrap_msg_set_if_none_match(self._as_parameter_, format, *args)

    def metadata(self):
        """//  Get/set the metadata field"""
        return czmq.Zhash(lib.xrap_msg_metadata(self._as_parameter_), False)

    def get_metadata(self):
        """//  Get the metadata field and transfer ownership to caller"""
        return czmq.Zhash(lib.xrap_msg_get_metadata(self._as_parameter_), False)

    def set_metadata(self, metadata_p):
        """"""
        return lib.xrap_msg_set_metadata(self._as_parameter_, byref(czmq.zhash_p.from_param(metadata_p)))

    def metadata_string(self, key, default_value):
        """Get/set a value in the metadata dictionary"""
        return lib.xrap_msg_metadata_string(self._as_parameter_, key, default_value)

    def metadata_insert(self, key, format, *args):
        """"""
        return lib.xrap_msg_metadata_insert(self._as_parameter_, key, format, *args)

    def if_unmodified_since(self):
        """Get/set the if_unmodified_since field"""
        return lib.xrap_msg_if_unmodified_since(self._as_parameter_)

    def set_if_unmodified_since(self, if_unmodified_since):
        """"""
        return lib.xrap_msg_set_if_unmodified_since(self._as_parameter_, if_unmodified_since)

    def if_match(self):
        """Get/set the if_match field"""
        return lib.xrap_msg_if_match(self._as_parameter_)

    def set_if_match(self, format, *args):
        """"""
        return lib.xrap_msg_set_if_match(self._as_parameter_, format, *args)

    def status_text(self):
        """Get/set the status_text field"""
        return lib.xrap_msg_status_text(self._as_parameter_)

    def set_status_text(self, format, *args):
        """"""
        return lib.xrap_msg_set_status_text(self._as_parameter_, format, *args)

    @staticmethod
    def test(verbose):
        """Self test of this class."""
        return lib.xrap_msg_test(verbose)

################################################################################
#  THIS FILE IS 100% GENERATED BY ZPROJECT; DO NOT EDIT EXCEPT EXPERIMENTALLY  #
#  Please refer to the README for information about making permanent changes.  #
################################################################################