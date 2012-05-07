#!/usr/bin/env ruby

$LOAD_PATH.insert 0, File.expand_path(File.join(File.dirname(__FILE__), "../../", "lib"))

require "asyncengine"




class MyUDP
  include AE::UDPSocket

  def initialize a,b=nil
    puts "initialize() => a=#{(@a=a).inspect}, b=#{(@b=b).inspect}"
    puts "initialize() => ip_type=#{ip_type.inspect}, local_ip=#{local_ip}, local_port=#{local_port}"
  end
end


begin
  AE.open_udp_socket_ip4 "1.2.3.4", 1234, {}, MyUDP, "AAA", "BBB" do |c|
    puts "#{c.inspect}"
  end
rescue
end
AE.run { AE.add_timer(0.1) { puts "FIN" } }
exit


if true #and false
  AE.run do
    AE.test_send_udp4("1.2.3.4", 9999, "111")
    AE.test_send_udp4("1.02.03.004", 9999, "222")
    AE.test_send_udp4("1.2.3.A", 9999, "333")
  end
end





AE.run