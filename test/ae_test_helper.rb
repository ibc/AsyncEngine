require "asyncengine"
require "test/unit"
require "socket"


class AETest < Test::Unit::TestCase

  def assert_true(object, message="")
    assert_equal(true, object, message)
  end

  def assert_false(object, message="")
    assert_equal(false, object, message)
  end

  def setup
    AE.ensure_released
  end

  def host_loopback_ipv4 ; @@host_loopback_ipv4 ; end
  def host_ipv4 ; @@host_ipv4 ; end
  def host_loopback_ipv6 ; @@host_loopback_ipv6 ; end
  def host_ipv6 ; @@host_ipv6 ; end


  private

  def self.get_host_ipv4 loopback
    remote_ip = ( loopback ? "127.0.0.1" : "1.2.3.4" )
    orig, ::Socket.do_not_reverse_lookup = Socket.do_not_reverse_lookup, true
    ::UDPSocket.open(::Socket::AF_INET) do |s|
      s.connect remote_ip, 1
      s.addr.last
    end
  rescue => e
    warn "WARN: ae_test_helper.rb: cannot get IPv4 #{loopback ? "loopback " : ""}address in this host, some tests will be skipped"
    false
  ensure
    ::Socket.do_not_reverse_lookup = orig
  end

  def self.get_host_ipv6 loopback
    remote_ip = ( loopback ? "::1" : "2001::1" )
    orig, ::Socket.do_not_reverse_lookup = Socket.do_not_reverse_lookup, true
    ::UDPSocket.open(::Socket::AF_INET6) do |s|
      s.connect remote_ip, 1
      s.addr.last
    end
  rescue => e
    warn "WARN: ae_test_helper.rb: cannot get IPv6 #{loopback ? "loopback " : ""}address in this host, some tests will be skipped"
    false
  ensure
    ::Socket.do_not_reverse_lookup = orig
  end


  @@host_loopback_ipv4 = get_host_ipv4 loopback=true
  @@host_ipv4 = get_host_ipv4 loopback=false
  @@host_loopback_ipv6 = get_host_ipv6 loopback=true
  @@host_ipv6 = get_host_ipv6 loopback=false

end
