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
  @_exception_handler = nil
  @_blocks = {}
  @_next_ticks = []
  @_handles = {}

  def self.run
    raise AsyncEngine::Error, "AsyncEngine already running"  if @_is_running
    @_is_running = true

    # SIGPIPE is received by a program when reading from a pipe whose other
    # end has been closed, so it's good to trap and ignore it:
    #   http://doc.dvgu.ru/devel/ev.html#the_special_problem_of_sigpipe
    #
    # However this is not needed since both UV and Ruby ignore by default
    # SIGPIPE signal.
    trap(:PIPE) {}

    yield  if block_given?

    if _c_run
      # _c_run does not terminate (it blocks) until all the handles end.
      # If so, let's set again @_is_running to false and return true.
      @_is_running = false
      return true
    else
      @_is_running = false
      raise AsyncEngine::Error, "AsyncEngine failed to run"
    end
  end

  def self.running?
    @_is_running
  end

  def self.set_exception_handler block1=nil, &block2
    block = block1 || block2
    raise AsyncEngine::Error, "no block provided"  unless block.respond_to? :call

    @_exception_handler = block
  end

  def self.unset_exception_handler
    @_exception_handler = nil
  end

  def self.handle_exception e
    if @_exception_handler
      @_exception_handler.call e
    else
      raise e
    end
  end

  # TODO: testing
  def self.debug
    puts "DBG: AsyncEngine debug:"
    puts
    puts "@_blocks: #{@_blocks.inspect}"
    puts
    puts "@_handles: #{@_handles.inspect}"
    puts
  end

  class << self
    private :_c_run
    private :handle_exception
  end

end


# Let's allow AE.xxxxx usage.
AE = AsyncEngine