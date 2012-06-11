require "ae_test_helper"


class TestBasic < AETest

  def test_01_AE_is_restartable
    str = ""

    assert_false AE.running?
    AE.run { assert_true AE.running? ; AE.stop }
    assert_false AE.running?

    assert_true AE.run { AE.stop }
    assert_false AE.running?

    assert_true AE.run { AE.add_timer(0.001) { assert_true AE.running? ; str << "a" ; AE.stop } }
    assert_false AE.running?

    assert_true AE.run { AE.next_tick { str << "b" ; AE.stop } }
    assert_true AE.run { AE.stop }

    AE.run do
      char = "b"
      iteration = 0
      pt1 = AE::PeriodicTimer.new(0.001) do
        str << char.next!
        iteration += 1
        AE.stop  if iteration == 4
      end
    end
    assert_false AE.running?

    assert_equal "abcdef", str
  end

  def test_02_stop_works_as_expected
    @var = false
    AE.run do
      AE.next_tick { @var = true }
      AE.stop
    end
    assert_false AE.running?
    assert_true @var

    @var = false
    AE.run do
      AE.next_tick { AE.next_tick { @var = true } }
      AE.stop
    end
    assert_false AE.running?
    assert_false @var

    @var = false
    AE.run do
      AE.add_timer(0.1) { @var = true }
      AE.stop
    end
    assert_false @var

    @var = false
    AE.run do
      AE.add_timer(0.11) { @var = true }
      AE.next_tick { AE.stop }
    end
    assert_false @var
  end

  def test_03_run_in_run
    num = 0

    AE.run do
      num+=1
      AE.run { num+=1 }
      AE.run { AE.run { num+=1 } } # Wont' occur.
      AE.next_tick { num+=1 }
      AE.next_tick { AE.next_tick { num+=1 } }  # Won't occur.
      AE.stop
    end
    assert_equal 3, num
  end

  def test_04_multiple_stop
    AE.run do
      AE.next_tick { AE.next_tick { AE.stop } }
      AE.next_tick { AE.stop ; AE.stop }
      AE.add_timer(0) { AE.stop }
    end
    assert_false AE.running?
  end

  def test_05_stop_from_different_thread
    @var = false
    Thread.new { sleep 0.05 ; AE.stop }
    AE.run do
      AE.add_timer(1) { @var = true }  # Won't occur.
    end
    assert_false @var
  end

  def test_06_signal_kills_asyncengine
    @var = false

    assert_raise SignalException do
      AE.run do
        AE.add_timer(0) { Process.kill :USR1, $$ }
        AE.add_timer(1) { @var = true }  # Won't occur.
      end
    end

    assert_true AE.run { AE.stop }
    assert_false @var
  end

  def test_07_force_still_releasing_error
    @var = false
    assert_raise AE::StillReleasingError do
      AE.run do
        AE.stop
        AE.next_tick { AE.run { @var = true } }
      end
    end
    assert_false @var

    @var = false
    assert_raise AE::StillReleasingError do
      th = Thread.new { sleep }
      AE.run do
        AE.stop
        AE.next_tick { AE.run { @var = true } }
      end
      th.kill
    end
    assert_false @var
  end

end
