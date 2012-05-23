require "ae_test_helper"


class TestTimer < AETest

  def test_01_timer_fires
    t1_executed = false

    AE.run do
      t1 = AE::Timer.new(0.01) { t1_executed = true ; assert_false t1.alive? }
      assert_true t1.alive?
      AE.add_timer(0.02) { assert_false t1.alive? }
      AE.add_timer(0.03) { assert_false t1.cancel }
    end

    assert_true t1_executed
  end

  def test_02_timer_canceled
    t1_executed = false
    pt1_executed = false

    AE.run do
      t1 = AE::Timer.new(0.02) { t1_executed = true }
      AE.add_timer(0.01) { assert_true t1.cancel ; assert_false t1.alive? }
      AE.add_timer(0.03) { assert_false t1.cancel }

      pt1 = AE::PeriodicTimer.new(0.02) { pt1_executed = true }
      AE.add_timer(0.01) { assert_true pt1.cancel ; assert_false pt1.alive? }
      AE.add_timer(0.03) { assert_false pt1.cancel }
    end

    assert_false t1_executed
    assert_false pt1_executed
  end

  def test_03_periodic_timer_canceled_after_4_repeats
    pt1_ticks = 0

    AE.run do
      pt1 = AE::PeriodicTimer.new(0.01) { pt1_ticks += 1 }
      AE.add_timer(0.045) { assert_true pt1.cancel }
    end

    assert_equal 4, pt1_ticks
  end

  def test_04_periodic_timer_interval_increased
    pt1_ticks = 0
    pt1_interval = 0.001

    AE.run do
      pt1 = AE::PeriodicTimer.new(pt1_interval) do
        # This should stop after 0.015 seconds.
        pt1.cancel if (pt1_ticks += 1) == 4
        pt1.restart (pt1_interval *= 2)
      end

      # So check that pt1 is terminated after 0.05 seconds (it should).
      AE.add_timer(0.05) { assert_false pt1.alive? }
    end

    assert_equal 4, pt1_ticks
  end

  def test_05_timer_restarted
    str1 = ""
    str2 = ""

    AE.run do
      t1 = AE::Timer.new(0.01) { str1 << "0" }
      t1.restart  # So "0" will be written.
      AE.next_tick { t1.restart }
      AE.add_timer(0.02) { t1.restart }  # Timer was cancelled so nothing new occurs.

      t2 = AE::Timer.new(0.01, Proc.new { str2 << "0" })
      t2.stop
      AE.add_timer(0.001) { t2.restart(0.002) }
    end

    assert_equal "0", str1
    assert_equal "0", str2
  end

  def test_06_timer_canceled_on_its_callback_does_not_crash
    AE.run do
      t1 = AE::Timer.new(0.001) { assert_false t1.cancel }
    end
  end

end
