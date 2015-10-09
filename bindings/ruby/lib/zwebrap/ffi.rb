################################################################################
#  THIS FILE IS 100% GENERATED BY ZPROJECT; DO NOT EDIT EXCEPT EXPERIMENTALLY  #
#  Please refer to the README for information about making permanent changes.  #
################################################################################

require 'ffi'

require_relative 'ffi/version'


module Zwebrap
  module FFI
    extend ::FFI::Library
    
    def self.available?
      @available
    end
    
    begin
      lib_name = 'libzwebrap'
      lib_paths = ['/usr/local/lib', '/opt/local/lib', '/usr/lib64']
        .map { |path| "#{path}/#{lib_name}.#{::FFI::Platform::LIBSUFFIX}" }
      ffi_lib lib_paths + [lib_name]
      @available = true
    rescue LoadError
      warn ""
      warn "WARNING: ::Zwebrap::FFI is not available without libzwebrap."
      warn ""
      @available = false
    end
    
    if available?
      opts = {
        blocking: true  # only necessary on MRI to deal with the GIL.
      }
      
      attach_function :xrap_msg_new, [:int], :pointer, **opts
      attach_function :xrap_msg_destroy, [:pointer], :void, **opts
      attach_function :xrap_msg_print, [:pointer], :void, **opts
      attach_function :xrap_msg_decode, [:pointer], :pointer, **opts
      attach_function :xrap_msg_encode, [:pointer], :pointer, **opts
      attach_function :xrap_msg_recv, [:pointer], :pointer, **opts
      attach_function :xrap_msg_recv_nowait, [:pointer], :pointer, **opts
      attach_function :xrap_msg_send, [:pointer, :pointer], :int, **opts
      attach_function :xrap_msg_send_again, [:pointer, :pointer], :int, **opts
      attach_function :xrap_msg_id, [:pointer], :int, **opts
      attach_function :xrap_msg_set_id, [:pointer, :int], :void, **opts
      attach_function :xrap_msg_parent, [:pointer], :string, **opts
      attach_function :xrap_msg_set_parent, [:pointer, :string, :varargs], :void, **opts
      attach_function :xrap_msg_content_type, [:pointer], :string, **opts
      attach_function :xrap_msg_set_content_type, [:pointer, :string, :varargs], :void, **opts
      attach_function :xrap_msg_content_body, [:pointer], :string, **opts
      attach_function :xrap_msg_set_content_body, [:pointer, :string, :varargs], :void, **opts
      attach_function :xrap_msg_status_code, [:pointer], :pointer, **opts
      attach_function :xrap_msg_set_status_code, [:pointer, :pointer], :void, **opts
      attach_function :xrap_msg_location, [:pointer], :string, **opts
      attach_function :xrap_msg_set_location, [:pointer, :string, :varargs], :void, **opts
      attach_function :xrap_msg_etag, [:pointer], :string, **opts
      attach_function :xrap_msg_set_etag, [:pointer, :string, :varargs], :void, **opts
      attach_function :xrap_msg_date_modified, [:pointer], :pointer, **opts
      attach_function :xrap_msg_set_data_modified, [:pointer, :pointer], :void, **opts
      attach_function :xrap_msg_resource, [:pointer], :string, **opts
      attach_function :xrap_msg_set_resource, [:pointer, :string, :varargs], :void, **opts
      attach_function :xrap_msg_parameters_string, [:pointer, :string, :string], :string, **opts
      attach_function :xrap_msg_parameters_insert, [:pointer, :string, :string, :varargs], :void, **opts
      attach_function :xrap_msg_if_modified_since, [:pointer], :pointer, **opts
      attach_function :xrap_msg_set_if_modified_since, [:pointer, :pointer], :void, **opts
      attach_function :xrap_msg_if_none_match, [:pointer], :string, **opts
      attach_function :xrap_msg_set_if_none_match, [:pointer, :string, :varargs], :void, **opts
      attach_function :xrap_msg_metadata_string, [:pointer, :string, :string], :string, **opts
      attach_function :xrap_msg_metadata_insert, [:pointer, :string, :string, :varargs], :void, **opts
      attach_function :xrap_msg_if_unmodified_since, [:pointer], :pointer, **opts
      attach_function :xrap_msg_set_if_unmodified_since, [:pointer, :pointer], :void, **opts
      attach_function :xrap_msg_if_match, [:pointer], :string, **opts
      attach_function :xrap_msg_set_if_match, [:pointer, :string, :varargs], :void, **opts
      attach_function :xrap_msg_status_text, [:pointer], :string, **opts
      attach_function :xrap_msg_set_status_text, [:pointer, :string, :varargs], :void, **opts
      attach_function :xrap_msg_test, [:bool], :void, **opts
      
      require_relative 'ffi/xrap_msg'
      
      attach_function :zwr_client_new, [], :pointer, **opts
      attach_function :zwr_client_destroy, [:pointer], :void, **opts
      attach_function :zwr_client_actor, [:pointer], :pointer, **opts
      attach_function :zwr_client_msgpipe, [:pointer], :pointer, **opts
      attach_function :zwr_client_connected, [:pointer], :bool, **opts
      attach_function :zwr_client_connect, [:pointer, :string, :pointer, :string], :int, **opts
      attach_function :zwr_client_set_handler, [:pointer, :string, :string], :int, **opts
      attach_function :zwr_client_request, [:pointer, :pointer, :pointer], :int, **opts
      attach_function :zwr_client_deliver, [:pointer, :pointer, :pointer], :int, **opts
      attach_function :zwr_client_recv, [:pointer], :pointer, **opts
      attach_function :zwr_client_command, [:pointer], :string, **opts
      attach_function :zwr_client_status, [:pointer], :int, **opts
      attach_function :zwr_client_reason, [:pointer], :string, **opts
      attach_function :zwr_client_sender, [:pointer], :pointer, **opts
      attach_function :zwr_client_content, [:pointer], :pointer, **opts
      attach_function :zwr_client_test, [:bool], :void, **opts
      
      require_relative 'ffi/zwr_client'
    end
  end
end

################################################################################
#  THIS FILE IS 100% GENERATED BY ZPROJECT; DO NOT EDIT EXCEPT EXPERIMENTALLY  #
#  Please refer to the README for information about making permanent changes.  #
################################################################################
