require "ae_test_helper"


class TestCallFromOtherThread < AETest

  def test_01_call_from_other_thread_run_in_order_when_in_same_thread
    str = ""

    AE.run do
      AE.call_from_other_thread { str << "1" }
      AE.call_from_other_thread { str << "2" }
      AE.call_from_other_thread { str << "3" ; AE.call_from_other_thread { str << "6" } ; AE.call_from_other_thread { str << "7" } }
      AE.call_from_other_thread { AE.call_from_other_thread { str << "8" ; AE.call_from_other_thread { str << "B" } } }
      AE.call_from_other_thread { AE.call_from_other_thread { str << "9" ; AE.call_from_other_thread { str << "C" } ; AE.call_from_other_thread { str << "D" ; AE.stop } } }
      AE.call_from_other_thread { str << "4" }
      AE.call_from_other_thread { str << "5" ; AE.call_from_other_thread { str << "A" } }
    end

    assert_equal "123456789ABCD", str
  end

  def test_02_call_from_other_thread_and_timer
    str = ""

    AE.run do
      AE.call_from_other_thread { str << "1" }
      AE.add_timer(0.01) { AE.call_from_other_thread { str << "3" } }
      AE.add_timer(0.02) { AE.call_from_other_thread { str << "4" ; AE.call_from_other_thread { str << "6" ; AE.stop } } ; AE.call_from_other_thread { str << "5" } }
      AE.call_from_other_thread { str << "2" }
    end

    assert_equal "123456", str
  end

  def test_03_multiple_stop_in_cascade_from_different_thread
    @var = false

    AE.run do
      AE.add_timer(1) { @var = true }  # Won't occur.
      Thread.new do
        AE.call_from_other_thread do
          AE.call_from_other_thread do
            AE.call_from_other_thread do
              AE.stop
            end
          end
        end
      end
    end
    assert_false @var
  end

end


