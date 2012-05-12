#!/usr/bin/env ruby

require "eventmachine"
$LOAD_PATH.insert 0, File.expand_path(File.join(File.dirname(__FILE__), "../../", "lib"))
require "asyncengine"


Thread.abort_on_exception = true


$ae_datagrams = 0
$em_datagrams = 0
$time_start = Time.now

$em_port = 11111
$ae_port = 22222


at_exit do
  puts "\n\nINFO: exiting..."
  puts "- interval = #{interval = Time.now - $time_start} seconds"
  puts "- em_datagrams = #{$em_datagrams}  (#{($em_datagrams / interval).to_i} per second)"
  puts "- ae_datagrams = #{$ae_datagrams}  (#{($ae_datagrams / interval).to_i} per second)"

  exit!
end



class EmUdp < EM::Connection
  def receive_data data
    $em_datagrams += 1
    #send_data data.succ
    send_datagram data.succ, "127.0.0.1", $em_port
  end
end


class AeUdp < AE::UDPSocket
  def on_received_datagram data
    $ae_datagrams += 1
    send_datagram "127.0.0.1", $ae_port, data.succ
  end
end



t1 = Thread.new do
  EM.run do
    EM::open_datagram_socket("127.0.0.1", $em_port, EmUdp) do |udp|
      udp.send_datagram "A", "127.0.0.1", $em_port
    end
  end
end


t2 = Thread.new do
  AE.run do
    AE.open_udp_socket("127.0.0.1", $ae_port, AeUdp) do |udp|
      udp.send_datagram("127.0.0.1", $ae_port, "A")
    end
  end
end


t1.join rescue nil
t2.join rescue nil
