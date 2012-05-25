require "ae_test_helper"


class TestIpUtils < AETest

  def test_01_ip_type
    assert_equal :ipv4, AE::IpUtils.ip_type("1.2.3.4")
    assert_false AE::IpUtils.ip_type("1.2.3.04")
    assert_false AE::IpUtils.ip_type("1.2.3.004")
    assert_false AE::IpUtils.ip_type("1.2.3.0004")

    assert_equal :ipv6, AE::IpUtils.ip_type("1080::8:800:200C:417A")
    assert_equal :ipv6, AE::IpUtils.ip_type("1080:0:0:0:8:800:200C:417A")
    assert_equal :ipv6, AE::IpUtils.ip_type("1080:0000:0000:0000:0008:0800:200C:417A")
    assert_false AE::IpUtils.ip_type("1::8::1")

    assert_equal :ipv6_reference, AE::IpUtils.ip_type("[1080::8:800:200C:417A]")
    assert_equal :ipv6_reference, AE::IpUtils.ip_type("[1080:0:0:0:8:800:200C:417A]")
    assert_equal :ipv6_reference, AE::IpUtils.ip_type("[1080:0000:0000:0000:0008:0800:200C:417A]")
    assert_false AE::IpUtils.ip_type("[1::8::1]")

    assert_false AE::IpUtils.ip_type("domain.net")
  end

  def test_02_compare_ips
    assert_true AE::IpUtils.compare_ips("255.255.255.255", "255.255.255.255")
    assert_false AE::IpUtils.compare_ips("1.2.3.4", "9.9.9.9")
    assert_nil AE::IpUtils.compare_ips("1.2.3.4", "1.2.3.04")

    assert_true AE::IpUtils.compare_ips("1080::8:800:200C:417A", "1080:0:0:0:8:800:200C:417A")
    assert_true AE::IpUtils.compare_ips("1080::8:800:200C:417A", "1080:0000:0000:0000:0008:0800:200c:417a")
    assert_true AE::IpUtils.compare_ips("::", "00::00:0")
    assert_false AE::IpUtils.compare_ips("1080::8:800:200C:417A", "1::2")

    assert_true AE::IpUtils.compare_ips("[1080::8:800:200C:417A]", "[1080:0:0:0:8:800:200C:417A]")
    assert_true AE::IpUtils.compare_ips("[1080::8:800:200C:417A]", "[1080:0:0:0:8:800:200C:417A]")

    assert_true AE::IpUtils.compare_ips("1080::8:800:200C:417A", "[1080:0:0:0:8:800:200C:417A]", true)
    assert_true AE::IpUtils.compare_ips("[1080:0:0:0:8:800:200C:417A]", "1080::8:800:200C:417A", true)

    assert_nil AE::IpUtils.compare_ips("1", "::")
  end

end
