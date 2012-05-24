require "ae_test_helper"


class TestThreads < AETest

  def test_01_thread_works_as_expected
    str = ""

    AE.run do
      AE.add_periodic_timer(0.1) {}

      Thread.new { str << "a" }
      Thread.new { sleep 0.01 ; str << "b" }
      Thread.new { sleep 0.02 ; str << "c" }
      Thread.new { sleep 0.03 ; str << "d" }
      Thread.new { sleep 0.04 ; AE.stop }
    end

    assert_false AE.running?
    assert_equal "abcd", str
  end

  def test_02_can_run_anything_in_call_from_other_thread
    str = ""

    AE.run do
      Thread.new do
        AE.call_from_other_thread { AE.next_tick { str << "a" } }
      end
      Thread.new do
        sleep 0.02
        AE.call_from_other_thread { AE.add_timer(0) { str << "b" } }
      end
      Thread.new do
        sleep 0.02
        AE.call_from_other_thread { AE.add_timer(0.05) { str << "c" } }
      end
      str << "1"
      AE.next_tick { str << "2" }
      AE.add_timer(0.1) { str << "d" ; AE.stop }
    end

    assert_false AE.running?
    assert_equal "12abcd", str
  end

  def test_03_run_and_stop_are_thread_safe
    summ = 0

    th1 = Thread.new do
      AE.run { AE.add_timer(0.1) { AE.stop } }
    end

    th2 = Thread.new do
      sleep 0.01
      AE.run { summ+=1 }
    end

    th3 = Thread.new do
      sleep 0.01
      AE.run { AE.add_timer(0) { summ+=1 } }
    end

    th1.join ; th2.join ; th3.join

    assert_equal 2, summ
  end

end
