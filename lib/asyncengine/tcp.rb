module AsyncEngine

  class TCPSocket < Handle
    private

    def on_connected
      puts "#{inspect}: connected: local address: #{local_address()}"
    end

    def on_connection_error error
      puts "#{inspect}: connection error: #{error.inspect}"
    end

    def on_data_received data
      #puts "#{inspect}: received data from #{peer_address()}: #{data.inspect}"
      puts "#{inspect}: received data: #{data.inspect}"
    end

    def on_disconnected error
      puts "#{inspect}: disconnected: #{error.inspect}"
    end
  end  # class TCPSocket

end
