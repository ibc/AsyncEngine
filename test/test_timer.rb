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

  def test_02_timer_is_cancelled
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

end


