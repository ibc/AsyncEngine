require "asyncengine"
require "test/unit"


class AETestError     < ::StandardError  ; end
class AETestTimeout   < AETestError      ; end


class AETest < Test::Unit::TestCase

  def assert_true(object, message="")
    assert_equal(true, object, message)
  end

  def assert_false(object, message="")
    assert_equal(false, object, message)
  end

end
