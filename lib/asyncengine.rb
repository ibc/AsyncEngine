# AsyncEngine native C extension.
require "asyncengine/asyncengine_ext.so"

# AsyncEngine Ruby library files.
require "asyncengine/version.rb"
require "asyncengine/errors.rb"
require "asyncengine/timers.rb"


module AsyncEngine

  @handles = {}

  def self.start
    yield  if block_given?

    if self._c_start
      # This method does not terminate (it blocks) unless no handles are
      # given (which is useless, but if so, return true).
      return true
    else
      raise AsyncEngine::Error, "failed to start"
    end
  end

end


AE = AsyncEngine