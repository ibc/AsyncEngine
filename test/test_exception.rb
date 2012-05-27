require "ae_test_helper"


class TestException < AETest

  def test_01_exception_raised
    assert_raise NameError do
      AE.run { ouch1a }
    end
    assert_false AE.running?

    assert_raise NameError do
      AE.run { AE.next_tick { ouch1b } }
    end
    assert_false AE.running?

    assert_raise NameError do
      AE.run { AE.add_timer(0) { ouch1c } }
    end
    assert_false AE.running?

    assert_raise NameError do
      AE.run do
        AE.add_timer(0) do
          AE.next_tick do
            AE.add_timer(0) { ouch1d }
          end
        end
      end
    end
    assert_false AE.running?
  end

  def test_02_exception_handler
    AE.set_exception_handler {|e| assert e.is_a? ::StandardError }
    assert_respond_to AE.instance_variable_get(:@_exception_handler), :call

    assert_nothing_raised do
      AE.run { qweqwe }
      AE.run { AE.next_tick { uhhhh1 } }
      AE.run { AE.add_timer(0) { uhhhh2 } }
      AE.run do
        AE.next_tick { uhhhh3 } ; AE.add_timer(0.001) { uhhhh4 }
      end
      AE.run do
        AE.next_tick { uhhhh5 } ; AE.add_timer(0.001) { uhhhh6 }
      end
      AE.run do
        AE.add_timer(0.001) { ohhhh1 }
        AE::Timer.new(0.001) { ohhhh2 }
        pt1 = AE::PeriodicTimer.new(0.001) { pt1.stop ; ohhhh3 }
      end
    end
    assert_false AE.running?

    # Dissable the exception handler again.
    AE.unset_exception_handler
    assert_equal nil, AE.instance_variable_get(:@_exception_manager)

    assert_raise NameError do
      AE.run { AE.add_timer(0) { opppps } }
    end
    assert_false AE.running?
  end

  def test_03_exception_in_exception_handler
    AE.set_exception_handler {|e| 1/0 }

    assert_raise ZeroDivisionError do
      AE.run { AE.next_tick { ouch3 } }
    end
    assert_false AE.running?

    assert_raise ZeroDivisionError do
      AE.run { AE::Timer.new(0) { 1/0 } }
    end
    assert_false AE.running?

    # Dissable the exception handler again.
    AE.unset_exception_handler
  end

end
