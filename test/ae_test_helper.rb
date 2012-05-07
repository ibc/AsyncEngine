require "asyncengine"
require "test/unit"
require "timeout"


class AETestError     < ::StandardError  ; end
class AETestTimeout   < AETestError      ; end


class AETest < Test::Unit::TestCase

def assert_true(object, message="")
    assert_equal(true, object, message)
  end

  def assert_false(object, message="")
    assert_equal(false, object, message)
  end

  # NOTE: Do not use assert_xxxx() methods within the block provided to this
  # method _set_timeout_ since they will be executed in a different process.
  def set_timeout timeout=2, block1=nil, &block2
    block = block1 || block2

    pid = Process.fork do
      begin
        Timeout.timeout(timeout) { block.call }
      rescue Timeout::Error
        exit 99
      rescue => e
        puts "ERROR: exception rescued => #{e.class}: #{e.message}"
        exit 1
      end
    end
    Process.wait pid

    case $?.exitstatus
    when 0
      return true
    when 99
      raise AETestTimeout, "test was cancelled after #{timeout} seconds"
    else
      raise AETestError, "test failed due to an exception"
    end
  end

end
