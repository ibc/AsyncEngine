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

end
