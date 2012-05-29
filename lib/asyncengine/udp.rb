module AsyncEngine

  def self.open_udp_socket bind_ip, bind_port, klass=AsyncEngine::UDPSocket, *args
    ensure_ready_for_handles()

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


  class UDPSocket < Handle
    def ip_type
      @_ip_type
    end

    alias :receiving= :set_receiving
    alias :sending= :set_sending
    #alias :broadcast= :set_broadcast

    alias orig_to_s to_s
    def to_s
      ip, port = local_address()
      if alive?
        "#{orig_to_s} (#{@_ip_type} : #{ip} : #{port})"
      else
        "#{orig_to_s} (#{@_ip_type} : #{ip} : #{port}) (not alive)"
      end
    end
    alias :inspect :to_s


    private

    def on_datagram_received datagram, src_ip, src_port
      puts "#{inspect}: received datagram from #{src_ip} : #{src_port}: #{datagram.inspect}"
    end

    class << self
      private :new
    end
  end  # class UDPSocket

end
