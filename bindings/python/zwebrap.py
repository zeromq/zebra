################################################################################
#  THIS FILE IS 100% GENERATED BY ZPROJECT; DO NOT EDIT EXCEPT EXPERIMENTALLY  #
#  Please refer to the README for information about making permanent changes.  #
################################################################################

from __future__ import print_function
import os, sys
from ctypes import *
from ctypes.util import find_library

# zwebrap
try:
    # If LD_LIBRARY_PATH or your OSs equivalent is set, this is the only way to
    # load the library.  If we use find_library below, we get the wrong result.
    if os.name == 'posix':
        if sys.platform == 'darwin':
            lib = cdll.LoadLibrary('libzwebrap.0.dylib')
        else:
            lib = cdll.LoadLibrary("libzwebrap.so.0")
    elif os.name == 'nt':
        lib = cdll.LoadLibrary('libzwebrap.dll')
except OSError:
    libpath = find_library("zwebrap")
    if not libpath:
        raise ImportError("Unable to find libzwebrap")
    lib = cdll.LoadLibrary(libpath)

class xrap_msg_t(Structure):
    pass # Empty - only for type checking
xrap_msg_p = POINTER(xrap_msg_t)

class zmsg_t(Structure):
    pass # Empty - only for type checking
zmsg_p = POINTER(zmsg_t)

class number_t(Structure):
    pass # Empty - only for type checking
number_p = POINTER(number_t)

class zwr_client_t(Structure):
    pass # Empty - only for type checking
zwr_client_p = POINTER(zwr_client_t)

class zactor_t(Structure):
    pass # Empty - only for type checking
zactor_p = POINTER(zactor_t)

class zsock_t(Structure):
    pass # Empty - only for type checking
zsock_p = POINTER(zsock_t)

class zuuid_t(Structure):
    pass # Empty - only for type checking
zuuid_p = POINTER(zuuid_t)


# xrap msg
lib.xrap_msg_new.restype = xrap_msg_p
lib.xrap_msg_new.argtypes = [c_int]
lib.xrap_msg_destroy.restype = None
lib.xrap_msg_destroy.argtypes = [POINTER(xrap_msg_p)]
lib.xrap_msg_print.restype = None
lib.xrap_msg_print.argtypes = [xrap_msg_p]
lib.xrap_msg_decode.restype = xrap_msg_p
lib.xrap_msg_decode.argtypes = [POINTER(zmsg_p)]
lib.xrap_msg_encode.restype = zmsg_p
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
lib.xrap_msg_set_data_modified.restype = None
lib.xrap_msg_set_data_modified.argtypes = [xrap_msg_p, number_p]
lib.xrap_msg_resource.restype = c_char_p
lib.xrap_msg_resource.argtypes = [xrap_msg_p]
lib.xrap_msg_set_resource.restype = None
lib.xrap_msg_set_resource.argtypes = [xrap_msg_p, c_char_p]
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

    def print(self):
        """Print properties of the xrap msg object."""
        return lib.xrap_msg_print(self._as_parameter_)

    @staticmethod
    def decode(msg_p):
        """Parse a xrap_msg from zmsg_t. Returns a new object, or NULL if
the message could not be parsed, or was NULL. Destroys msg and
nullifies the msg reference."""
        return lib.xrap_msg_decode(byref(zmsg_p.from_param(msg_p)))

    @staticmethod
    def encode(self_p):
        """Encode xrap_msg into zmsg and destroy it. Returns a newly created
object or NULL if error. Use when not in control of sending the message."""
        return lib.xrap_msg_encode(byref(xrap_msg_p.from_param(self_p)))

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

    def set_data_modified(self, data_modified):
        """"""
        return lib.xrap_msg_set_data_modified(self._as_parameter_, data_modified)

    def resource(self):
        """Get/set the resource field"""
        return lib.xrap_msg_resource(self._as_parameter_)

    def set_resource(self, format, *args):
        """"""
        return lib.xrap_msg_set_resource(self._as_parameter_, format, *args)

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

    def metadata_string(self, key, default_value):
        """Get/set the parameters field"""
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


# zwr_client
lib.zwr_client_new.restype = zwr_client_p
lib.zwr_client_new.argtypes = []
lib.zwr_client_destroy.restype = None
lib.zwr_client_destroy.argtypes = [POINTER(zwr_client_p)]
lib.zwr_client_actor.restype = zactor_p
lib.zwr_client_actor.argtypes = [zwr_client_p]
lib.zwr_client_msgpipe.restype = zsock_p
lib.zwr_client_msgpipe.argtypes = [zwr_client_p]
lib.zwr_client_connected.restype = c_bool
lib.zwr_client_connected.argtypes = [zwr_client_p]
lib.zwr_client_connect.restype = c_int
lib.zwr_client_connect.argtypes = [zwr_client_p, c_char_p, number_p, c_char_p]
lib.zwr_client_set_handler.restype = c_int
lib.zwr_client_set_handler.argtypes = [zwr_client_p, c_char_p, c_char_p]
lib.zwr_client_request.restype = c_int
lib.zwr_client_request.argtypes = [zwr_client_p, number_p, POINTER(zmsg_p)]
lib.zwr_client_deliver.restype = c_int
lib.zwr_client_deliver.argtypes = [zwr_client_p, zuuid_p, POINTER(zmsg_p)]
lib.zwr_client_recv.restype = zmsg_p
lib.zwr_client_recv.argtypes = [zwr_client_p]
lib.zwr_client_command.restype = c_char_p
lib.zwr_client_command.argtypes = [zwr_client_p]
lib.zwr_client_status.restype = c_int
lib.zwr_client_status.argtypes = [zwr_client_p]
lib.zwr_client_reason.restype = c_char_p
lib.zwr_client_reason.argtypes = [zwr_client_p]
lib.zwr_client_sender.restype = zuuid_p
lib.zwr_client_sender.argtypes = [zwr_client_p]
lib.zwr_client_content.restype = zmsg_p
lib.zwr_client_content.argtypes = [zwr_client_p]
lib.zwr_client_test.restype = None
lib.zwr_client_test.argtypes = [c_bool]

