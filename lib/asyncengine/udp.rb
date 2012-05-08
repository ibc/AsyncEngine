module AsyncEngine

  def self.open_udp_socket_ip4 bind_ip=nil, bind_port=nil, udp_options={}, klass=AsyncEngine::UDPSocketDefaultClass, *args, &block
    _open_udp_socket nil, bind_ip || "0.0.0.0", bind_port || 0, udp_options, klass, *args, &block
  end

  def self.open_udp_socket_ip6 bind_ip=nil, bind_port=nil, udp_options={}, klass=AsyncEngine::UDPSocketDefaultClass, *args, &block
    _open_udp_socket true, bind_ip || "::0", bind_port || 0, udp_options, klass, *args, &block
  end

  def self._open_udp_socket is_ipv6, bind_ip, bind_port, udp_options, klass, *args
    raise AsyncEngine::Error, "klass must include AsyncEngine::UDPSocket module" unless
      klass.include? AsyncEngine::UDPSocket

    # First allocate a handler instance.
    sock = klass.allocate

    # Set the UV handler.
    unless (ret = sock.send :_c_init_udp_socket, is_ipv6, bind_ip, bind_port) == true
      raise AsyncEngine.get_uv_error(ret)
    end

    # Initiate instance attributes.
    sock.instance_variable_set :@_ip_type, ( is_ipv6 ? :ipv6 : :ipv4 )
    sock.instance_variable_set :@_bind_ip, bind_ip
    sock.instance_variable_set :@_bind_port, bind_port

    # Call the usual initialize() method as defined by the user.
    sock.send :initialize, *args

    # Run the given block.
    block_given? and yield sock

    # TODO: Falta meterlo en un hash no?

    # Return the klass instance.
    sock
  end

  class << self
    private :_open_udp_socket
  end


  module UDPSocket
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
  end  # module UDPSocket


  class UDPSocketDefaultClass
    include AsyncEngine::UDPSocket
  end  # class UDPSocketDefaultClass

end
