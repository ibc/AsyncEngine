module AsyncEngine

#   def self.open_udp_socket_ip4 bind_ip=nil, bind_port=nil, udp_options={}, klass=AsyncEngine::UDPSocket, *args, &block
#     _open_udp_socket nil, bind_ip || "0.0.0.0", bind_port || 0, udp_options, klass, *args, &block
#   end
# 
#   def self.open_udp_socket_ip6 bind_ip=nil, bind_port=nil, udp_options={}, klass=AsyncEngine::UDPSocket, *args, &block
#     _open_udp_socket true, bind_ip || "::0", bind_port || 0, udp_options, klass, *args, &block
#   end

  def self.open_udp_socket bind_ip, bind_port, klass=AsyncEngine::UDPSocket, *args
    raise AsyncEngine::Error, "klass must inherit from AsyncEngine::UDPSocket" unless
      klass <= AsyncEngine::UDPSocket

    # First allocate a handler instance.
    sock = klass.allocate

    # Set the UV handler.
    unless (ret = sock.send :_c_init_udp_socket, bind_ip, bind_port) == true
      raise AsyncEngine.get_uv_error(ret)
    end

    # Call the usual initialize() method as defined by the user.
    sock.send :initialize, *args

    # Run the given block.
    block_given? and yield sock

    # TODO: Falta meterlo en un hash no?

    # Return the klass instance.
    sock
  end

  class << self
    #private :_open_udp_socket
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

    class << self
      private :new
    end
  end  # class UDPSocket

end
