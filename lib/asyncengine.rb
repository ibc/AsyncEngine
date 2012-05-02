# AsyncEngine native C extension.
require "asyncengine/asyncengine_ext.so"

# AsyncEngine Ruby library files.
require "asyncengine/version.rb"
require "asyncengine/errors.rb"
require "asyncengine/timer.rb"
require "asyncengine/next_tick.rb"
require "asyncengine/udp.rb"


module AsyncEngine

  @_is_running = false
  @_blocks = {}
  @_next_ticks = []

  def self.run
    raise AsyncEngine::Error, "AsyncEngine already running"  if @_is_running
    @_is_running = true

    yield  if block_given?

    if _c_run
      # _c_run does not terminate (it blocks) until all the handles end.
      # If so, let's set again @_is_running to false and return true.
      @_is_running = false
      return true
    else
      raise AsyncEngine::Error, "AsyncEngine failed to run"
    end
  end

  def self.running?
    @_is_running
  end

  def self.exception_manager block1=nil, &block2
    if block1 || block2
      @_exception_manager = (block1 || block2)
    elsif instance_variable_defined? :@_exception_manager
      remove_instance_variable :@_exception_manager
    end
  end

  def self.manage_exception e
    if @_exception_manager
      @_exception_manager.call e
    else
      raise e
    end
  end

  class << self
    private :_c_run
    private :manage_exception
  end

end


# Let's allow AE.xxxxx usage.
AE = AsyncEngine