class ZwrClient(object):
    """ZWebRap Client"""

    def __init__(self, *args):
        """Create a new zwr_client, return the reference if successful, or NULL
if construction failed due to lack of available memory."""
        if len(args) == 2 and type(args[0]) is c_void_p and isinstance(args[1], bool):
            self._as_parameter_ = cast(args[0], zwr_client_p) # Conversion from raw type to binding
            self.allow_destruct = args[1] # This is a 'fresh' value, owned by us
        elif len(args) == 2 and type(args[0]) is zwr_client_p and isinstance(args[1], bool):
            self._as_parameter_ = args[0] # Conversion from raw type to binding
            self.allow_destruct = args[1] # This is a 'fresh' value, owned by us
        else:
            assert(len(args) == 0)
            self._as_parameter_ = lib.zwr_client_new() # Creation of new raw type
            self.allow_destruct = True

    def __del__(self):
        """Destroy the zwr_client and free all memory used by the object."""
        if self.allow_destruct:
            lib.zwr_client_destroy(byref(self._as_parameter_))

    def __bool__(self):
        "Determine whether the object is valid by converting to boolean" # Python 3
        return self._as_parameter_.__bool__()

    def __nonzero__(self):
        "Determine whether the object is valid by converting to boolean" # Python 2
        return self._as_parameter_.__nonzero__()

    def actor(self):
        """Return actor, when caller wants to work with multiple actors and/or
input sockets asynchronously."""
        return lib.zwr_client_actor(self._as_parameter_)

    def msgpipe(self):
        """Return message pipe for asynchronous message I/O. In the high-volume case,
we send methods and get replies to the actor, in a synchronous manner, and
we send/recv high volume message data to a second pipe, the msgpipe. In
the low-volume case we can do everything over the actor pipe, if traffic
is never ambiguous."""
        return lib.zwr_client_msgpipe(self._as_parameter_)

    def connected(self):
        """Return true if client is currently connected, else false. Note that the
client will automatically re-connect if the server dies and restarts after
a successful first connection."""
        return lib.zwr_client_connected(self._as_parameter_)

    def connect(self, endpoint, timeout, address):
        """Connect to server endpoint, with specified timeout in msecs (zero means wait
forever). Constructor succeeds if connection is successful. The caller may
specify its address.
Returns >= 0 if successful, -1 if interrupted."""
        return lib.zwr_client_connect(self._as_parameter_, endpoint, timeout, address)

    def set_handler(self, method, route):
        """Offer to handle particular XRAP requests, where the route matches request's
resource.
Returns >= 0 if successful, -1 if interrupted."""
        return lib.zwr_client_set_handler(self._as_parameter_, method, route)

    def request(self, timeout, content_p):
        """No explanation
Returns >= 0 if successful, -1 if interrupted."""
        return lib.zwr_client_request(self._as_parameter_, timeout, byref(zmsg_p.from_param(content_p)))

    def deliver(self, sender, content_p):
        """Send XRAP DELIVER message to server, takes ownership of message
and destroys message when done sending it."""
        return lib.zwr_client_deliver(self._as_parameter_, sender, byref(zmsg_p.from_param(content_p)))

    def recv(self):
        """Receive message from server; caller destroys message when done"""
        return lib.zwr_client_recv(self._as_parameter_)

    def command(self):
        """Return last received command. Can be one of these values:
    "XRAP DELIVER""""
        return lib.zwr_client_command(self._as_parameter_)

    def status(self):
        """Return last received status"""
        return lib.zwr_client_status(self._as_parameter_)

    def reason(self):
        """Return last received reason"""
        return lib.zwr_client_reason(self._as_parameter_)

    def sender(self):
        """Return last received sender"""
        return lib.zwr_client_sender(self._as_parameter_)

    def content(self):
        """Return last received content"""
        return lib.zwr_client_content(self._as_parameter_)

    @staticmethod
    def test(verbose):
        """Self test of this class."""
        return lib.zwr_client_test(verbose)

################################################################################
#  THIS FILE IS 100% GENERATED BY ZPROJECT; DO NOT EDIT EXCEPT EXPERIMENTALLY  #
#  Please refer to the README for information about making permanent changes.  #
################################################################################
