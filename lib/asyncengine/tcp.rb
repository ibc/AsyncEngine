module AsyncEngine

  def self.connect_tcp dest_ip, dest_port, klass=AsyncEngine::TCPSocket, *args
    ensure_ready_for_handles()

    raise AsyncEngine::Error, "klass must inherit from AsyncEngine::TCPSocket" unless
      klass <= AsyncEngine::TCPSocket

    # First allocate a handler instance.
    sock = klass.allocate

    # Set the UV TCP handler and connect.  TODO: Sure connect now?
    sock.send :uv_handle_init, dest_ip, dest_port

    # Call the usual initialize() method as defined by the user.
    sock.send :initialize, *args

    # Run the given block.
    yield sock  if block_given?

    # Return the klass instance.
    sock
  end


  class TCPSocket < Handle
    def ip_type
      @_ip_type
    end

    def on_connected
      puts "#{inspect}: connected"
    end

    def on_connection_error error
      puts "#{inspect}: connection error: #{error.inspect}"
    end

    def on_data_received data
      puts "#{inspect}: received data from #{peer_address()}: #{datagram.inspect}"
    end

    class << self
      private :new
    end
  end  # class TCPSocket

end
