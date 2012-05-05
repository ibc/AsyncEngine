module AsyncEngine

  def self.open_udp_socket_ip4 bind_ip, bind_port, handler=AsyncEngine::UDPSocket, *args
    raise AsyncEngine::Error, "handler must be AE::UDPSocket or a subclass" unless
      handler <= AsyncEngine::UDPSocket

    sock = handler.allocate
    sock.send :ae_init, :ipv4, bind_ip, bind_port
    sock.on_init *args
    block_given? and yield sock
    sock
  end


  class UDPSocket
    # Don't allow the user to init a class instance.
    class << self
      private :new
    end

    def ae_init ip_type, bind_ip=nil, bind_port=nil
      @_ip_type = ip_type
      @_bind_ip = bind_ip
      @_bind_port = bind_port  # TODO: si es 0 será random y hay que saber cual es!
    end
    private :ae_init

    def ip_type   ; @_ip_type   ; end
    def bind_ip   ; @_bind_ip   ; end
    def bind_port ; @_bind_port ; end

    # This method should be rewriten by the user in the inherited class.
    def on_init *args
      puts "DBG: on_init() => args = #{args.inspect}"
    end
  end

end
