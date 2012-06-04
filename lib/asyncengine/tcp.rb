module AsyncEngine

  def self.tcp_connect dest_ip, dest_port, bind_ip=nil, bind_port=nil, klass=AsyncEngine::TCPSocket
    klass.new dest_ip, dest_port, bind_ip, bind_port
  end

  class TCPSocket < Handle
    private

    def on_connected
      puts "#{inspect}: connected: local address: #{local_address().inspect}, peer address: #{peer_address().inspect}"
    end

    def on_connection_error error
      puts "#{inspect}: connection error: #{error.inspect}"
    end

    def on_data_received data
      puts "#{inspect}: received data: #{data.inspect}"
    end

    def on_disconnected error
      puts "#{inspect}: disconnected: #{error.inspect}"
    end
  end  # class TCPSocket

end
