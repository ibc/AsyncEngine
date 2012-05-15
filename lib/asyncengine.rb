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
      puts "RB NOTICE: AE.run() => AsyncEngine is already running, return nil"
      return nil
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
    rescue AsyncEngine::StopException => e
      stop rescue nil
      return true
    end

    @_running_thread = Thread.current

    begin
      if _c_run
        return true
      else
        # TODO: This should NEVER occur!
        raise AsyncEngine::Error, "AsyncEngine failed to run"
      end
    rescue AsyncEngine::StopException
      stop rescue nil
      return true
    rescue Exception => e
      stop rescue nil  # Ignore the AsyncEngine::StopException to be created.
      raise e # And raise the original exception.
    end
  end

  def self.stop
    ae_was_running = running?
    ae_thread = @_running_thread

    puts "--- AE.stop 1"
    @_handles.each_value { |handle| handle.send :destroy }
    @_handles.clear
    @_blocks.clear
    @_next_ticks.clear
    @_exception_handler = nil
    @_running_thread = nil

    # ESTE PUTO RAISE me estaba jodiendo, creo
    #raise AsyncEngine::StopException

    if Thread.current == ae_thread
      puts "AE.stop:  Thread.current == ae_thread  =>  raise molon"
      raise AsyncEngine::StopException
    end
    puts "--- AE.stop 2 (end)"
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

  class << self
    private :_c_run
    private :handle_exception
  end

end


# Let's allow AE.xxxxx usage.
AE = AsyncEngine