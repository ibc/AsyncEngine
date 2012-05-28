require "ae_test_helper"


class TestUtils < AETest

  def test_01_hrtime
    assert (AE::Utils.get_hrtime < AE::Utils.get_hrtime)
  end

  def test_02_memory
    assert (AE::Utils.get_free_memory < AE::Utils.get_total_memory)
  end

  def test_03_loadavg
    loadavg = AE::Utils.get_loadavg

    assert_true loadavg[0].is_a? Float
    assert_true loadavg[1].is_a? Float
    assert_true loadavg[2].is_a? Float
  end

  def test_04_uptime
    assert (AE::Utils.get_uptime < AE::Utils.get_uptime)
  end

  def test_05_network_interfaces
    AE::Utils.get_network_interfaces.each do |iface|
      assert_true (iface.is_a? AE::Utils::NetworkInterface)
      assert_true (iface.name.is_a? String)
      assert_true ([true, false].include? iface.internal?)
      assert_true ([:ipv4, :ipv6].include? iface.ip_type)
      assert_true (iface.ip.is_a? String)
    end
  end

  def test_06_cpu_info
    AE::Utils.get_cpu_info.each do |cpu|
      assert_true (cpu.is_a? AE::Utils::CpuInfo)
      assert_true (cpu.model.is_a? String)
      assert_true (cpu.speed.is_a? Fixnum)
      assert_true (cpu.time_sys.is_a? Fixnum)
      assert_true (cpu.time_user.is_a? Fixnum)
      assert_true (cpu.time_idle.is_a? Fixnum)
      assert_true (cpu.time_irq.is_a? Fixnum)
      assert_true (cpu.time_nice.is_a? Fixnum)
    end
  end

end
