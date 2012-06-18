require "ae_test_helper"


class TestUdp < AETest

  RECEIVED_DATAGRAMS = []

  class MyUDP < AE::UDPSocket
    def initialize
      @local_ip, @local_port = local_address()
    end
  end

  def setup
    super
    RECEIVED_DATAGRAMS.clear
  end

  def test_01_udp_basic_ipv4
    return false  unless (ip = host_loopback_ipv4)
    private_test_01_udp_basic ip
  end

  def test_01_udp_basic_ipv6
    return false  unless (ip = host_loopback_ipv6)
    private_test_01_udp_basic ip
  end

  def test_02_set_encoding_ipv4
    return false  unless (ip = host_loopback_ipv4)
    private_test_02_set_encoding ip
  end

  def test_02_set_encoding_ipv6
    return false  unless (ip = host_loopback_ipv6)
    private_test_02_set_encoding ip
  end

  def test_03_send_callback_ipv4
    return false  unless (ip = host_loopback_ipv4)
    private_test_03_send_callback ip
  end

  def test_03_send_callback_ipv6
    return false  unless (ip = host_loopback_ipv6)
    private_test_03_send_callback ip
  end

  def test_04_datagram_source_address_ipv4
    return false  unless (ip = host_loopback_ipv4)
    private_test_04_datagram_source_address ip
  end

  def test_04_datagram_source_address_ipv6
    return false  unless (ip = host_loopback_ipv6)
    private_test_04_datagram_source_address ip
  end


  private

  def private_test_01_udp_basic ip
    AE.run do
      sock = AE.open_udp_socket ip, 0
      local_ip, local_port = sock.local_address()

      def sock.on_datagram_received datagram, ip, port
        RECEIVED_DATAGRAMS << datagram
      end

      assert_true sock.alive?
      assert_false sock.paused?

      assert_equal ip, local_ip

      # This will be received.
      assert_true sock.send_datagram("1", local_ip, local_port)

      AE.add_timer(0.02) do
        sock.pause
        assert_true sock.paused?

        # This wont be received.
        assert_true sock.send_datagram("2", local_ip, local_port)
      end

      AE.add_timer(0.06) do
        sock.resume
        assert_false sock.paused?

        # This will be received.
        assert_true sock.send_datagram("3", local_ip, local_port)
      end

      AE.add_timer(0.12) do
        assert_true sock.close()

        assert_false sock.alive?

        # Socket closed, cannot be sent.
        assert_false sock.send_datagram "hello", "1.2.3.4", 1234

        AE.stop
      end
    end

    assert_equal ["1", "3"], RECEIVED_DATAGRAMS
  end

  def private_test_02_set_encoding ip
    AE.run do
      sock = AE.open_udp_socket ip, 0
      local_ip, local_port = sock.local_address()

      def sock.on_datagram_received datagram, ip, port
        RECEIVED_DATAGRAMS << datagram
        AE.stop  if RECEIVED_DATAGRAMS.size == 2
        set_encoding_ascii()
      end

      # Default UDP receiving encoding is ASCII_8BIT.
      assert_equal sock.encoding, :ascii

      sock.set_encoding_utf8
      assert_equal sock.encoding, :utf8
      sock.send_datagram "\x80", local_ip, local_port

      sock.send_datagram "\x80", local_ip, local_port
    end

    assert_equal RECEIVED_DATAGRAMS[0].encoding, Encoding::UTF_8
    assert_false RECEIVED_DATAGRAMS[0].valid_encoding?
    assert_equal RECEIVED_DATAGRAMS[1].encoding, Encoding::ASCII_8BIT
    assert_true RECEIVED_DATAGRAMS[1].valid_encoding?
  end

  def private_test_03_send_callback ip
    send_cb_ok = 0
    send_cb_error = 0

    AE.run do
      sock = AE.open_udp_socket ip, 4444
      local_ip, local_port = sock.local_address()

      def sock.on_datagram_received datagram, ip, port
        RECEIVED_DATAGRAMS << datagram
      end

      sock.send_datagram "1", local_ip, local_port do |error|
        error ? (send_cb_error += 1) : (send_cb_ok += 1)
      end

      sock.send_datagram("2", local_ip, local_port, proc do |error|
        error ? (send_cb_error += 1) : (send_cb_ok += 1)
      end)

      # Sending to destination port 0 causes a EINVAL error.
      sock.send_datagram "3", local_ip, 0 do |error|
        error ? (send_cb_error += 1) : (send_cb_ok += 1)
        assert_equal error.class, AE::UvError
        assert_equal error.type, :EINVAL
      end

      AE.add_timer(0.1) { assert_true sock.close() ; AE.stop }
    end

    assert_equal ["1", "2"], RECEIVED_DATAGRAMS
    assert_equal 2, send_cb_ok
    assert_equal 1, send_cb_error
  end

  def private_test_04_datagram_source_address ip
    sock = nil
    local_ip = nil
    local_port = nil

    AE.run do
      sock = AE.open_udp_socket ip, 0
      local_ip, local_port = sock.local_address()

      def sock.on_datagram_received datagram, src_ip, src_port
        RECEIVED_DATAGRAMS << datagram
        @last_src_ip = src_ip
        @last_src_port = src_port

        case RECEIVED_DATAGRAMS.size
        when 1
          send_datagram "reply1", src_ip, src_port
        when 2
          send_datagram "reply2", src_ip, src_port do |error|
            error ? (@reply_cb_error += 1) : (@reply_cb_ok += 1)
          end
        when 3
          send_datagram("reply3", src_ip, src_port, proc do |error|
            error ? (@reply_cb_error += 1) : (@reply_cb_ok += 1)
          end)
        when 4
          AE.stop
        end
      end

      sock.instance_variable_set :@reply_cb_ok, 0
      sock.instance_variable_set :@reply_cb_error, 0

      sock.send_datagram "hello", local_ip, local_port
    end

    assert_equal ["hello", "reply1", "reply2", "reply3"], RECEIVED_DATAGRAMS
    assert_equal 2, sock.instance_variable_get(:@reply_cb_ok)
    assert_equal 0, sock.instance_variable_get(:@reply_cb_error)
    assert_equal sock.instance_variable_get(:@last_src_ip), local_ip
    assert_equal sock.instance_variable_get(:@last_src_port), local_port
  end

end
