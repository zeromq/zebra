################################################################################
#  THIS FILE IS 100% GENERATED BY ZPROJECT; DO NOT EDIT EXCEPT EXPERIMENTALLY  #
#  Please refer to the README for information about making permanent changes.  #
################################################################################

module Zwebrap
  module FFI

    # Handler for XRAP requests
    class ZebHandler
      class DestroyedError < RuntimeError; end

      # Boilerplate for self pointer, initializer, and finalizer
      class << self
        alias :__new :new
      end
      def initialize ptr, finalize=true
        @ptr = ptr
        if @ptr.null?
          @ptr = nil # Remove null pointers so we don't have to test for them.
        elsif finalize
          @finalizer = self.class.create_finalizer_for @ptr
          ObjectSpace.define_finalizer self, @finalizer
        end
      end
      def self.create_finalizer_for ptr
        Proc.new do
          ptr_ptr = ::FFI::MemoryPointer.new :pointer
          ptr_ptr.write_pointer ptr
          ::Zwebrap::FFI.zeb_handler_destroy ptr_ptr
        end
      end
      def null?
        !@ptr or @ptr.null?
      end
      # Return internal pointer
      def __ptr
        raise DestroyedError unless @ptr
        @ptr
      end
      # So external Libraries can just pass the Object to a FFI function which expects a :pointer
      alias_method :to_ptr, :__ptr
      # Nullify internal pointer and return pointer pointer
      def __ptr_give_ref
        raise DestroyedError unless @ptr
        ptr_ptr = ::FFI::MemoryPointer.new :pointer
        ptr_ptr.write_pointer @ptr
        ObjectSpace.undefine_finalizer self if @finalizer
        @finalizer = nil
        @ptr = nil
        ptr_ptr
      end

      # Create a new callback of the following type:
      # Handle an incomming XRAP request. Callee keeps ownership of the request.
      #     typedef xrap_msg_t * (zeb_handler_handle_request_fn) (
      #         xrap_msg_t *xrequest);                            
      #
      # WARNING: If your Ruby code doesn't retain a reference to the
      #   FFI::Function object after passing it to a C function call,
      #   it may be garbage collected while C still holds the pointer,
      #   potentially resulting in a segmentation fault.
      def self.handle request fn
        ::FFI::Function.new :pointer, [:pointer], blocking: true do |xrequest|
          xrequest = XrapMsg.__new xrequest, false
          result = yield xrequest
          result = result.__ptr if result
          result
        end
      end

      # Create a new callback of the following type:
      # Checks if the request etag matches our current one. Returns true if etags
      # match, otherwise false.                                                  
      #     typedef bool (zeb_handler_check_etag_fn) (
      #         const char *etag);                    
      #
      # WARNING: If your Ruby code doesn't retain a reference to the
      #   FFI::Function object after passing it to a C function call,
      #   it may be garbage collected while C still holds the pointer,
      #   potentially resulting in a segmentation fault.
      def self.check etag fn
        ::FFI::Function.new :bool, [:string], blocking: true do |etag|
          result = yield etag
          result = !(0==result||!result) # boolean
          result
        end
      end

      # Create a new callback of the following type:
      # Checks if the request last modified timestamp matches our current one.
      # Returns true if timestamps match, otherwise false.                    
      #     typedef bool (zeb_handler_check_last_modified_fn) (
      #         const uint64_t last_modified);                 
      #
      # WARNING: If your Ruby code doesn't retain a reference to the
      #   FFI::Function object after passing it to a C function call,
      #   it may be garbage collected while C still holds the pointer,
      #   potentially resulting in a segmentation fault.
      def self.check last modified fn
        ::FFI::Function.new :bool, [:pointer], blocking: true do |last_modified|
          result = yield last_modified
          result = !(0==result||!result) # boolean
          result
        end
      end

      # Create a new zeb_handler
      def self.new(endpoint)
        endpoint = String(endpoint)
        ptr = ::Zwebrap::FFI.zeb_handler_new(endpoint)
        __new ptr
      end

      # Destroy the zeb_handler
      def destroy()
        return unless @ptr
        self_p = __ptr_give_ref
        result = ::Zwebrap::FFI.zeb_handler_destroy(self_p)
        result
      end

      # Add a new offer this handler will handle. Returns 0 if successful,
      # otherwise -1.                                                     
      def add_offer(method, uri)
        raise DestroyedError unless @ptr
        self_p = @ptr
        method = Integer(method)
        uri = String(uri)
        result = ::Zwebrap::FFI.zeb_handler_add_offer(self_p, method, uri)
        result
      end

      # Add a new accept type that this handler can deliver. May be a regular
      # expression. Returns 0 if successfull, otherwise -1.                  
      def add_accept(accept)
        raise DestroyedError unless @ptr
        self_p = @ptr
        accept = String(accept)
        result = ::Zwebrap::FFI.zeb_handler_add_accept(self_p, accept)
        result
      end

      # Set a callback handler to handle incoming requests. Returns the response
      # to be send back to the client.                                          
      def set_handle_request_fn(handle_request_fn)
        raise DestroyedError unless @ptr
        self_p = @ptr
        result = ::Zwebrap::FFI.zeb_handler_set_handle_request_fn(self_p, handle_request_fn)
        result
      end

      # Set a callback handler to check if provided etag matches the current one.
      # Returns true if etags match, otherwise false.                            
      def set_check_etag_fn(check_etag_fn)
        raise DestroyedError unless @ptr
        self_p = @ptr
        result = ::Zwebrap::FFI.zeb_handler_set_check_etag_fn(self_p, check_etag_fn)
        result
      end

      # Set a callback handler to check if provided last_modified timestamp matches
      # the current one. Returns true if timestamp match, otherwise false.         
      def set_check_last_modified_fn(last_modified_fn)
        raise DestroyedError unless @ptr
        self_p = @ptr
        result = ::Zwebrap::FFI.zeb_handler_set_check_last_modified_fn(self_p, last_modified_fn)
        result
      end

      # Self test of this class.
      def self.test(verbose)
        verbose = !(0==verbose||!verbose) # boolean
        result = ::Zwebrap::FFI.zeb_handler_test(verbose)
        result
      end
    end
  end
end

################################################################################
#  THIS FILE IS 100% GENERATED BY ZPROJECT; DO NOT EDIT EXCEPT EXPERIMENTALLY  #
#  Please refer to the README for information about making permanent changes.  #
################################################################################
