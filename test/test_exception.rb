require "ae_test_helper"


class TestException < AETest

  def _test_01_exception_in_yield
    assert_raise NameError do
      AE.run { ouch1 }
    end

    assert_false AE.running?
    assert_true AE.run
  end

  def _test_02_exception_in_callback
    assert_raise NameError do
      AE.run { AE.next_tick { ouch2a } }
    end

    assert_false AE.running?
    assert_true AE.run

    assert_raise NameError do
      AE.run { AE::Timer.new(0) { ouch2b } }
    end

    assert_false AE.running?
    assert_true AE.run
  end

  def _test_03_exception_in_exception_handler
    AE.set_exception_handler {|e| 1/0 }

    assert_raise ZeroDivisionError do
      AE.run { AE.next_tick { ouch3 } }
    end

    assert_false AE.running?
    assert_true AE.run

    assert_raise ZeroDivisionError do
      AE.run { AE::Timer.new(0) { 1/0 } }
    end

    assert_false AE.running?
    assert_true AE.run
  end
end
