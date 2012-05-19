require "ae_test_helper"


class TestTimer < AETest

  def test_01_timer_fires
    t1_executed = false

    t1 = AE::Timer.new(0.01) { t1_executed = true ; assert_false t1.alive? }
    assert_true t1.alive?
    AE.add_timer(0.02) { assert_false t1.alive? }
    AE.add_timer(0.03) { assert_false t1.cancel }

    AE.run

    assert_true t1_executed
  end

  def test_02_timer_canceled
    t1_executed = false
    pt1_executed = false

    t1 = AE::Timer.new(0.02) { t1_executed = true }
    AE.add_timer(0.01) { assert_true t1.cancel ; assert_false t1.alive? }
    AE.add_timer(0.03) { assert_false t1.cancel }

    pt1 = AE::PeriodicTimer.new(0.02) { pt1_executed = true }
    AE.add_timer(0.01) { assert_true pt1.cancel ; assert_false pt1.alive? }
    AE.add_timer(0.03) { assert_false pt1.cancel }

    AE.run

    assert_false t1_executed
    assert_false pt1_executed
  end

  def test_03_periodic_timer_canceled_after_4_repeats
    pt1_ticks = 0

    pt1 = AE::PeriodicTimer.new(0.01) { pt1_ticks += 1 }
    AE.add_timer(0.045) { assert_true pt1.cancel }

    AE.run

    assert_equal 4, pt1_ticks
  end

  def test_04_periodic_timer_interval_increased
    pt1_ticks = 0
    pt1_interval = 0.001

    pt1 = AE::PeriodicTimer.new(pt1_interval) do
      # This should stop after 0.015 seconds.
      pt1.cancel if (pt1_ticks += 1) == 4
      pt1.restart (pt1_interval *= 2)
    end

    # So check that pt1 is terminated after 0.016 seconds (it should).
    AE.add_timer(0.016) { assert_false pt1.alive? }

    AE.run

    assert_equal 4, pt1_ticks
  end

  def _test_05_timer_restarted
    str1 = ""
    str2 = ""

    t1 = AE::Timer.new(0.001) { str1 << "0" }
    t1.restart { str1 << "1" }  # So "0" will never be written.
    AE.next_tick { t1.restart }  # Restarts with last block so writes "1".
    AE.add_timer(0.010) { t1.restart { str1 << "2" } }  # Restarts with same block so writes "1".

    t2 = AE::Timer.new(0.001, Proc.new { str2 << "0" })
    t2.cancel
    AE.add_timer(0.01) { t2.restart(0.002) }

    AE.run

    assert_equal "12", str1
    assert_equal "0", str2

  end

  def _test_06_periodic_timer_restarted
    pt1_ticks = 0
    str = ""

    pt1 = AE::PeriodicTimer.new(0.001) do
      str << "0"
      pt1.restart(0.002) do
        str << "1"
        pt1.cancel
        AE.next_tick do
          pt1.restart(0.001) { str << "NO" }
          pt1.restart(nil, 0.002) do
            str << "2"
            pt1.cancel
          end
        end
      end
    end

    AE.run

    assert_equal "012", str
  end

  def test_07_timer_canceled_on_its_callback_does_not_crash
    t1 = AE::Timer.new(0.001) { assert_false t1.cancel }

    AE.run
  end

end
