# AsyncEngine native C extension.
require "asyncengine/asyncengine_ext.so"

# AsyncEngine Ruby library files.
require "asyncengine/version.rb"
require "asyncengine/errors.rb"
require "asyncengine/timer.rb"
require "asyncengine/next_tick.rb"


module AsyncEngine

  @handles = {}
  @next_ticks = []

  def self.start
    yield  if block_given?

    if _c_start
      # This method does not terminate (it blocks) until all the handles
      # end. If so it returns true.
      return true
    else
      raise AsyncEngine::Error, "failed to start"
    end
  end

  class << self
    alias :run :start
    private :_c_start
  end

end


# Let's allow AE.xxxxx usage.
AE = AsyncEngine