require "ae_test_helper"


class TestTimer < AETest

  def test_01_timer_expires
    t1_executed = false

    t1 = AE::Timer.new(0.01) { t1_executed = true ; assert_true t1.active? }
    assert_true t1.active?
    AE.add_timer(0.02) { assert_false t1.active? }
    AE.add_timer(0.03) { assert_false t1.cancel }

    AE.run

    assert_true t1_executed
  end

  def test_02_timer_is_canceled
    t1_executed = false
    pt1_executed = false

    t1 = AE::Timer.new(0.02) { t1_executed = true }
    AE.add_timer(0.01) { assert_true t1.cancel ; assert_false t1.active? }
    AE.add_timer(0.03) { assert_false t1.cancel }

    pt1 = AE::PeriodicTimer.new(0.02) { pt1_executed = true }
    AE.add_timer(0.01) { assert_true pt1.cancel ; assert_false pt1.active? }
    AE.add_timer(0.03) { assert_false pt1.cancel }

    AE.run

    assert_false t1_executed
    assert_false pt1_executed
  end

  def test_03_periodic_timer_is_canceled_after_4_repeats
    pt1_ticks = 0

    pt1 = AE::PeriodicTimer.new(0.01) { pt1_ticks += 1 }
    AE.add_timer(0.045) { assert_true pt1.cancel }

    AE.run

    assert_equal 4, pt1_ticks
  end

  def test_04_periodic_timer_interval_is_increased
    pt1_ticks = 0
    pt1_interval = 0.001

    pt1 = AE::PeriodicTimer.new(pt1_interval) do
      # This should stop after 0.015 seconds.
      pt1.stop if (pt1_ticks += 1) == 4
      pt1.set_interval (pt1_interval *= 2)
    end
    pt1.set_interval (pt1_interval *= 2)

    # So check that pt1 is stopped after 0.016 seconds (it should).
    AE.add_timer(0.016) { assert_false pt1.active? }

    AE.run

    assert_equal 4, pt1_ticks
  end

end


