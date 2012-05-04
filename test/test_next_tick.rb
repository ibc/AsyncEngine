require "ae_test_helper"


class TestNextTick < AETest

  def test_01_next_tick_run_in_order
    str = ""

    AE.next_tick { str << "1" }
    AE.next_tick { str << "2" }
    AE.next_tick { str << "3" ; AE.next_tick { str << "6" } ; AE.next_tick { str << "7" } }
    AE.next_tick { AE.next_tick { str << "8" ; AE.next_tick { str << "B" } } }
    AE.next_tick { AE.next_tick { str << "9" ; AE.next_tick { str << "C" } ; AE.next_tick { str << "D" } } }
    AE.next_tick { str << "4" }
    AE.next_tick { str << "5" ; AE.next_tick { str << "A" } }

    AE.run

    assert_equal "123456789ABCD", str
  end

  def test_02_next_tick_and_timer
    str = ""

    AE.next_tick { str << "1" }
    AE.add_timer(0.001) { AE.next_tick { str << "3" } }
    AE.add_timer(0.002) { AE.next_tick { str << "4" ; AE.next_tick { str << "6" } } ; AE.next_tick { str << "5" } }
    AE.next_tick { str << "2" }

    AE.run

    assert_equal "123456", str
  end

end


