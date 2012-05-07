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
  @_exception_manager = nil
  @_blocks = {}
  @_next_ticks = []

  def self.run
    raise AsyncEngine::Error, "AsyncEngine already running"  if @_is_running
    @_is_running = true

    @_thread = Thread.current

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

  def self.set_exception_manager block1=nil, &block2
    block = block1 || block2
    raise AsyncEngine::Error, "no block provided"  unless block.respond_to? :call

    @_exception_manager = block
  end

  def self.unset_exception_manager
    @_exception_manager = nil
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