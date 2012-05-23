module AsyncEngine

  def self.open_udp_socket bind_ip, bind_port, klass=AsyncEngine::UDPSocket, *args
    ensure_AE_is_ready_for_handles()

    raise AsyncEngine::Error, "klass must inherit from AsyncEngine::UDPSocket" unless
      klass <= AsyncEngine::UDPSocket

    # First allocate a handler instance.
    sock = klass.allocate

    # Set the UV UDP handler and bind.
    sock.send :uv_handle_init, bind_ip, bind_port

    # Call the usual initialize() method as defined by the user.
    sock.send :initialize, *args

    # Run the given block.
    yield sock  if block_given?

    # Return the klass instance.
    sock
  end


  class UDPSocket
    def ip_type
      @_ip_type
    end

    def bind_ip
      @_bind_ip
    end
    alias :local_ip :bind_ip

    def bind_port
      @_bind_port
    end
    alias :local_port :bind_port

    alias orig_to_s to_s
    def to_s
      "#{self.orig_to_s} (#{@_ip_type} : #{@_bind_ip} : #{@_bind_port})"
    end
    alias :inspect :to_s

    def on_received_datagram datagram
      puts "INFO: received datagram: #{datagram.inspect}"
    end

    class << self
      private :new
    end
  end  # class UDPSocket

end
