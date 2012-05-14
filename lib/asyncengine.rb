# AsyncEngine native C extension.
require "asyncengine/asyncengine_ext.so"

# AsyncEngine Ruby library files.
require "asyncengine/version.rb"
require "asyncengine/errors.rb"
require "asyncengine/timer.rb"
require "asyncengine/next_tick.rb"
require "asyncengine/udp.rb"


module AsyncEngine

  @_exception_handler = nil
  @_blocks = {}
  @_next_ticks = []
  @_handles = {}

  def self.run
    if running?
      puts log + "NOTICE: AsyncEngine was already running, forcing stop and re-run."

      begin
        stop
      rescue AsyncEngine::StopException
        puts "--- run() : rescue AsyncEngine::StopException"
      end

      loop do
        sleep 0.01
        break  unless running?
      end
    end

    # SIGPIPE is received by a program when reading from a pipe whose other
    # end has been closed, so it's good to trap and ignore it:
    #   http://doc.dvgu.ru/devel/ev.html#the_special_problem_of_sigpipe
    #
    # However this is not needed since both UV and Ruby ignore by default
    # SIGPIPE signal.
    trap(:PIPE) {}

    # Run the block, and be ready to properly exit if the use calls to stop()
    # within the first level of the given block.
    begin
      yield  if block_given?
    rescue AsyncEngine::StopException
      return false
    end

    @_running_thread = Thread.current

    at_exit { stop rescue nil }

    begin
      if _c_run
        return true
      else
        # TODO: This should NEVER occur!
        raise AsyncEngine::Error, "AsyncEngine failed to run"
      end
    rescue AsyncEngine::StopException
    rescue AsyncEngine::StopFromOtherThreadException
      stop rescue nil  # Ignore the AsyncEngine::StopException to be created.
    rescue Exception => e
      puts log + "NOTICE: AsyncEngine._c_run raised #{e.class} (#{e}), running AE.stop now and raising the exception..."
      stop rescue nil  # Ignore the AsyncEngine::StopException to be created.
      raise e # And raise the original exception.
    end
  end

  # TODO: Si el thread en el que se arrancó AE ha muerto (i.e. Thread#kill) sin terminar AE,
  # entonces esto peta casi siempre.
  def self.stop
    raise AsyncEngine::StopException, "AsyncEngine is not running yet"  unless running?

    # NOTE: Cannot run _c_stop from a different thread than the thread in which AsyncEngine
    # is running (@_running_thread) since it would run uv_run in a different OS thread and
    # provoke a crash. So if we are calling stop on a different thread, raise an exception
    # in the AsyncEngine thread.
    if running? and Thread.current != @_running_thread and @_running_thread.alive?
      puts log + "NOTICE: calling stop() on a different thread. Calling to stop() in AE.next_tick instead."
      @_running_thread.raise AsyncEngine::StopFromOtherThreadException
      return nil
    end

    @_handles.each_value { |handle| handle.send :destroy }
    @_handles.clear
    @_blocks.clear
    @_next_ticks.clear

    _c_stop

    # Raise a custom AsyncEngine::Stop exception to avoid that new handles created
    # after calling stop() are loaded into UV. This exception is ignored by the run
    # function.
    raise AsyncEngine::StopException
  end

  # TODO: Test
  def self.ae_thread_killed
    puts "NOTICE: rb ae_thread_killed()"

    @_handles.each_value { |handle| handle.send :destroy }
    @_handles.clear
    @_blocks.clear
    @_next_ticks.clear

    asdasd rescue nil

    puts "NOTICE: rb ae_thread_killed() => exiting..."
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
    puts "\nDBG: AsyncEngine debug:"
    puts "- AE.running?: #{running?.inspect}"
    puts "- AE.num_handles: #{num_handles}"
    puts "- @_blocks: #{@_blocks.inspect}"
    puts "- @_handles: #{@_handles.inspect}"
    puts
  end

  def self.log
    "[AE thr:#{Thread.current}] "
  end

  class << self
    private :_c_run
    private :_c_stop
    private :handle_exception
  end

end


# Let's allow AE.xxxxx usage.
AE = AsyncEngine