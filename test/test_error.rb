require "ae_test_helper"


class TestError < AETest

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

  def test_02_error_handler
    AE.on_error {|e| assert e.is_a? ::StandardError }
    assert_respond_to AE.instance_variable_get(:@_error_handler), :call

    assert_nothing_raised do
      AE.run do
        AE.next_tick { qweqwe }
        AE.add_timer(0) { uhhhh2 }
        AE.add_timer(0.01) { AE.stop }
        lalalalala
      end
    end
    assert_false AE.running?

    # Dissable the error handler again.
    AE.unset_on_error
    assert_equal nil, AE.instance_variable_get(:@_error_manager)

    assert_raise NameError do
      AE.run { AE.add_timer(0) { opppps } }
    end
    assert_false AE.running?
  end

  def test_03_exception_in_error_handler
    AE.on_error {|e| 1/0 }

    assert_raise ZeroDivisionError do
      AE.run { AE.next_tick { ouch3 } }
    end
    assert_false AE.running?

    assert_raise ZeroDivisionError do
      AE.run { AE::Timer.new(0) { 1/0 } }
    end
    assert_false AE.running?

    AE.unset_on_error
  end

  def test_04_error_handler_in_next_tick_allows_executes_all_next_ticks
    AE.on_error {|e| assert e.is_a? ::StandardError }

    num = 0
    AE.run do
      AE.next_tick { num+=1 }
      AE.next_tick { num+=1 }
      AE.next_tick { bumpppp }
      AE.next_tick { num+=1 }
      AE.next_tick { num+=1 }
      AE.next_tick { AE.stop }
    end

    AE.unset_on_error

    assert_equal 4, num
  end

end
