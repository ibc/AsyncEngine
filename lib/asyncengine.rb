# AsyncEngine native C extension.
require "asyncengine/asyncengine_ext.so"

# AsyncEngine Ruby library files.
require "asyncengine/version.rb"
require "asyncengine/errors.rb"
require "asyncengine/timer.rb"
require "asyncengine/next_tick.rb"


module AsyncEngine

  @callbacks = {}
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

  def self.error_handler callback=nil, &block
    if callback || block
      @error_handler = (callback || block)
    elsif instance_variable_defined? :@error_handler
      remove_instance_variable :@error_handler
    end
  end

  def self.handle_error e
    if @error_handler
      @error_handler.call e
    else
      raise e
    end
  end

  class << self
    alias :run :start
    private :_c_start
    private :handle_error
  end

end


# Let's allow AE.xxxxx usage.
AE = AsyncEngine