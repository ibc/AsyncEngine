# AsyncEngine vative C extension.
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
      return true
    else
      raise AsyncEngine::Error, "uv_run failed to start"
    end
  end

end


AE = AsyncEngine