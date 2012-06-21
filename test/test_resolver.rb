require "ae_test_helper"


class TestResolver < AETest

  def test_01_resolve_success
    @num_ips = 0

    AE.run do
      AE::Resolver.resolve "google.com" do |error, ips|
        assert_nil error
        assert_equal Array, ips.class
        @num_ips = ips.size
        AE.stop
      end
    end

    AE.run do
      AE::Resolver.resolve "google.com", Socket::AF_UNSPEC do |error, ips|
        assert_equal @num_ips, ips.size
        AE.stop
      end
    end

    AE.run do
      AE::Resolver.resolve "google.com", Socket::AF_INET6 do |error, ips|
        assert_true @num_ips > ips.size
        AE.stop
      end
    end
  end

  def test_02_resolve_error
    AE.run do
      AE::Resolver.resolve "qweasdzxc.qwe" do |error, ips|
        assert_true error.is_a? AE::UvError
        assert_equal error.type, :ENOENT
        AE.stop
      end
    end

    AE.run do
      AE::Resolver.resolve "ipv6.google.com", Socket::AF_INET do |error, ips|
        assert_true error.is_a? AE::UvError
        assert_equal error.type, :ENOENT
        AE.stop
      end
    end
  end

end
