module AsyncEngine

  def self.open_udp_socket *params
    sock = UDPSocket.new *params

    # Run the given block.
    yield sock  if block_given?

    # Return the instance.
    sock
  end


  class UDPSocket < Handle
    alias orig_to_s to_s
    def to_s
      ip, port = local_address()
      if alive?
        "#{orig_to_s} (#{ip_type().inspect} : #{ip} : #{port})"
      else
        "#{orig_to_s} (not alive)"
      end
    end
    alias :inspect :to_s


    private

    def on_datagram_received datagram, src_ip, src_port
      puts "#{inspect}: received datagram from #{src_ip} : #{src_port}: #{datagram.inspect}"
    end
  end  # class UDPSocket

end
