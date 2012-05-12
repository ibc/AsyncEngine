require "ae_test_helper"


class TestBasic < AETest

  def test_01_AE_is_restartable
    str = ""

    assert_false AE.running?
    AE.run { assert_true AE.running? }
    assert_false AE.running?

    AE.run
    assert_false AE.running?

    AE.run { AE.add_timer(0.001) { assert_true AE.running? ; str << "a" } }
    assert_false AE.running?

    AE.next_tick { str << "b" }
    AE.run

    AE.run do
      char = "b"
      iteration = 0
      pt1 = AE::PeriodicTimer.new(0.001) do
        str << char.next!
        iteration += 1
        pt1.cancel if iteration == 4
      end
    end
    assert_false AE.running?

    assert_equal "abcdef", str
  end

  def test_02_exception_handler
    AE.set_exception_handler {|e| assert e.is_a? ::StandardError }

    assert_respond_to AE.instance_variable_get(:@_exception_handler), :call

    assert_nothing_raised do
      AE.run { AE.next_tick { oppps } ; AE.add_timer(0.001) { uhhhh } }

      AE.next_tick { ouchh } ; AE.add_timer(0.001) { LOL }
      AE.run
    end

    assert_nothing_raised do
      AE.add_timer(0.001) { ohhhh1 }
      AE::Timer.new(0.001) { ohhhh2 }
      pt1 = AE::PeriodicTimer.new(0.001) { pt1.stop ; ohhhh3 }
      AE.run
    end

    # Dissable the exception handler again.
    AE.unset_exception_handler

    assert_equal nil, AE.instance_variable_get(:@_exception_manager)
  end

  def test_03_num_handles
    assert_equal 0, AE.num_handles

    AE.run do
      assert_equal 0, AE.num_handles
      AE.add_timer(0.001) { assert_equal 1, AE.num_handles }
      pt1 = AE::PeriodicTimer.new(0.002) { assert_equal 1, AE.num_handles ; pt1.stop ; assert_equal 0, AE.num_handles }
    end

    assert_equal 0, AE.num_handles
  end

end


